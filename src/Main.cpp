#include <Arduino.h>
#include "Timers.h"
#include "Input.h"
#include "Generator.h"
#include "EEManager.h"
#include "RunningAverage.h"
#include "Menu\Menu.h"
#include "GyverOLEDCustom.h"
#include "GyverOLED.h"
#include "GyverEncoder.h"
#include "RemoteControl.h"
#include "GyverDS18.h"
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
#define PIN_TORCH_ON_INPUT 12
#define PIN_ARC_OK_OUTPUT 13
#define PIN_RELAY_SELECT 14

#define THC_Z_STEP_PIN 25
#define THC_Z_DIR_PIN 26

#define PIN_ARC_VOLTAGE_ADC 34

#define DEFAULT_TIME_BOUNCE_BTN_MS 50 // Дефолтное время дребезга кнопок в милисекундах

#define NOM_PIN_ENC_TOLEFT 11  // номер Pin. Энкодер. Влево
#define NOM_PIN_ENC_TORIGHT 12 // номер Pin. Энкодер. Вправо
#define NOM_PIN_ENC_BTN 13     // номер Pin. Энкодер. Кнопка
#define NOM_PIN_BUZZER 39      // номер Pin. Буззер

// --- НАСТРОЙКИ ДИНАМИЧЕСКОЙ СКОРОСТИ ---
const float Kp = 2.5;              // Коэффициент усиления скорости (подбирается экспериментально)
const uint64_t MAX_SPEED_US = 120; // Максимальная скорость Z (чем МЕНЬШЕ микросекунд, тем БЫСТРЕЕ)
const uint64_t MIN_SPEED_US = 800; // Минимальная скорость Z при мелких корректировках

#define MS_100 20  // милисекунды для моргания
#define MS_300 20  // милисекунды для моргания
#define MS_500 500 // милисекунды для моргания

const unsigned long PIERCE_DELAY_MS = 600;

// --- НАСТРОЙКА КАЛИБРОВКИ ВОЛЬТМЕТРА ---
const float DIVIDER_RATIO = 100.0;     // Коэффициент ВАШЕГО делителя (Рекомендуется 1:100)
const float ESP32_ADC_MAX_VOLTS = 2.3; // Ваш линейный предел АЦП ESP32

// Настройки параметров реза в физических величинах
const float TARGET_VOLTS = 120.0;     // Желаемое напряжение дуги (Вольты)
const float DEADZONE_VOLTS = 2.0;     // Допуск зоны нечувствительности (+/- Вольты)


// Автоматический пересчет в единицы АЦП при старте микроконтроллера
const int TARGET_VOLTAGE_ADC = (int)(((TARGET_VOLTS / DIVIDER_RATIO) / ESP32_ADC_MAX_VOLTS) * 4095.0);
const int VOLTAGE_DEADZONE   = (int)(((DEADZONE_VOLTS / DIVIDER_RATIO) / ESP32_ADC_MAX_VOLTS) * 4095.0);




enum THCState
{
  IDLE,
  PIERCING,
  ACTIVE_CUTTING
};
THCState currentState = IDLE;

// Переменные таймера (Стандарт Arduino Core 3.x)
hw_timer_t * zTimer = NULL;
volatile bool stepPhase = false;
volatile int stepDirection = 0; 
// Определяем две шины I2C
// TwoWire Wire2 = TwoWire(1);

// GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled(0x3C, &Wire2);
// GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oledMini(0x3C, &Wire);
// GyverOLED<SSH1106_128x64> oledMini;
Encoder Enc(NOM_PIN_ENC_TOLEFT, NOM_PIN_ENC_TORIGHT, -1, TYPE2); // объявим энкодер класс

// Переменные таймера
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

DIn BtnEnc{false, DEFAULT_TIME_BOUNCE_BTN_MS}; // класс. Кнопка энкодера

DIn ReqDS(false, 1000, 0); // опрашивание DS

DIn TorchOn(false, DEFAULT_TIME_BOUNCE_BTN_MS); // класс. Сигнал включения дуги

AIn AInVelocity, AInVelo;
AIn AInArcVoltage; // Аналоговый вход. Напряжение дуги

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

bool BeepTurn; // выдача звука при поворотах

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
TON TmrPiercing;          // Таймер задержки пробивки металла
TaskHandle_t TaskDisplay; // Задача 1
TaskHandle_t Task2;       // Задача 2
TaskHandle_t TaskESPNow;  // Задача ESP-NOW
void TaskDisplayCicle(void *pvParameters);
void Task2code(void *pvParameters);
void TaskESPNowCicle(void *pvParameters);
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

  // инициализируем номера pin на кнопках
  BtnEnc.ModePin(NOM_PIN_ENC_BTN); // Кнопка. Энкодера"
  Enc.setTickMode(AUTO);
  BuzzerWarning.Init(1000, 1000, 4);

  AInArcVoltage.Init(13, 5000, 0, 8192, 1, 100); // AIn::Init(byte NoPin, uint16_t _KoeffFiltr, uint16_t MinADC, uint16_t MaxADC, uint16_t MinTech, uint16_t MaxTech)
  AInArcVoltage.InitUnreliability(8190, 10);

  setCpuFrequencyMhz(240);

  ReservMemoryForScrDisplay();

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

  TorchOn.ModePin(PIN_TORCH_ON_INPUT); // Кнопка. Энкодера"
  pinMode(NOM_PIN_BLUE_LED, OUTPUT);   // выход Синий светодиод
  pinMode(NOM_PIN_BUZZER, OUTPUT);     // Выход. Буззер
  pinMode(PIN_ARC_OK_OUTPUT, OUTPUT);
  pinMode(PIN_RELAY_SELECT, OUTPUT);
  pinMode(THC_Z_STEP_PIN, OUTPUT);
  pinMode(THC_Z_DIR_PIN, OUTPUT);

  digitalWrite(PIN_ARC_OK_OUTPUT, LOW);
  digitalWrite(PIN_RELAY_SELECT, LOW);
  digitalWrite(THC_Z_STEP_PIN, LOW);


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
void IRAM_ATTR onTimer()
{
  if (stepDirection == 0)
    return;

  if (!stepPhase)
  {
    digitalWrite(THC_Z_DIR_PIN, stepDirection == 1 ? HIGH : LOW);
    digitalWrite(THC_Z_STEP_PIN, HIGH);
    stepPhase = true;
  }
  else
  {
    digitalWrite(THC_Z_STEP_PIN, LOW);
    stepPhase = false;
  }
}

