#include <Arduino.h>
#include "Timers.h"
#include "Input.h"
#include "Generator.h"
#include "ControlRaspred.h"
#include "EEManager.h"
#include "RunningAverage.h"
#include "Menu\Menu.h"
#include "GyverOLEDCustom.h"
#include "GyverOLED.h"
#include "GyverEncoder.h"
#include "RemoteControl.h"
#include "GyverDS18.h"
#include "FadeLamp.h"
#include "FadeVent.h"
#include "BlinkStrobStart.h"

#include "driver/pcnt.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "soc/pcnt_struct.h"
#include "driver/timer.h"

//------------------------------------------------------------
// константы определенные препроцессором
//------------------------------------------------------------
#define INIT_KEY 6 // ключ первого запуска. 0-254, на выбор

#define NOM_PIN_BLUE_LED 15 // номер Pin Синий светодиод

// #define NOM_PIN_OLED_SDA 35 // номер Pin OLED 2.4" SDA
// #define NOM_PIN_OLED_SCL 37 // номер Pin OLED 2.4" SCL

// #define NOM_PIN_OLED_MINI_SDA 38 // номер Pin OLED 1.3" SDA
// #define NOM_PIN_OLED_MINI_SCL 36 // номер Pin OLED 1.3" SCL

// --- НАСТРОЙКА ПИНОВ ---
#define PIN_TORCH_ON_INPUT = 12; 
#define PIN_ARC_OK_OUTPUT   = 13; 
#define PIN_RELAY_SELECT    = 14; 

#define THC_Z_STEP_PIN      = 25; 
#define THC_Z_DIR_PIN       = 26; 

#define PIN_ARC_VOLTAGE_ADC = 34; 

#define DEFAULT_TIME_BOUNCE_BTN_MS 50 // Дефолтное время дребезга кнопок в милисекундах

// --- НАСТРОЙКИ ДИНАМИЧЕСКОЙ СКОРОСТИ ---
const float Kp = 2.5;                 // Коэффициент усиления скорости (подбирается экспериментально)
const uint64_t MAX_SPEED_US = 120;    // Максимальная скорость Z (чем МЕНЬШЕ микросекунд, тем БЫСТРЕЕ)
const uint64_t MIN_SPEED_US = 800;    // Минимальная скорость Z при мелких корректировках


#define MS_100 20  // милисекунды для моргания
#define MS_300 20  // милисекунды для моргания
#define MS_500 500 // милисекунды для моргания

const unsigned long PIERCE_DELAY_MS = 600; 

enum THCState { IDLE, PIERCING, ACTIVE_CUTTING };
THCState currentState = IDLE;
unsigned long torchOnStartTime = 0;

// Определяем две шины I2C
// TwoWire Wire2 = TwoWire(1);

// GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled(0x3C, &Wire2);
// GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oledMini(0x3C, &Wire);
// GyverOLED<SSH1106_128x64> oledMini;
Encoder Enc(NOM_PIN_ENC_TOLEFT, NOM_PIN_ENC_TORIGHT, -1, TYPE2); // объявим энкодер класс

// Переменные таймера
hw_timer_t * zTimer = NULL;
volatile bool stepPhase = false;
volatile int stepDirection = 0; 
volatile uint64_t currentTimerPeriodUs = MIN_SPEED_US; // Текущий период таймера


#define COEF_TEMP 0.1 // коэффициент для фильтра 0.1 - 0.001, чем выше - тем резче

// Конфигурация PCNT
volatile uint64_t measuredIntervalEngine = 0;
volatile bool dataAvailableEngine = false;
volatile uint32_t CountImpulseEngine = 0; // Счетчик количества импульсов двигателя

volatile uint64_t measuredIntervalDriveshaft = 0;
volatile bool dataAvailableDriveshaft = false;
volatile uint32_t CountImpulseDriveshaft = 0; // Счетчик количества импульсов двигателя

//------------------------------------------------------------
// глобальные переменные
//------------------------------------------------------------

Screen ArrScr(7); // класс экранов

//------------------------------------------------------------
// Структура запоминаемых данных
//------------------------------------------------------------
struct StructRef
{
  float &TOil = Screens[0][0].Param;      // Ссылка. Т масла. Т масла
  float &TWater = Screens[0][1].Param;    // Ссылка. Т воды. Т воды
  float &TOutside = Screens[0][2].Param;  // Ссылка. Темп. нар. воздуха
  float &RPMEngine = Screens[3][0].Param; // Ссылка. Обороты двигателя, об/мин. Обороты двигателя, об/мин
  float &Velosity = Screens[4][0].Param;  // Ссылка. Скорость, км/час. Скорость, км/час

  float &TOilVentStart = Screens[1][1].Param;   // Ссылка. Т масла. T Включения
  float &TOilVentStop = Screens[1][2].Param;    // Ссылка. Т масла. T Отключения
  float &TOilMinRPMStart = Screens[1][3].Param; // Ссылка. Т масла. Мин. скорость
  float &TOilTMaxRPM = Screens[1][4].Param;     // Ссылка. Т масла. Максимальная температура, при которой максимаотная скорость вентилятора

  float &TWaterVent1Start = Screens[2][1].Param;       // Ссылка. Т воды. T Включения 1ого вент
  float &TWaterVent1Stop = Screens[2][2].Param;        // Ссылка. Т воды. T Отключения 1ого вент
  float &TWaterVent1MinRPMStart = Screens[2][3].Param; // Ссылка. Т воды. Мин. Скорость 1ого вент
  float &TWaterVent1TMaxRPM = Screens[2][4].Param;     // Ссылка. Т воды. Максимальная температура, при которой максимаотная скорость вентилятора

  bool &RPMEngineVisuMiniDisplay = Screens[3][1].BoolParam; // Ссылка. Обороты двигателя, об/мин. Отображать на маленьком дисплее
  float &RPMEngineKoefMul = Screens[3][2].Param;            // Ссылка. Обороты двигателя, об/мин. Коэффициент домножения

  bool &VelocityVisuMiniDisplay = Screens[4][1].BoolParam; // Ссылка. Скорость, км/час. Отображать на маленьком дисплее
  float &VelocityDiamWheel = Screens[4][2].Param;          // Ссылка. Скорость, км/час. Диаметр колеса, см
  float &VelocityKoefReduction = Screens[4][3].Param;      // Ссылка. Скорость, км/час. Редукция на мосте
  float &VelocityKoefMul = Screens[4][4].Param;            // Ссылка. Скорость, км/час. Коэффициент домножения

  bool &BlinkStart = Screens[5][2].BoolParam;    // Ссылка. Настройки. Моргание стробосками и поворотниками при старте
  float &TimeStrobOnStart = Screens[5][3].Param; // Ссылка. Настройки. Время включения стробоскопов, мсек
  float &TimeTurnOnStart = Screens[5][4].Param;  // Ссылка. Настройки. Время вкл.поворотников, мсек

