#include "RemoteControl.h"
#include <string.h>


/*************************************************************************************/
// объявление переменных
PutGet PutGet_1; // объявим переменную имя такое же, как и имя класса
PutGet PutGet_2; // объявим переменную имя такое же, как и имя класса
// ЗАМЕНИТЕ МАС-АДРЕСОМ ПЛАТЫ-ПОЛУЧАТЕЛЯ
uint8_t MACAddress[4][6] = {
    {0x9c, 0xcc, 0x01, 0x7c, 0x18, 0xac}, // Oled 24 
    {0x94, 0xa9, 0x90, 0x98, 0x08, 0xe8}, // Oled 13 
    {0xC0, 0x4E, 0x30, 0x55, 0xBD, 0x34},
    {0x48, 0xf6, 0xee, 0x61, 0xb8, 0x42} // здесь mac адрес свой собственный 48:f6:ee:61:b8:42
}; // адрес приемника (соседа)
StructGetData_1 GetData_1 = {};
StructPutData_1 PutData_1 = {};
StructGetData_2 GetData_2 = {};
StructPutData_2 PutData_2 = {};

/*************************************************************************************/
void PutGet::InitTmr(uint16_t TimeLinkOff, uint16_t TimePolling) // функция инициализации таймеров
{
  _TimeLinkOff = TimeLinkOff; // перекладываем таймер
  _TimePolling = TimePolling; // перекладываем таймер
}
/*************************************************************************************/

/*************************************************************************************/
// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  // Serial.print("\r\nLast Packet Send Status:\t");
  ; //  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
/*************************************************************************************/

/*************************************************************************************/
void PutGet::Cycle(uint16_t IndexDevice, uint16_t *CountLifeCurrent) // функция циклическая
{
  uint16_t i;
  // Проверим какой номер устройства ввели
  if ((IndexDevice < 0) or (IndexDevice > MAX_COUNT_RECEIVE))
  {
    i = 0;
  }
  else
  {
    i = IndexDevice;
  }

  // Serial.print("_CountLifePrev: ");
  // Serial.println(_CountLifePrev);
  CountLife.ReadDIn(false, _TimeLinkOff, 0, *CountLifeCurrent == _CountLifePrev);
  _CountLifePrev = *CountLifeCurrent; //
  // Serial.print("GetData_1.CountLife: ");
  // Serial.println(GetData_1.CountLife);
  // Serial.print("_CountLifePrev: ");
  // Serial.println(_CountLifePrev);

  if (!CountLife.Q) // если есть связь
  {
    LinkOk = true; // есть связь
  }
  else
  {
    LinkOk = false;           // нет связи
    if (_SettingsResetStruct) // если есть признак обнулить структуру при обрыве связи
    {
      switch (i)
      {
      case 0:
      {
        memset(&GetData_1 + sizeof(GetData_1.CountLife), 0, sizeof(GetData_1) - sizeof(GetData_1.CountLife));
        break;
      }
      case 1:
      {
        memset(&GetData_2 + sizeof(GetData_2.CountLife), 0, sizeof(GetData_2) - sizeof(GetData_2.CountLife));
        break;
      }
      case 2:
      {
        memset(&GetData_2 + sizeof(GetData_2.CountLife), 0, sizeof(GetData_2) - sizeof(GetData_2.CountLife));
        break;
      }
      }
    }
  }
  {
    // Подготовка отправки
    char MACAddrStr[18]; // объявим строковую переменную MAC адреса для сообщений
    snprintf(MACAddrStr, sizeof(MACAddrStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             MACAddress[i][0], MACAddress[i][1], MACAddress[i][2], MACAddress[i][3], MACAddress[i][4], MACAddress[i][5]);
    // Serial.print("Отправляем данные на ");
    // Serial.println(MACAddrStr); // печатаем MAC адрес
    if (PutData_1.NumPacket > MAX_QTY_PACKETS_OLED24 - 1) // если номер пакета стал больше максимального количества пакетов
    {
      PutData_1.NumPacket = 0; // обнулим номер пакета
    }
    if (PutData_2.NumPacket > MAX_QTY_PACKETS_OLED13 - 1) // если номер пакета стал больше максимального количества пакетов
    {
      PutData_2.NumPacket = 0; // обнулим номер пакета
    }
    esp_err_t SentOk; // объявим временную переменную
    switch (i)
    {
    case 0:
    {
      PutData_1.IndexWithCountLife.CountLife++;

      switch (PutData_1.NumPacket)
      {
      case 0:
      {
        SentOk = esp_now_send(MACAddress[i], (uint8_t *)&PutData_1.IndexWithCountLife, sizeof(PutData_1.IndexWithCountLife));
        // Serial.print("PutData_1.CountLife=");
        // Serial.println(PutData_1.IndexWithCountLife.CountLife);
        // Serial.print("sizeof(PutData_1.IndexWithCountLife)= ");
        // Serial.println(sizeof(PutData_1.IndexWithCountLife));
        delay(_TimePolling);
        PutData_1.NumPacket++;
        break;
      }
      default:
      {
        uint16_t idx = PutData_1.NumPacket - 1;
        PutData_1.StructArrScr[idx].IndexPacket = PutData_1.NumPacket;
        SentOk = esp_now_send(MACAddress[i], (uint8_t *)&PutData_1.StructArrScr[idx], sizeof(PutData_1.StructArrScr[idx]));
        delay(_TimePolling);
        PutData_1.NumPacket++;
        break;
      }
      }
      // Serial.print("sizeof(PutData_1)= ");
      // Serial.println(sizeof(PutData_1));

      if (SentOk == ESP_OK)
      {
        ;
        // Serial.println("Отправка данных успешно");
      }
      else
      {
        ;
        // Serial.println("Ошибка отправления данных");
      }
      break;
    }
    case 1:
    {
      switch (PutData_2.NumPacket)
      {
      case 0:
      {
        PutData_2.IndexWithCountLife.CountLife++;
        SentOk = esp_now_send(MACAddress[i], (uint8_t *)&PutData_2.IndexWithCountLife, sizeof(PutData_2.IndexWithCountLife));
        delay(_TimePolling);
        PutData_2.NumPacket++;
        // if (SentOk == ESP_OK)
        // {
        //   ;
        //   Serial.println("Отправка данных успешно");
        // }
        // else
        // {
        //   ;
        //   Serial.println("Ошибка отправления данных");
        // }
         break;
      }
      default:
      {
        uint16_t idx = PutData_2.NumPacket - 1;
      PutData_2.StructArrScr[idx].IndexPacket = PutData_2.NumPacket;
      SentOk = esp_now_send(MACAddress[i], (uint8_t *)&PutData_2.StructArrScr[idx], sizeof(PutData_2.StructArrScr[idx]));
      delay(_TimePolling);
      PutData_2.NumPacket++;
        // if (SentOk == ESP_OK)
        // {
        //   ;
        //   Serial.println("Отправка данных успешно");
        // }
        // else
        // {
        //   ;
        //   Serial.println("Ошибка отправления данных");
        // }
        break;
      }
      }
      break;
    }

    case 2:
    {
      esp_err_t SentOk = esp_now_send(
          MACAddress[i],
          (uint8_t *)&PutData_2,
          sizeof(PutData_2));
      if (SentOk == ESP_OK)
      {
        ;
        // Serial.println("Отправка данных успешно");
      }
      else
      {
        ;
        // Serial.println("Ошибка отправления данных");
      }
      break;
    }
    }
    //_TmrPolling.TONTmr(false, _TimePolling); // вызываем функцию таймера повтора опроса, чтобы сбросить его при сработке
  }
}
/*************************************************************************************/

/*************************************************************************************/
void PutGetInit() // функция инициализации
{
  //  Set device as a Wi-Fi Station
  Serial.println("PutGetInit()");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.setSleep(false); // Отключите Power Save Mode
  WiFi.setTxPower(WIFI_POWER_2dBm);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Ошибка инициализации ESP-NOW");
  }
  else
  {
    Serial.println("Инициализация ESP-NOW успешна");
  }

  // Регистрируем отправку сообщения
  esp_now_register_send_cb(OnDataSent);
  // Регистрируем прием сообщения
    esp_now_register_recv_cb(OnDataRecv);

  // Указываем получателя
  for (uint8_t i = 0; i < MAX_COUNT_RECEIVE; ++i)
  {
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, MACAddress[i], 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.print("Peer ");
      Serial.print(i);
      Serial.println(" not added");
    }
    else
    {
      Serial.print("Peer ");
      Serial.print(i);
      Serial.println(" added");
    }
  }
}
/*************************************************************************************/