//============================================================
// Функция настройки таймера прерывания для подсчета об/мин двигателя и карданного вала
//============================================================
void setup_timer_hardware()
{

  // === ИСПРАВЛЕННЫЙ СИНТАКСИС ДЛЯ СТАРОГО ЯДРА ESP32 (2.x.x) ===
  // Номер таймера 0, делитель 80 (80 МГц / 80 = 1 МГц, т.е. 1 тик = 1 мкс), считать вверх = true
  zTimer = timerBegin(0, 80, true); 
  
  // Привязываем функцию прерывания к таймеру, тип прерывания по фронту = true
  timerAttachInterrupt(zTimer, &onTimer, true);
  
  // Устанавливаем первоначальный интервал (в тиках/мкс) и включаем автоперезапуск = true
  timerAlarmWrite(zTimer, MIN_SPEED_US, true);
  
  // Активируем таймер (в старых версиях это обязательная отдельная функция)
  timerAlarmEnable(zTimer);

  // timer_config_t config = {
  //     .alarm_en = TIMER_ALARM_EN,
  //     .counter_en = TIMER_PAUSE,
  //     .counter_dir = TIMER_COUNT_UP,
  //     .auto_reload = TIMER_AUTORELOAD_EN,
  //     .divider = 80 // 1 тик = 1 мкс при 80 МГц
  // };

  // timer_init(TIMER_GROUP_0, TIMER_0, &config);
  // // Устанавливаем порог срабатывания для таймера двигателя(например, через 5 секунд)
  // timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIME_RPM_RESET);
  // timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  // timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, timer_isr_engine, NULL, 0);
  // timer_start(TIMER_GROUP_0, TIMER_0);
}
//============================================================

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
// Вызов функции циклически
//============================================================
void Task2code(void *pvParameters)
{
  setup_timer_hardware(); // инициализация таймеров прерывания

  for (;;)
  {
    TestLive();

    bool torchOnSignal = TorchOn.ReadDIn(); // Считываем сигнал включения дуги

    TmrPiercing.TONTmr(currentState == PIERCING, PIERCE_DELAY_MS); // Запускаем таймер задержки после пробивки металла

    switch (currentState)
    {
    case IDLE:
      if (torchOnSignal) // Если сигнал включения дуги появился, то переходим в состояние пробивки металла
      {
        digitalWrite(PIN_RELAY_SELECT, HIGH); // Мгновенно забираем контроль над Z у FluidNC
        //delay(15);
        currentState = PIERCING;
      }
      break;

    case PIERCING: // 
      if (!torchOnSignal) // Если сигнал включения дуги пропал, то возвращаемся в состояние ожидания
      {
        currentState = IDLE;
        digitalWrite(PIN_RELAY_SELECT, LOW);
        break;
      }

      if (TmrPiercing.Q) // Если таймер задержки после пробивки металла сработал
      {
        digitalWrite(PIN_ARC_OK_OUTPUT, HIGH); // Металл пробит -> FluidNC поехал по X/Y
        currentState = ACTIVE_CUTTING;         //
      }
      break;

    case ACTIVE_CUTTING:
      if (!torchOnSignal) // Если сигнал включения дуги пропал, то возвращаемся в состояние ожидания
      {
        stepDirection = 0;
        digitalWrite(PIN_ARC_OK_OUTPUT, LOW);
        digitalWrite(PIN_RELAY_SELECT, LOW); // Отдаем Z обратно FluidNC для холостых переездов
        currentState = IDLE;
        break;
      }

      // Получаем отфильтрованное значение напряжения дуги
      int currentArcVoltage = AInArcVoltage.Value();

      int voltageError = currentArcVoltage - TARGET_VOLTAGE_ADC;

      if (abs(voltageError) <= VOLTAGE_DEADZONE)
      {
        stepDirection = 0; // Напряжение в допуске, удерживаем высоту
      }
      else
      {
        // Задаем направление (Высокое напряжение = длинная дуга = едем ВНИЗ)
        if (voltageError > 0)
        {
          stepDirection = -1;
        }
        else
        {
          stepDirection = 1;
        }

        // P-регулятор скорости: расчет периода таймера в зависимости от величины отклонения
        long calculatedPeriod = MIN_SPEED_US - (abs(voltageError) * Kp);

        if (calculatedPeriod < (long)MAX_SPEED_US)
        {
          currentTimerPeriodUs = MAX_SPEED_US;
        }
        else if (calculatedPeriod > (long)MIN_SPEED_US)
        {
          currentTimerPeriodUs = MIN_SPEED_US;
        }
        else
        {
          currentTimerPeriodUs = (uint32_t)calculatedPeriod;
        }

        // Обновляем интервал шагов Z на лету
       timerAlarmWrite(zTimer, currentTimerPeriodUs, true);
      }
      break;
    }

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
    delay(5);
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