  float &TimeTurnOn = Screens[6][1].Param;       // Ссылка. Поворотники. Время в отключенном состоянии, мс
  float &TimeTurnOff = Screens[6][2].Param;      // Ссылка. Поворотники. Время в включенном состоянии, мс
  float &TimeTurnOnBuzzer = Screens[6][3].Param; // Ссылка. Поворотники. Время выдачи звука на буззер, мс
};
//------------------------------------------------------------
StructRef Ref;
//------------------------------------------------------------

//------------------------------------------------------------
// добавляем в эту структуру всё что хотим сохранить в эрергонезависимой памяти
//------------------------------------------------------------
struct StructForMem
{
  float TOilVentStart;   // Т масла. T Включения
  float TOilVentStop;    // Т масла. T Отключения
  float TOilMinRPMStart; // Т масла. Мин. скорость
  float TOilTMaxRPM;     // Т масла. Максимальная температура, при которой максимаотная скорость вентилятора

  float TWaterVent1Start;       // Т воды. T Включения 1ого вент
  float TWaterVent1Stop;        // Т воды. T Отключения 1ого вент
  float TWaterVent1MinRPMStart; // Т воды. Мин. Скорость 1ого вент
  float TWaterVent1TMaxRPM;     // Т воды. Максимальная температура, при которой максимаотная скорость вентилятора

  bool RPMEngineVisuMiniDisplay; // Обороты двигателя, об/мин. Отображать на маленьком дисплее
  float RPMEngineKoefMul;        // Обороты двигателя, об/мин. Коэффициент домножения

  bool VelocityVisuMiniDisplay; // Скорость, км/час. Отображать на маленьком дисплее
  float VelocityDiamWheel;      // Скорость, км/час. Диаметр колеса, см
  float VelocityKoefReduction;  // Скорость, км/час. Редукция на мосте
  float VelocityKoefMul;        // Скорость, км/час. Коэффициент домножения

  bool BlinkStart;        // Настройки. Моргание стробосками и поворотниками при старте
  float TimeStrobOnStart; // Настройки. Время включения стробоскопов, мсек
  float TimeTurnOnStart;  // Настройки. Время вкл.поворотников, мсек

  float TimeTurnOn;       // Поворотники. Время в отключенном состоянии, мс
  float TimeTurnOff;      // Поворотники. Время в включенном состоянии, мс
  float TimeTurnOnBuzzer; // Поворотники. Время выдачи звука на буззер, мс
};

StructForMem FlashMemory;      // объявляем структуру для запоминания в памяти
EEManager Memory(FlashMemory); // передаём нашу переменную (фактически её адрес)
//------------------------------------------------------------

DIn BtnTurnLeft{false, DEFAULT_TIME_BOUNCE_BTN_MS};  // Кнопка. Поворотник левый
DIn BtnTurnRight{false, DEFAULT_TIME_BOUNCE_BTN_MS}; // Кнопка. Поворотник Правый
DIn BtnAlarm{false, DEFAULT_TIME_BOUNCE_BTN_MS};     // Кнопка. Аварийка
DIn BtnStrob{false, DEFAULT_TIME_BOUNCE_BTN_MS};     // Кнопка. Стробоскоп
DIn BtnEnc{false, DEFAULT_TIME_BOUNCE_BTN_MS};       // класс. Кнопка энкодера

DIn ReqDS(false, 1000, 0); // опрашивание DS

AIn FilterFreqEngine, FilterFreqDriveshaft;
AIn Joystick;

FadeLampX3 FadeTurnLeft, FadeTurnRight; // класс. плавного зажигания ламп поворотников

FadeVentilation FadeVentOil(MaxResolutionPWM);    // класс. Вентилятор масла
FadeVentilation FadeVentWater1(MaxResolutionPWM); // класс. Вентилятор воды 1

Generator Blink100x300ms;
Generator Blink500x500ms;
Generator BlinkTurn;
BlinkRepeatly BuzzerWarning;

uint16_t i, x;
TimeScan TimeScanCPU;

bool BtnMenuOk;        // кнопка меню Ok для Task дисплей
bool BtnMenuUp;        // кнопка меню Вверх для Task дисплей
bool BtnMenuDown;      // кнопка меню Вниз для Task дисплей
bool FirstScan = true; // признак первого скана

bool BeepTurn;              // выдача звука при поворотах
bool LampTurnLeftCanStart;  // Признак, что можем включать поворотник налево
bool LampTurnRightCanStart; // Признак, что можем включать поворотник направо

float kkk;

String ArrScrMini[2], ArrScrMiniNew[2];

BlinkRepeatly BlinkRepeatTwice;
BlinkRepeatly BlinkBlueSaveToFlash; // моргание синего диода при сохранении во флэш
BlinkStrob BlinkStrobTurn;          // класс. Моргания поворотников и стробоскопа при загрузке контроллера

void SaveValueToFlash(bool DefaultValue, bool FirstScan); // Функция сохранения/выгрузки значения из eeprom
void DOut();                                              // функция обработки дискретных выходов
void setup();
void loop();
void Display();
void DataToSend(); // Функция для формирования отправляемых данных
void TestLive();   // Функция для показания что жив CPU на дисплее
void ReservMemoryForScrDisplay();
void StrForOledMini(String *ResultStr, const String AddStr, uint16_t Value, uint16_t Length); // Функция для обрезки строк для OldeMini
// void IRAM_ATTR CountPulse();
float MapFloat(float x, float xmin, float xmax, float ymin, float ymax);

TON TmrCicle;
TON TmrBeepTurn;
TON TmrTurn;              // Таймер Звука для поворота
TaskHandle_t TaskDisplay; // Задача 1
TaskHandle_t Task2;       // Задача 2
TaskHandle_t TaskESPNow;  // Задача ESP-NOW
void TaskDisplayCicle(void *pvParameters);
void Task2code(void *pvParameters);
void TaskESPNowCicle(void *pvParameters);
void ApplyParam(uint16_t NumScr, uint16_t NumStr, float Value, uint16_t &Param, void (CtrlRaspred::*FuncInit)(StructConstCoord), CtrlRaspred &obj, StructConstCoord Coord);
void SaveParam(uint16_t NumScr, uint16_t NumStr); // Функция сохранения координат
void DefaultValueInit();                          // Функция для инициализации значений по умолчанию
//------------------------------------------------------------

