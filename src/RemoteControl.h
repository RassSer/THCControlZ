#pragma once
#ifndef _REMOTECONTROL_H
#define _REMOTECONTROL_H

#include <Arduino.h>
#include "Timers.h"
#include "Input.h"
#include <esp_now.h>
#include <WiFi.h>

#define MAX_COUNT_RECEIVE 2           // максимальное количество приемников
#define DEFAULT_TIME_BOUNCE_BTN_MS 50 // Дефолтное время дребезга кнопок в милисекундах
#define MAX_QTY_PACKETS_OLED24 9      // максимальное количество посылок в oled24
#define MAX_QTY_PACKETS_OLED13 3      // максимальное количество посылок в oled13
#define MAX_STR_LEN 100
#ifndef MAX_STR_DISPLAY
#define MAX_STR_DISPLAY 8            // максимальное количество строк на одном экране
#endif

//--------------------------------------------
// Структура принятых данных с удаленного пульта
struct StructGetData_1
{
  uint16_t CountLife; // Счетчик жизни
};
//---------------

struct StructIndexWithStr
{
  uint16_t IndexPacket;     // номер пакета
  char ArrScr[MAX_STR_LEN]; // массив символов в строке
};

struct StructIndexWithCountLife
{
  uint16_t IndexPacket; // номер пакета
  uint16_t CountLife;   // Счетчик жизни
};

//--------------------------------------------
// Структура отправляемых данных
struct StructPutData_1
{
  uint16_t NumPacket; // номер пакета
  StructIndexWithCountLife IndexWithCountLife;
  StructIndexWithStr StructArrScr[MAX_STR_DISPLAY]; // массив строк на экран c номером пакета
};
//---------------

//--------------------------------------------
// Структура принятых данных с удаленного пульта
struct StructGetData_2
{
  uint16_t CountLife; // Счетчик жизни
};
//---------------

//--------------------------------------------
// Структура отправляемых данных
struct StructPutData_2
{
  uint16_t NumPacket; // номер пакета
  StructIndexWithCountLife IndexWithCountLife;
  StructIndexWithStr StructArrScr[MAX_STR_DISPLAY]; // массив строк на экран c номером пакета
};
//---------------

class PutGet
{
public:
  bool LinkOk;                                              // связь есть
  void Cycle(uint16_t IndexDevice, uint16_t *CountLife);    // цикл
  void InitTmr(uint16_t TimeLinkOff, uint16_t TimePolling); // функция инициализации таймеров

private:
  bool _SettingsResetStruct;    // настройка: обнулять данные при обрыве связи, кроме счетчика
  DIn CountLife;                // Проверка связи
  uint16_t _TimeLinkOff = 1000; // Время до выдачи признака отсутствия связи, мс
  uint16_t _TimePolling = 5;    // Время через которое будет совершаться следующая отправка пакета, мс
  uint16_t _CountLifePrev;      // счетчик жизни предыдущего скана
  TON _TmrPolling;              // Таймер повтора опроса
};

/*************************************************************************************/
// === Внешние объявления (определены в RemoteControl.cpp) ===
// объявление переменных
extern PutGet PutGet_1; // объявим переменную имя такое же, как и имя класса
extern PutGet PutGet_2; // объявим переменную имя такое же, как и имя класса
extern uint8_t MACAddress[4][6];
extern StructGetData_1 GetData_1;
extern StructPutData_1 PutData_1;
extern StructGetData_2 GetData_2;
extern StructPutData_2 PutData_2;
/*************************************************************************************/
void PutGetInit(); // функция инициализации
void OnDataRecv(const uint8_t *MACAddr, const uint8_t *incomingData, int len);
void PutGetCycle(); // функция циклическая, вызываем в loop
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
#endif