// /*************************************************************************************/
// // callback function that will be executed when data is received
// void OnDataRecv(const uint8_t *MACAddr, const uint8_t *incomingData, int len)
// {
//   char MACAddrStr[18]; // объявим строковую переменную MAC адреса для сообщений
//   snprintf(MACAddrStr, sizeof(MACAddrStr), "%02x:%02x:%02x:%02x:%02x:%02x",
//            MACAddr[0], MACAddr[1], MACAddr[2], MACAddr[3], MACAddr[4], MACAddr[5]);
//   // Serial.print("Пришли данные с mac адреса ");
//   // Serial.println(MACAddrStr); // печатаем MAC адрес

//   uint16_t i, j;
//   // Проверяем с какого mac адреса пришло сообщение
//   for (int i = 0; i < MAX_COUNT_RECEIVE; i++)
//   {
//     if (memcmp(MACAddr, MACAddress[i], 6) == 0)
//     {
//       // Serial.print("Данный MAC адрес есть в системе под номером: ");
//       // Serial.println(i);

//       switch (i)
//       {
//       case 0:
//         memcpy(&GetData_1, incomingData, sizeof(GetData_1));
//         break;
//       case 1:
//         memcpy(&GetData_2, incomingData, sizeof(GetData_2));
//         break;
//       case 2:
//         memcpy(&GetData_2, incomingData, sizeof(GetData_2));
//         break;
//       }
//       return;
//     }
//   }
//   Serial.println("Но такого mac адреса в системе нет");
// }
// /*************************************************************************************/

/*************************************************************************************/
void PutGetCycle() // функция циклическая, вызываем в loop
{
  uint16_t i = 0;

  PutGet_1.Cycle(0, &GetData_1.CountLife);
  PutGet_2.Cycle(1, &GetData_2.CountLife);
}
/*************************************************************************************/