//============================================================
// Вызов функции только один раз в момент загрузки
//============================================================
void setup()
{
  setlocale(LC_ALL, "en_US.UTF-8");
  Serial.begin(115200);
  delay(2000);
  Serial.println("========================================");
  Serial.println("SYSTEM STARTING...");

  // Причина сброса
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.print("Reset reason: ");
  Serial.println(reason);

  // // Дополнительная диагностика
  // Serial.print("Heap size: ");
  // Serial.println(ESP.getHeapSize());
  // Serial.print("Free heap: ");
  // Serial.println(ESP.getFreeHeap());
  // Serial.println("========================================");

  // установим разрешение для датчиков температур DS18
  TempOil.setResolution(12);
  TempWater.setResolution(12);
  TempOutside.setResolution(12);
  TempOil.requestTemp();     // запрос следующего измерения
  TempWater.requestTemp();   // запрос следующего измерения
  TempOutside.requestTemp(); // запрос следующего измерения

  // инициализируем номера pin на кнопках
  BtnTurnLeft.ModePin(NOM_PIN_BTN_TURN_LEFT);   // Кнопка. Поворотник левый
  BtnTurnRight.ModePin(NOM_PIN_BTN_TURN_RIGHT); // Кнопка. Поворотник Правый
  BtnAlarm.ModePin(NOM_PIN_BTN_ALARM);          // Кнопка. Аварийка
  BtnStrob.ModePin(NOM_PIN_BTN_STROB);          // Кнопка. Стробоскоп
  BtnEnc.ModePin(NOM_PIN_ENC_BTN);              // Кнопка. Энкодера"
  Enc.setTickMode(AUTO);
  BuzzerWarning.Init(1000, 1000, 4);

  FilterFreqEngine._KoeffFiltr = 3000;
  FilterFreqDriveshaft._KoeffFiltr = 3000;
  setCpuFrequencyMhz(240);

  ReservMemoryForScrDisplay();

  // Настройка каналов ШИМ
  ledcSetup(0, freq, resolution);        // Канал 0
  ledcAttachPin(NOM_PIN_FAN_OIL, 0);     // Привязка пина к каналу
  ledcSetup(1, freq, resolution);        // Канал 1
  ledcAttachPin(NOM_PIN_FAN_WATER_1, 1); // Привязка пина к каналу
  // ledcSetup(2, freq, resolution);        // Канал 2
  // ledcAttachPin(NOM_PIN_FAN_WATER_2, 2);     // Привязка пина к каналу
  // ledcSetup(3, freq, resolution);            // Канал 3
  // ledcAttachPin(NOM_PIN_DOUT_TURN_LEFT, 3);  // Привязка пина к каналу
  // ledcSetup(4, freq, resolution);            // Канал 4
  // ledcAttachPin(NOM_PIN_DOUT_TURN_RIGHT, 4); // Привязка пина к каналу

  EEPROM.begin(512);             // резервируем блок во флэш памяти
  DefaultValueInit();            // Функция для инициализации значений по умолчанию
  SaveValueToFlash(false, true); // Функция сохранения/выгрузки значения из eeprom

  BlinkStrobTurn.BlinkStart = Ref.BlinkStart; // переложим бит необходимости включения моргания стробоскопов и поворотников при загрузке
  BlinkStrobTurn.TimeBuzzer = 1000;
  BlinkStrobTurn.TimeStrob = Ref.TimeStrobOnStart;
  // BlinkStrobTurn.TimeStrob = 1000;
  BlinkStrobTurn.TimeTurn = Ref.TimeTurnOnStart;
  // BlinkStrobTurn.TimeTurn = 1000;

  Serial.print("=Размер занятой памяти в EEPROM: ");
  Serial.print(Memory.blockSize());
  Serial.println(" Байт=");

  //------------------------------------------------------------
  // Инициализация Pin и другого
  //------------------------------------------------------------
  pinMode(NOM_PIN_BLUE_LED, OUTPUT);         // выход Синий светодиод
  pinMode(NOM_PIN_BUZZER, OUTPUT);           // Выход. Буззер
  pinMode(NOM_PIN_DOUT_STROB_LEFT, OUTPUT);  // Выход.
  pinMode(NOM_PIN_DOUT_STROB_RIGHT, OUTPUT); // Выход.
  pinMode(NOM_PIN_DOUT_TURN_LEFT, OUTPUT);   // Выход.
  pinMode(NOM_PIN_DOUT_TURN_RIGHT, OUTPUT);  // Выход.

  // //  Инициализируем шины
  // Wire2.begin(NOM_PIN_OLED_SDA, NOM_PIN_OLED_SCL);          // SDA, SCL для первой шины
  // Wire.begin(NOM_PIN_OLED_MINI_SDA, NOM_PIN_OLED_MINI_SCL); // SDA, SCL для второй шины

  // oled.init();
  // oled.clear();
  // oled.setCursor(0, 0);
  // oled.setScale(1);

  // oledMini.init();
  // oledMini.clear();
  // oledMini.setScale(2);
  // oledMini.setCursor(0, 0);

  Serial.print("Свободно памяти: ");
  Serial.println(ESP.getFreeHeap());

  Serial.print("Макс. непрерывный блок: ");
  Serial.println(ESP.getMaxAllocHeap());

  BlinkRepeatTwice.Init(200, 300, 5);   // 200мс горит, 300мс не горит, и так 5 раз
  BlinkBlueSaveToFlash.Init(50, 70, 5); // 100мс горит, 200мс не горит, и так 2 раза моргание синего диода при сохранении во флэш

  xTaskCreatePinnedToCore(
      TaskDisplayCicle, /* Функция для задачи */
      "TaskDisplay",    /* Имя задачи */
      1024,             /* Размер стека */
      NULL,             /* Параметр задачи */
      2,                /* Приоритет */
      &TaskDisplay,     /* Выполняемая операция */
      0);               /* Номер ядра, на котором она должна выполняться */

  xTaskCreatePinnedToCore(
      Task2code, /* Функция для задачи */
      "Task2",   /* Имя задачи */
      2048,      /* Размер стека */
      NULL,      /* Параметр задачи */
      3,         /* Приоритет */
      &Task2,    /* Выполняемая операция */
      0);        /* Номер ядра, на котором она должна выполняться */

  xTaskCreatePinnedToCore(
      TaskESPNowCicle, /* Функция для задачи */
      "TaskESPNow",    /* Имя задачи */
      4096,            /* Размер стека */
      NULL,            /* Параметр задачи */
      3,               /* Приоритет */
      &TaskESPNow,     /* Выполняемая операция */
      0);              /* Номер ядра, на котором она должна выполняться */
}

//------------------------------------------------------------
// обработчик прерываний по таймеру для формирования импульсов на шаговом двигателе
//------------------------------------------------------------
void IRAM_ATTR onTimer() {
  if (stepDirection == 0) return;
  
  if (!stepPhase) {
    digitalWrite(THC_Z_DIR_PIN, stepDirection == 1 ? HIGH : LOW);
    digitalWrite(THC_Z_STEP_PIN, HIGH);
    stepPhase = true;
  } else {
    digitalWrite(THC_Z_STEP_PIN, LOW);
    stepPhase = false;
  }
}


//------------------------------------------------------------
// Функция циклически вызываемая по приоритету
//------------------------------------------------------------
void TaskDisplayCicle(void *pvParameters)
{
  for (;;)
  {
 

    //  Переложим на другие экраны
    Screens[0][0].Param = Screens[1][0].Param = Ref.TOil;    // Температура масла
    Screens[1][0].QualityParam = Screens[0][0].QualityParam; // Качество. Температура масла
    Screens[0][1].Param = Screens[2][0].Param = Ref.TWater;  // Температура воды
    Screens[2][0].QualityParam = Screens[0][1].QualityParam; // Качество. Температура воды

    //  Переложим значения с экрана в функцию управления вентилятором
    FadeVentOil.TemperatureStart = Ref.TOilVentStart;
    FadeVentOil.TemperatureStop = Ref.TOilVentStop;
    FadeVentOil.MaxTemperature = Ref.TOilTMaxRPM;
    FadeVentOil.MinRPMStart = Ref.TOilMinRPMStart;
    FadeVentWater1.TemperatureStart = Ref.TWaterVent1Start;
    FadeVentWater1.TemperatureStop = Ref.TWaterVent1Stop;
    FadeVentWater1.MaxTemperature = Ref.TWaterVent1TMaxRPM;
    FadeVentWater1.MinRPMStart = Ref.TWaterVent1MinRPMStart;

    ArrScr.RefreshData(BtnMenuOk, BtnMenuDown, BtnMenuUp);
    if (BtnMenuOk)
    {
      BtnMenuOk = false;
    } // кнопка меню Ok для Task дисплей
    if (BtnMenuUp)
    {
      BtnMenuUp = false; // кнопка меню Вверх для Task дисплей
    }
    if (BtnMenuDown)
      BtnMenuDown = false; // кнопка меню Вниз для Task дисплей

    //------------------------------------------------------------
    // возврат к заводским настройкам
    //------------------------------------------------------------
    if (Screens[5][5].ExitFromParam and Screens[5][5].BoolParam) // при выходе из параметра возврата к заводским настройкам
    {
      Screens[5][5].BoolParam = false;
      SaveValueToFlash(true, false);
    }
    Display();

    delay(10);
  }
  vTaskDelete(NULL);
  //------------------------------------------------------------
}
//------------------------------------------------------------

//------------------------------------------------------------
// Функция циклически вызываемая по приоритету для ESP NOW
//------------------------------------------------------------
void TaskESPNowCicle(void *pvParameters)
{
  PutGetInit();

  for (;;)
  {
    ;
    // Serial.println("TaskESPNow running...");
    PutGetCycle();
    delay(20);
    // Инициализация мьютекса
  }
  vTaskDelete(NULL);
}
//------------------------------------------------------------

//------------------------------------------------------------
// Функция прорисовки на OLED
//------------------------------------------------------------
void Display()
{
  // oled.setCursor(0, 0);
  for (x = 0; x < MAX_STR_DISPLAY; x++)
  {
    // if (ArrScr.NeedUpdateStrDisplay[x])
    {
      ;
      // oled.setCursor(0, x);
      // oled.print(ArrScr.StrForLCD[x]);

      strncpy(PutData_1.StructArrScr[x].ArrScr, ArrScr.StrForLCD[x].c_str(), MAX_STR_LEN - 1);
      PutData_1.StructArrScr[x].ArrScr[MAX_STR_LEN - 1] = '\0'; // гарантия завершения
      // Serial.print("PutData_1.ArrScr[");
      // Serial.print(x);
      // Serial.print("] = ");
      // Serial.print(PutData_1.StructArrScr[x].ArrScr);
      // Serial.print(";");
      // Serial.println(sizeof(PutData_1.StructArrScr[x].ArrScr));
    }
  }
  // Serial.println(sizeof(PutData_1));
}
//------------------------------------------------------------

//------------------------------------------------------------
// Функция loop
//------------------------------------------------------------
void loop()
{
  if (Enc.isLeft())
  {
    BtnMenuDown = true;
  }

  // Serial.print("BtnMenuDown = ");
  // Serial.println(BtnMenuDown);

  if (Enc.isRight())
  {
    BtnMenuUp = true;
  }
  // Serial.print("BtnMenuUp = ");
  // Serial.println(BtnMenuUp);
  //------------------------------------------------------------
  // вызов функции сохранения параметров
  //------------------------------------------------------------
  SaveParam(5, 1); // вызов функции сохранения параметров
                   //------------------------------------------------------------
  vTaskDelay(pdMS_TO_TICKS(1));
}
//------------------------------------------------------------

/*************************************************************************************/
// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *MACAddr, const uint8_t *incomingData, int len)
{
  if (!MACAddr || !incomingData || len <= 0)
    return;
  char MACAddrStr[18]; // объявим строковую переменную MAC адреса для сообщений
  snprintf(MACAddrStr, sizeof(MACAddrStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           MACAddr[0], MACAddr[1], MACAddr[2], MACAddr[3], MACAddr[4], MACAddr[5]);
  // Serial.print("Пришли данные с mac адреса ");
  // Serial.println(MACAddrStr); // печатаем MAC адрес

  uint16_t i, j;
  // Проверяем с какого mac адреса пришло сообщение
  for (int i = 0; i < MAX_COUNT_RECEIVE; i++)
  {
    if (memcmp(MACAddr, MACAddress[i], 6) == 0)
    {
      // Serial.print("Данный MAC адрес есть в системе под номером: ");
      // Serial.println(i);

      switch (i)
      {
      case 0:
        if (len < (int)sizeof(GetData_1))
          return;
        memcpy(&GetData_1, incomingData, sizeof(GetData_1));
        // Serial.print("GetData_1.ValueForwardBackward = ");
        // Serial.println(GetData_1.ValueForwardBackward);

        break;
      case 1:
        if (len < (int)sizeof(GetData_2))
          return;
        memcpy(&GetData_2, incomingData, sizeof(GetData_2));
        break;
      case 2:
        if (len < (int)sizeof(GetData_2))
          return;
        memcpy(&GetData_2, incomingData, sizeof(GetData_2));
        break;
      }
      return;
    }
  }
  Serial.println("Но такого mac адреса в системе нет");
}
/*************************************************************************************/

//============================================================
// Функция настройки таймера прерывания для подсчета об/мин двигателя и карданного вала
//============================================================
void setup_timer_hardware()
{
  timer_config_t config = {
      .alarm_en = TIMER_ALARM_EN,
      .counter_en = TIMER_PAUSE,
      .counter_dir = TIMER_COUNT_UP,
      .auto_reload = TIMER_AUTORELOAD_EN,
      .divider = 80 // 1 тик = 1 мкс при 80 МГц
  };

  timer_init(TIMER_GROUP_0, TIMER_0, &config);
  timer_init(TIMER_GROUP_0, TIMER_1, &config);

  // Устанавливаем порог срабатывания для таймера двигателя(например, через 5 секунд)
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIME_RPM_RESET);
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, timer_isr_engine, NULL, 0);
  timer_start(TIMER_GROUP_0, TIMER_0);

  // Устанавливаем порог срабатывания для таймера карданного вала(например, через 5 секунд)
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_1, TIME_RPM_RESET);
  timer_enable_intr(TIMER_GROUP_0, TIMER_1);
  timer_isr_callback_add(TIMER_GROUP_0, TIMER_1, timer_isr_driveshaft, NULL, 0);
  timer_start(TIMER_GROUP_0, TIMER_1);
}
//============================================================

//============================================================
// Универсальная функция настройки PCNT
//============================================================
void setup_pcnt(pcnt_unit_t unit, int pulse_pin, const char *name)
{
  // Настройка PCNT
  pcnt_config_t config = {
      .pulse_gpio_num = pulse_pin,
      .ctrl_gpio_num = PCNT_PIN_NOT_USED,
      .lctrl_mode = PCNT_MODE_KEEP,
      .hctrl_mode = PCNT_MODE_KEEP,
      .pos_mode = PCNT_COUNT_INC,
      .neg_mode = PCNT_COUNT_DIS,
      .counter_h_lim = 100, // Важно: прерывание на 1 импульсе
      .counter_l_lim = 0,
      .unit = unit,
      .channel = PCNT_CHANNEL_0};

  esp_err_t err = pcnt_unit_config(&config);
  if (err != ESP_OK)
  {
    Serial.printf("ERROR: pcnt_unit_config failed for %s: %d\n", name, err);
    while (1)
      ;
  }

  // 4. ВАЖНО: Настраиваем фильтр ДО установки значений
  pcnt_set_filter_value(unit, 1000);
  pcnt_filter_enable(unit);

  // 5. Установка пороговых значений для прерываний
  pcnt_set_event_value(unit, PCNT_EVT_THRES_1, 1);

  // 6. Включение событий для прерываний
  pcnt_event_enable(unit, PCNT_EVT_THRES_1);

  // 7. Отключаем лишние прерывания
  pcnt_event_disable(unit, PCNT_EVT_ZERO);

  // 8. Включение прерываний для этого юнита
  err = pcnt_intr_enable(unit);
  if (err != ESP_OK)
  {
    Serial.printf("ERROR: pcnt_intr_enable failed for %s: %d\n", name, err);
    while (1)
      ;
  }
  // 9. ПРАВИЛЬНЫЙ запуск счетчика: пауза -> очистка -> запуск
  pcnt_counter_pause(unit);
  pcnt_counter_clear(unit);
  pcnt_counter_resume(unit);

  // 10. Дополнительно: настраиваем пин как вход
  pinMode(pulse_pin, INPUT_PULLDOWN);

  Serial.printf("PCNT %s ready on GPIO %d\n", name, pulse_pin);
}

//============================================================
// Инициализация всех PCNT модулей
//============================================================
void setup_all_pcnts()
{
  // Настраиваем модуль двигателя
  setup_pcnt(PCNT_UNIT_ENGINE, PIN_PULSE_ENGINE, "Engine");

  // Настраиваем модуль карданного вала
  setup_pcnt(PCNT_UNIT_DRIVESHAFT, PIN_PULSE_DRIVESHAFT, "DRIVESHAFT");

  // Регистрируем ОДИН обработчик прерываний для всех модулей
  esp_err_t err = pcnt_isr_register(pcnt_isr_handler, NULL, 0, NULL);
  if (err != ESP_OK)
  {
    Serial.printf("ERROR: pcnt_isr_register failed: %d\n", err);
    while (1)
      ;
  }
  Serial.println("\n=== SYSTEM READY ===");
}

//============================================================
// Функция. Подсчет об/мин
//============================================================
float CountRPM(const volatile uint64_t &measuredInterval, volatile bool &dataAvailable, AIn &FilterFreq)
{
  uint64_t intervalLocal;
  bool availableLocal;

  noInterrupts();
  intervalLocal = measuredInterval;
  availableLocal = dataAvailable;
  interrupts();
  dataAvailable = false;

  // Serial.println("\n=== VALID MEASUREMENT ===");
  // Serial.printf("Счетчик:             %u\n", CountImpulseEngine);
  // Serial.printf("measuredInterval:    %llu microseconds\n", measuredInterval);
  // Serial.printf("measuredInterval:    %.4f milliseconds\n", measuredInterval / 1000.0);

  float FreqRotate;
  float RPM;

  if (availableLocal && intervalLocal != 0ULL)
  {
    FreqRotate = 1000.0 / (measuredInterval / 1000.0);
    RPM = FilterFreq.Filtr1th(FreqRotate * 60.0);
  }
  else
  {
    FreqRotate = 0.0;
    RPM = 0.0;
  }

  // Serial.printf("Частота:             %.4f Hz\n", FreqRotate);
  // Serial.printf("RPM:                 %.4f об/мин \n", RPM);

  // Serial.println("=========================\n");
  return RPM;
}

//============================================================
// Вызов функции циклически
//============================================================
void Task2code(void *pvParameters)
{
  setup_timer_hardware(); // инициализация таймеров прерывания
  setup_all_pcnts();      // инициализация PCNT

  for (;;)
  {
    TestLive();

    // опросим кнопки
    BtnTurnLeft.ReadDIn();
    BtnTurnRight.ReadDIn();
    BtnAlarm.ReadDIn();
    BtnStrob.ReadDIn();
    BtnEnc.ReadDIn();
    // Enc.tick(); // вызываем функцию считывания показаний энкодера

    // Serial.print("BtnEnc.Q = ");
    // Serial.println(BtnEnc.Q);
    if (BtnEnc.QOnce())
    {
      BtnMenuOk = true;
      // Serial.print("BtnMenuOk = ");
      // Serial.println(BtnMenuOk);
    }

    if (dataAvailableEngine)
    {
      // Вызов функции подсчета об/мин для двигателя
      Screens[0][3].Param = Ref.RPMEngine = CountRPM(measuredIntervalEngine, dataAvailableEngine, FilterFreqEngine);
      // Serial.print("Ref.RPMEngine = ");
      // Serial.println(Ref.RPMEngine);
    }
    if (dataAvailableDriveshaft)
    { // Вызов функции подсчета об/мин для двигателя
      // диаметр колеса в см умножаем на 60/10000 (60 минут в часе, 100000см в км )
      Ref.VelocityKoefReduction = 3.5;
      Screens[0][4].Param = Ref.Velosity = Ref.VelocityDiamWheel * 0.0006 * CountRPM(measuredIntervalDriveshaft, dataAvailableDriveshaft, FilterFreqDriveshaft) / Ref.VelocityKoefReduction;
      // Serial.print("Ref.Velosity = ");
      // Serial.println(Ref.Velosity);
    }

    // При старте запуск моргания стробоскопов, поворотников
    BlinkStrobTurn.Blink();

    // Поворотники
    if (!BlinkTurn.Q and !BlinkStrobTurn.BlinkStart) // если Лампа повортников выключена
    {
      if (BtnTurnLeft.Q) // если Нажата кнопка поворот налево
      {
        LampTurnLeftCanStart = true; // запоминаем что можем включать поворотник налево
      }
      else // если не Нажата кнопка поворот налево
      {
        LampTurnLeftCanStart = false; // сбрасываем что можем включать поворотник налево
      }
      if (BtnTurnRight.Q) // если Нажата кнопка поворот направо
      {
        LampTurnRightCanStart = true; // запоминаем что можем включать поворотник направо
      }
      else // если не Нажата кнопка поворот направо
      {
        LampTurnRightCanStart = false; // сбрасываем что можем включать поворотник направо
      }
      if (BtnAlarm.Q) // если Нажата кнопка Аварийка
      {
        LampTurnLeftCanStart = true;  // запоминаем что можем включать поворотник налево
        LampTurnRightCanStart = true; // запоминаем что можем включать поворотник направо
      }
    }

    BlinkTurn.CmdReset = !LampTurnLeftCanStart and !LampTurnRightCanStart; // если не включены, сбрасываем флаг моргания

    BlinkTurn.Blink(Ref.TimeTurnOff, Ref.TimeTurnOn);

    TmrBeepTurn.TONTmr(BlinkTurn.Q, Ref.TimeTurnOnBuzzer); // Таймер выдачи звука при работе поворотников

    if ((!TmrBeepTurn.Q and BlinkTurn.Q) and (LampTurnLeftCanStart or LampTurnRightCanStart))
    {
      BeepTurn = true;
    }
    else
    {
      BeepTurn = false;
    }

    //------------------------------------------------------------
    // вызов функций моргания, .Q функции моргает с определенной частотой добавляем здесь другие моргалки, далее будем их использовать
    //------------------------------------------------------------
    Blink100x300ms.Blink(MS_100, MS_300);
    Blink500x500ms.Blink(MS_500, MS_500);
    //------------------------------------------------------------

    //------------------------------------------------------------
    // вызов функции повтора моргания
    //------------------------------------------------------------
    BlinkRepeatTwice.Cycle();
    BlinkBlueSaveToFlash.Cycle();
    //------------------------------------------------------------

    //------------------------------------------------------------
    // Функция для формирования отправляемых данных
    //------------------------------------------------------------
    // DataToSend();
    //------------------------------------------------------------

    //------------------------------------------------------------
    // вызов функции обработки дискретных выходов
    //------------------------------------------------------------
    DOut();
    //------------------------------------------------------------
    FirstScan = false; // признак первого скана
    delay(50);
  }
  vTaskDelete(NULL);
}

//------------------------------------------------------------
// Функция обработки выходов, все выхода пишем тут они в конце каждого цикла будут отрабатывать
//------------------------------------------------------------
void DOut()
{
  // Выход синего диода
  digitalWrite(NOM_PIN_BLUE_LED, Blink100x300ms.Q /*BtnJoystick[1].Q*/);

  // Выход активного Буззера
  digitalWrite(NOM_PIN_BUZZER, (BeepTurn and !BlinkStrobTurn.BlinkStart) or BlinkStrobTurn.BuzzerOn);

  // Выход левого поворотника
  digitalWrite(NOM_PIN_DOUT_TURN_LEFT, (BlinkStrobTurn.BlinkStart and BlinkStrobTurn.TurnOn) or (LampTurnLeftCanStart and BlinkTurn.Q and !BlinkStrobTurn.BlinkStart));

  // Выход правого поворотника
  digitalWrite(NOM_PIN_DOUT_TURN_RIGHT, (BlinkStrobTurn.BlinkStart and BlinkStrobTurn.TurnOn) or (!BlinkStrobTurn.BlinkStart and BlinkTurn.Q and LampTurnRightCanStart));

  // Выход левого стробоскопа
  digitalWrite(NOM_PIN_DOUT_STROB_LEFT, (!BlinkStrobTurn.BlinkStart and BtnStrob.Q and !LampTurnLeftCanStart and !LampTurnRightCanStart) or (BlinkStrobTurn.BlinkStart and BlinkStrobTurn.StrobOn));

  // Выход правого стробоскопа
  digitalWrite(NOM_PIN_DOUT_STROB_RIGHT, (!BlinkStrobTurn.BlinkStart and BtnStrob.Q and !LampTurnLeftCanStart and !LampTurnRightCanStart) or (BlinkStrobTurn.BlinkStart and BlinkStrobTurn.StrobOn));
}

//------------------------------------------------------------
// Функция обработки сообщений, все сообщения пишем тут они в конце каждого цикла будут отрабатывать
//------------------------------------------------------------
void Msg()
{
  ;
}

//------------------------------------------------------------
// Функция сохранения значений на флэш
//------------------------------------------------------------
void SaveValueToFlash(bool DefaultValue, bool FirstScan)
{
  delay(100);
  if (FirstScan) // если начальная загрузка
  {

    // FlashMemory.ConstRaspredTypeMove = CoordTypeMove.Raspred; // переложим в структуру флэш памяти

    // проиницилизируем, если первый запуск
    uint8_t ResultMemory = (Memory.begin(0, INIT_KEY));
    Serial.print("ResultMemory = ");
    Serial.println(ResultMemory);

    // проиницилизируем, если первый запуск
    if ((ResultMemory == 0) or (ResultMemory == 1)) // если удачная работа функции
    {
      Ref.TOilVentStart = FlashMemory.TOilVentStart;     // Т масла. T Включения
      Ref.TOilVentStop = FlashMemory.TOilVentStop;       // Т масла. T Отключения
      Ref.TOilMinRPMStart = FlashMemory.TOilMinRPMStart; // Т масла. Мин. скорость
      Ref.TOilTMaxRPM = FlashMemory.TOilTMaxRPM;         // Т масла. Максимальная температура, при которой максимаотная скорость вентилятора

      Ref.TWaterVent1Start = FlashMemory.TWaterVent1Start;             // Т воды. T Включения 1ого вент
      Ref.TWaterVent1Stop = FlashMemory.TWaterVent1Stop;               // Т воды. T Отключения 1ого вент
      Ref.TWaterVent1MinRPMStart = FlashMemory.TWaterVent1MinRPMStart; // Т воды. Мин. Скорость 1ого вент
      Ref.TWaterVent1TMaxRPM = FlashMemory.TWaterVent1TMaxRPM;         // Т воды. Максимальная температура, при которой максимаотная скорость вентилятора

      Ref.RPMEngineVisuMiniDisplay = FlashMemory.RPMEngineVisuMiniDisplay; // Обороты двигателя, об/мин. Отображать на маленьком дисплее
      Ref.RPMEngineKoefMul = FlashMemory.RPMEngineKoefMul;                 // Обороты двигателя, об/мин. Коэффициент домножения

      Ref.VelocityVisuMiniDisplay = FlashMemory.VelocityVisuMiniDisplay; // Скорость, км/час. Отображать на маленьком дисплее
      Ref.VelocityDiamWheel = FlashMemory.VelocityDiamWheel;             // Скорость, км/час. Диаметр колеса, см
      Ref.VelocityKoefReduction = FlashMemory.VelocityKoefReduction;     // Скорость, км/час. Редукция на мосте
      Ref.VelocityKoefMul = FlashMemory.VelocityKoefMul;                 // Скорость, км/час. Коэффициент домножения

      Ref.BlinkStart = FlashMemory.BlinkStart;             // Настройки. Моргание стробосками и поворотниками при старте
      Ref.TimeStrobOnStart = FlashMemory.TimeStrobOnStart; // Настройки. Время включения стробоскопов, мсек
      Ref.TimeTurnOnStart = FlashMemory.TimeTurnOnStart;   // Настройки. Время вкл.поворотников, мсек

      Ref.TimeTurnOn = FlashMemory.TimeTurnOn;             // Поворотники. Время в отключенном состоянии, мс
      Ref.TimeTurnOff = FlashMemory.TimeTurnOff;           // Поворотники. Время в включенном состоянии, мс
      Ref.TimeTurnOnBuzzer = FlashMemory.TimeTurnOnBuzzer; // Поворотники. Время выдачи звука на буззер, мс
    }
  }
  else if (DefaultValue) // инициализация значениями по умолчанию
  {
    DefaultValueInit();

    Ref.TOilVentStart = FlashMemory.TOilVentStart;     // Т масла. T Включения
    Ref.TOilVentStop = FlashMemory.TOilVentStop;       // Т масла. T Отключения
    Ref.TOilMinRPMStart = FlashMemory.TOilMinRPMStart; // Т масла. Мин. скорость
    Ref.TOilTMaxRPM = FlashMemory.TOilTMaxRPM;         // Т масла. Максимальная температура, при которой максимаотная скорость вентилятора

    Ref.TWaterVent1Start = FlashMemory.TWaterVent1Start;             // Т воды. T Включения 1ого вент
    Ref.TWaterVent1Stop = FlashMemory.TWaterVent1Stop;               // Т воды. T Отключения 1ого вент
    Ref.TWaterVent1MinRPMStart = FlashMemory.TWaterVent1MinRPMStart; // Т воды. Мин. Скорость 1ого вент
    Ref.TWaterVent1TMaxRPM = FlashMemory.TWaterVent1TMaxRPM;         // Т воды. Максимальная температура, при которой максимаотная скорость вентилятора

    Ref.RPMEngineVisuMiniDisplay = FlashMemory.RPMEngineVisuMiniDisplay; // Обороты двигателя, об/мин. Отображать на маленьком дисплее
    Ref.RPMEngineKoefMul = FlashMemory.RPMEngineKoefMul;                 // Обороты двигателя, об/мин. Коэффициент домножения

    Ref.VelocityVisuMiniDisplay = FlashMemory.VelocityVisuMiniDisplay; // Скорость, км/час. Отображать на маленьком дисплее
    Ref.VelocityDiamWheel = FlashMemory.VelocityDiamWheel;             // Скорость, км/час. Диаметр колеса, см
    Ref.VelocityKoefReduction = FlashMemory.VelocityKoefReduction;     // Скорость, км/час. Редукция на мосте
    Ref.VelocityKoefMul = FlashMemory.VelocityKoefMul;                 // Скорость, км/час. Коэффициент домножения

    Ref.BlinkStart = FlashMemory.BlinkStart;             // Настройки. Моргание стробосками и поворотниками при старте
    Ref.TimeStrobOnStart = FlashMemory.TimeStrobOnStart; // Настройки. Время включения стробоскопов, мсек
    Ref.TimeTurnOnStart = FlashMemory.TimeTurnOnStart;   // Настройки. Время вкл.поворотников, мсек

    Ref.TimeTurnOn = FlashMemory.TimeTurnOn;             // Поворотники. Время в отключенном состоянии, мс
    Ref.TimeTurnOff = FlashMemory.TimeTurnOff;           // Поворотники. Время в включенном состоянии, мс
    Ref.TimeTurnOnBuzzer = FlashMemory.TimeTurnOnBuzzer; // Поворотники. Время выдачи звука на буззер, мс
    // Memory.updateNow();                                  // запишем во флэш памяти
  }
  else // просто сохраним во флэш память
  {
    FlashMemory.TOilVentStart = Ref.TOilVentStart;     // Т масла. T Включения
    FlashMemory.TOilVentStop = Ref.TOilVentStop;       // Т масла. T Отключения
    FlashMemory.TOilMinRPMStart = Ref.TOilMinRPMStart; // Т масла. Мин. скорость
    FlashMemory.TOilTMaxRPM = Ref.TOilTMaxRPM;         // Т масла. Максимальная температура, при которой максимаотная скорость вентилятора

    FlashMemory.TWaterVent1Start = Ref.TWaterVent1Start;             // Т воды. T Включения 1ого вент
    FlashMemory.TWaterVent1Stop = Ref.TWaterVent1Stop;               // Т воды. T Отключения 1ого вент
    FlashMemory.TWaterVent1MinRPMStart = Ref.TWaterVent1MinRPMStart; // Т воды. Мин. Скорость 1ого вент
    FlashMemory.TWaterVent1TMaxRPM = Ref.TWaterVent1TMaxRPM;         // Т воды. Максимальная температура, при которой максимаотная скорость вентилятора

    FlashMemory.RPMEngineVisuMiniDisplay = Ref.RPMEngineVisuMiniDisplay; // Обороты двигателя, об/мин. Отображать на маленьком дисплее
    FlashMemory.RPMEngineKoefMul = Ref.RPMEngineKoefMul;                 // Обороты двигателя, об/мин. Коэффициент домножения

    FlashMemory.VelocityVisuMiniDisplay = Ref.VelocityVisuMiniDisplay; // Скорость, км/час. Отображать на маленьком дисплее
    FlashMemory.VelocityDiamWheel = Ref.VelocityDiamWheel;             // Скорость, км/час. Диаметр колеса, см
    FlashMemory.VelocityKoefReduction = Ref.VelocityKoefReduction;     // Скорость, км/час. Редукция на мосте
    FlashMemory.VelocityKoefMul = Ref.VelocityKoefMul;                 // Скорость, км/час. Коэффициент домножения

    FlashMemory.BlinkStart = Ref.BlinkStart;             // Настройки. Моргание стробосками и поворотниками при старте
    FlashMemory.TimeStrobOnStart = Ref.TimeStrobOnStart; // Настройки. Время включения стробоскопов, мсек
    FlashMemory.TimeTurnOnStart = Ref.TimeTurnOnStart;   // Настройки. Время вкл.поворотников, мсек

    FlashMemory.TimeTurnOn = Ref.TimeTurnOn;             // Поворотники. Время в отключенном состоянии, мс
    FlashMemory.TimeTurnOff = Ref.TimeTurnOff;           // Поворотники. Время в включенном состоянии, мс
    FlashMemory.TimeTurnOnBuzzer = Ref.TimeTurnOnBuzzer; // Поворотники. Время выдачи звука на буззер, мс
    Memory.updateNow();                                  // запишем во флэш памяти
  }
}

//------------------------------------------------------------
// Функция для формирования отправляемых данных
//------------------------------------------------------------
void DataToSend()
{
  ;
}

//------------------------------------------------------------
// Функция сохранения параметров
//------------------------------------------------------------
void SaveParam(uint16_t NumScr, uint16_t NumStr)
{
  bool &ExitSaveCalibr = Screens[NumScr][NumStr].ExitFromParam; // объявим ссылку - что вышли из параметра
  bool &SaveCalibr = Screens[NumScr][NumStr].BoolParam;         // объявим ссылку - переменная сохранения

  if (ExitSaveCalibr and SaveCalibr) // при выходе из параметра и сохранении
  {
    ExitSaveCalibr = false; // сбросим что вышли из параметра
    SaveCalibr = false;     // сбросим переменную сохранения
    Serial.println("=Функция сохранения параметров=");
    SaveValueToFlash(false, false);    // вызов функции Записи значений
    BlinkBlueSaveToFlash.Start = true; // запустим моргалку синего диода
  }
  else if (ExitSaveCalibr) // при выходе из параметра без сохранения
  {
    ExitSaveCalibr = false; // сбросим что вышли из параметра
  }
}
//------------------------------------------------------------

//------------------------------------------------------------
// Функция применения координат
//------------------------------------------------------------
void ApplyParam(uint16_t NumScr, uint16_t NumStr, float Value, uint16_t &Param, void (CtrlRaspred::*FuncInit)(StructConstCoord), CtrlRaspred &obj, StructConstCoord Coord)
{
  bool &ExitApplyCalibr = Screens[NumScr][NumStr].ExitFromParam; // объявим ссылку - что вышли из параметра
  bool &ApplyCalibr = Screens[NumScr][NumStr].BoolParam;         // объявим ссылку - переменная сохранения

  if (ExitApplyCalibr and ApplyCalibr) // при выходе из параметра и сохранении
  {
    ExitApplyCalibr = false; // сбросим что вышли из параметра
    ApplyCalibr = false;     // сбросим переменную сохранения
    Param = (uint16_t)Value;
    Serial.print("=Установили значение:");
    Serial.println(Param);
    (obj.*FuncInit)(Coord);
  }
  else if (ExitApplyCalibr) // при выходе из параметра без сохранения
  {
    ExitApplyCalibr = false; // сбросим что вышли из параметра
  }
}

//------------------------------------------------------------
// Функция Map для типа float
//------------------------------------------------------------
float MapFloat(float x, float xmin, float xmax, float ymin, float ymax)
{
  if (xmax != xmin)
  {
    return (ymax - ymin) * ((x - xmin) / (xmax - xmin)) + ymin;
  }
  else
  {
    return 0.0;
  }
}
//------------------------------------------------------------

//------------------------------------------------------------
// Функция подсчет оборотов
//------------------------------------------------------------
void TestLive()
{
  Screens[0][1].Param = Screens[0][1].Param + 0.1;
}

//------------------------------------------------------------

//------------------------------------------------------------
// Функция резервирования памяти для строк
//------------------------------------------------------------
void ReservMemoryForScrDisplay()
{
  for (int s = 0; s < MAX_QTY_SCR / 2; s++)
  {
    for (int i = 0; i < MAX_STR; i++)
    {
      // for (int k = 0; k < 5; k++) {
      // Резервируем 100 байт под каждую из 5 строк в структуре
      Screens[s][i].Str[0].reserve(2);   // Выбор
      Screens[s][i].Str[1].reserve(100); // Название
      Screens[s][i].Str[2].reserve(10);  // Значение
      Screens[s][i].Str[3].reserve(15);  // Ед. изм.
      Screens[s][i].Str[4].reserve(2);   // Флаг
                                         // Screens[s][i].Str[k].reserve(100);
                                         // }
      yield();                           // Обязательно! Даем системе "подышать"
    }
  }

  // Если вы активно используете StructCurrentScr для отрисовки,
  // под него тоже стоит зарезервировать память
  for (int i = 0; i < MAX_STR_DISPLAY; i++)
  {
    for (int k = 0; k < 5; k++)
    {
      ArrScr.CurrentScreen.Str[i][k].reserve(100);
    }
  }
  for (i = 0; i <= 1; i++)
  {
    ArrScrMini[i].reserve(50);
    ArrScrMiniNew[i].reserve(50);
  }
}
//------------------------------------------------------------

//------------------------------------------------------------
// Функция для обрезки строк для OldeMini
//------------------------------------------------------------
void StrForOledMini(String *ResultStr, const String AddStr, uint16_t Value, uint16_t Length)
{
  String Str = String((int16_t(Value)));

  // Если строка уже длиннее Length, обрезаем или просто добавляем AddStr
  if (Str.length() < Length)
  {
    while (Str.length() < Length)
    {
      Str = " " + Str;
    }
  }
  *ResultStr = Str + AddStr;
}
//------------------------------------------------------------

//------------------------------------------------------------
// Функция для инициализации значений по умолчанию
//------------------------------------------------------------
void DefaultValueInit()
{
  FlashMemory.TOilVentStart = 82.0;   // Т масла. T Включения
  FlashMemory.TOilVentStop = 80.0;    // Т масла. T Отключения
  FlashMemory.TOilMinRPMStart = 50.0; // Т масла. Мин. скорость
  FlashMemory.TOilTMaxRPM = 100.0;    // Т масла. Максимальная температура, при которой максимаотная скорость вентилятора

  FlashMemory.TWaterVent1Start = 82.0;       // Т воды. T Включения 1ого вент
  FlashMemory.TWaterVent1Stop = 80.0;        // Т воды. T Отключения 1ого вент
  FlashMemory.TWaterVent1MinRPMStart = 50.0; // Т воды. Мин. Скорость 1ого вент
  FlashMemory.TWaterVent1TMaxRPM = 100.0;    // Т воды. Максимальная температура, при которой максимаотная скорость вентилятора

  FlashMemory.RPMEngineVisuMiniDisplay = true; // Обороты двигателя, об/мин. Отображать на маленьком дисплее
  FlashMemory.RPMEngineKoefMul = 1.0;          // Обороты двигателя, об/мин. Коэффициент домножения

  FlashMemory.VelocityVisuMiniDisplay = true; // Скорость, км/час. Отображать на маленьком дисплее
  FlashMemory.VelocityDiamWheel = 75.0;       // Скорость, км/час. Диаметр колеса, см
  FlashMemory.VelocityKoefReduction = 3.0;    // Скорость, км/час. Редукция на мосте
  FlashMemory.VelocityKoefMul = 1.0;          // Скорость, км/час. Коэффициент домножения

  FlashMemory.BlinkStart = true;         // Настройки. Моргание стробосками и поворотниками при старте
  FlashMemory.TimeStrobOnStart = 1000.0; // Настройки. Время включения стробоскопов, мсек
  FlashMemory.TimeTurnOnStart = 1000.0;  // Настройки. Время вкл.поворотников, мсек

  FlashMemory.TimeTurnOn = 500.0;      // Поворотники. Время в отключенном состоянии, мс
  FlashMemory.TimeTurnOff = 500.0;     // Поворотники. Время в включенном состоянии, мс
  FlashMemory.TimeTurnOnBuzzer = 50.0; // Поворотники. Время выдачи звука на буззер, мс
}
//------------------------------------------------------------