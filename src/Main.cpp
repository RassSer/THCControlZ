/**************************************************************************************
 *  ESP32-S2 — реципрокный частотомер на PCNT + esp_timer
 *
 *  Принцип: PCNT считает N импульсов -> событие H_LIM -> в ISR снимается метка времени.
 *  f = (P2 - P1) * 1e6 / (t2 - t1), где t1,t2 взяты НА ФРОНТАХ импульсов.
 *  Никакого опроса сырого регистра, никаких потерь на переполнении.
 **************************************************************************************/
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
#include "FastAccelStepper.h"

#include <Arduino.h>
#include "driver/pcnt.h"
#include "esp_timer.h"

//------------------------------------------------------------
// константы определенные препроцессором
//------------------------------------------------------------
// ------------------------------------------------------------------ КОНФИГ
#define PIN_PULSE 7               // вход импульсов
#define PCNT_UNIT_ENG PCNT_UNIT_0 // на S2 доступны 0..3

#define PULSES_PER_REV 1.0f     // импульсов на оборот
#define TARGET_WINDOW_US 100000 // целевое окно измерения, мкс (100 мс)
#define N_MIN 1                 // на низких частотах меряем период в период
#define N_MAX 20000
#define FILTER_TICKS 80              // 80 тиков APB = 1 мкс; 0 = выкл, макс 1023
#define PULL_MODE GPIO_PULLDOWN_ONLY // open-collector -> PULLUP, push-pull -> FLOATING
#define EDGE_NEG_MODE PCNT_COUNT_DIS // PCNT_COUNT_INC = считать оба фронта

#define PRINT_MS 250
#define TIMEOUT_US 1000000 // нет событий 1 с -> частота 0
#define EMA_ALPHA 0.0f     // 0 = без сглаживания, 0.2..0.3 = мягче

#define INIT_KEY 6 // ключ первого запуска. 0-254, на выбор

#define NOM_PIN_BLUE_LED 15 // номер Pin Синий светодиод
// --- НАСТРОЙКА ПИНОВ ---
#define PIN_TORCH_ON_INPUT 34
#define PIN_ARC_OK_OUTPUT 35
#define PIN_RELAY_SELECT 33

#define THC_Z_STEP_PIN 16
#define THC_Z_DIR_PIN 17
#define PIN_ARC_VOLTAGE_ADC 1

#define DEFAULT_TIME_BOUNCE_BTN_MS 50 // Дефолтное время дребезга кнопок в милисекундах

#define NOM_PIN_ENC_TOLEFT 5  // номер Pin. Энкодер. Влево
#define NOM_PIN_ENC_TORIGHT 6 // номер Pin. Энкодер. Вправо
#define NOM_PIN_ENC_BTN 3     // номер Pin. Энкодер. Кнопка
#define NOM_PIN_BUZZER 4      // номер Pin. Буззер
#define NOM_PIN_SLIDER 8      // номер Pin. Слайдер для выставления напряжения дуги

#define MS_100 20  // милисекунды для моргания
#define MS_300 20  // милисекунды для моргания
#define MS_500 500 // милисекунды для моргания

enum THCState
{
  IDLE,
  PIERCING,
  ACTIVE_CUTTING
};
THCState currentState = IDLE;
uint16_t FregTest;
// Настройки допуска и скорости
const int CENTER_VAL = 140; // Центральная точка (12-бит АЦП: 0 - 4095)
const int TOLERANCE = 3;    // Допуск (зона нечувствительности)
const int MAX_SPEED = 4000; // Максимальная скорость мотора (шагов/сек)

Encoder Enc(NOM_PIN_ENC_TOLEFT, NOM_PIN_ENC_TORIGHT, -1, TYPE2); // объявим энкодер класс

//------------------------------------------------------------
// глобальные переменные
//------------------------------------------------------------

Screen ArrScr(3); // класс экранов

//------------------------------------------------------------
// Структура запоминаемых данных
//------------------------------------------------------------
struct StructRef
{
  float &DeltaVoltage = Screens[0][2].Param; // Ссылка. Заданное напряжение, В. Дельта напряжения, В
  float &Sensitivity = Screens[0][3].Param;  // Ссылка. Заданное напряжение, В. Чувствительность, 50-100%

  float &TechMaxVoltagePotenciometr = Screens[1][1].Param; // Ссылка. Заданное напряжение, В. Технический максимум напряжения, при макс значении на потенциометре, В
  float &TechMinVoltagePotenciometr = Screens[1][2].Param; // Ссылка. Заданное напряжение, В. Технический минимум напряжения, при мин значении на потенциометре, В
  float &MinVoltageControlZ = Screens[1][3].Param;         // Ссылка. Заданное напряжение, В. Минимальное напряжение выше которого идет контроль оси Z

  float &StepAngle = Screens[2][1].Param;    // Ссылка. Скорость, 50-100%. Угол шага, °
  float &Acceleration = Screens[2][2].Param; // Ссылка. Скорость, 50-100%. Ускорение
  float &MaxSpeed = Screens[2][3].Param;     // Ссылка. Скорость, 50-100%. Максимальная скорость, об/мин
  float &MinSpeed = Screens[2][4].Param;     // Ссылка. Скорость, 50-100%. Минимальная скорость, об/мин

  float &TimePiercing = Screens[3][1].Param;       // Ссылка. Временные задержки. Время пробития, мс
  float &TimeBeforeControlZ = Screens[3][2].Param; // Ссылка. Временные задержки. Время перед началом контроля Z, мс
};
//------------------------------------------------------------
StructRef Ref;
//------------------------------------------------------------

//------------------------------------------------------------
// добавляем в эту структуру всё что хотим сохранить в эрергонезависимой памяти
//------------------------------------------------------------
struct StructForMem
{
  float DeltaVoltage; //. Дельта напряжения, В
  float Sensitivity;  //. Чувствительность, 50-100%

  float TechMaxVoltagePotenciometr; // Заданное напряжение, В. Технический максимум напряжения, при макс значении на потенциометре, В
  float TechMinVoltagePotenciometr; // Заданное напряжение, В. Технический минимум напряжения, при мин значении на потенциометре, В
  float MinVoltageControlZ;         // Заданное напряжение, В. Минимальное напряжение выше которого идет контроль оси Z

  float StepAngle;    // Скорость, 50-100%. Угол шага, °
  float Acceleration; // Скорость, 50-100%. Ускорение
  float MaxSpeed;     // Скорость, 50-100%. Максимальная скорость, об/мин
  float MinSpeed;     // Скорость, 50-100%. Минимальная скорость, об/мин

  float TimePiercing;       // Временные задержки. Время пробития, мс
  float TimeBeforeControlZ; // Временные задержки. Время перед началом контроля Z, мс
};

StructForMem FlashMemory;      // объявляем структуру для запоминания в памяти
EEManager Memory(FlashMemory); // передаём нашу переменную (фактически её адрес)
//------------------------------------------------------------

DIn BtnEnc{false, DEFAULT_TIME_BOUNCE_BTN_MS}; // класс. Кнопка энкодера

DIn TorchOn(false, DEFAULT_TIME_BOUNCE_BTN_MS); // класс. Сигнал включения дуги

AIn AInArcVoltage; // Аналоговый вход. Напряжение дуги
AIn AInSetVoltage; // Аналоговый вход. Задатчик напряжения дуги

float ArcVoltage; // Напряжение дуги

FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;

Generator Blink100x300ms;
Generator Blink500x500ms;
Generator BlinkTurn;
BlinkRepeatly BuzzerWarning;

bool BtnMenuOk;        // кнопка меню Ok для Task дисплей
bool BtnMenuUp;        // кнопка меню Вверх для Task дисплей
bool BtnMenuDown;      // кнопка меню Вниз для Task дисплей
bool FirstScan = true; // признак первого скана

void SaveValueToFlash(bool DefaultValue, bool FirstScan); // Функция сохранения/выгрузки значения из eeprom
void DOut();                                              // функция обработки дискретных выходов
void setup();
void loop();
void Display();
void DataToSend(); // Функция для формирования отправляемых данных
void TestLive();   // Функция для показания что жив CPU на дисплее
void ReservMemoryForScrDisplay();

TON TmrPiercing;          // Таймер задержки пробивки металла
TON TmrBeforeControlZ;    // Таймер задержки перед началом контроля напряжения на оси Z
TaskHandle_t TaskDisplay; // Задача 1
TaskHandle_t Task2;       // Задача 2
TaskHandle_t TaskESPNow;  // Задача ESP-NOW
void TaskDisplayCicle(void *pvParameters);
void Task2code(void *pvParameters);
void TaskESPNowCicle(void *pvParameters);
void SaveParam(uint16_t NumScr, uint16_t NumStr); // Функция сохранения координат
void DefaultValueInit();                          // Функция для инициализации значений по умолчанию

// ------------------------------------------------------------------ СОСТОЯНИЕ
static portMUX_TYPE s_mux = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t s_N = 10;    // импульсов на событие
static volatile uint64_t s_total = 0; // накопленные импульсы (только полные блоки)
static volatile int64_t s_lastUs = 0; // метка последнего события
static volatile uint32_t s_events = 0;
static volatile bool s_rebase = true; // сбросить опорную точку

static uint64_t g_baseP = 0;
static int64_t g_baseT = 0;
static bool g_haveBase = false;
static float g_freqHz = 0.0f;

uint16_t x, i;

const unsigned long PIERCE_DELAY_MS = 600;

// ------------------------------------------------------------------ ISR
// Вызывается раз в N импульсов -> нагрузка минимальна даже на сотнях кГц.
static void IRAM_ATTR pcnt_hlim_isr(void *)
{
  int64_t now = esp_timer_get_time();
  s_total += s_N;
  s_lastUs = now;
  s_events++;
}

// ------------------------------------------------------------------ PCNT
static void pcnt_apply_N(uint32_t n)
{
  if (n < N_MIN)
    n = N_MIN;
  if (n > N_MAX)
    n = N_MAX;

  pcnt_counter_pause(PCNT_UNIT_ENG);
  pcnt_set_event_value(PCNT_UNIT_ENG, PCNT_EVT_H_LIM, (int16_t)n);
  pcnt_counter_clear(PCNT_UNIT_ENG);

  portENTER_CRITICAL(&s_mux);
  s_N = n;
  s_rebase = true; // частичный счёт потерян -> опорную точку берём заново
  portEXIT_CRITICAL(&s_mux);

  pcnt_counter_resume(PCNT_UNIT_ENG);
}

static void pcnt_init()
{
  pcnt_config_t cfg = {};
  cfg.pulse_gpio_num = PIN_PULSE;
  cfg.ctrl_gpio_num = PCNT_PIN_NOT_USED;
  cfg.lctrl_mode = PCNT_MODE_KEEP;
  cfg.hctrl_mode = PCNT_MODE_KEEP;
  cfg.pos_mode = PCNT_COUNT_INC;
  cfg.neg_mode = EDGE_NEG_MODE;
  cfg.counter_h_lim = (int16_t)s_N;
  cfg.counter_l_lim = 0;
  cfg.unit = PCNT_UNIT_ENG;
  cfg.channel = PCNT_CHANNEL_0;
  ESP_ERROR_CHECK(pcnt_unit_config(&cfg));

  // подтяжку ставим ПОСЛЕ unit_config — он сам переписывает настройки пада
  gpio_set_pull_mode((gpio_num_t)PIN_PULSE, PULL_MODE);

#if FILTER_TICKS > 0
  ESP_ERROR_CHECK(pcnt_set_filter_value(PCNT_UNIT_ENG, FILTER_TICKS));
  pcnt_filter_enable(PCNT_UNIT_ENG);
#else
  pcnt_filter_disable(PCNT_UNIT_ENG);
#endif

  pcnt_counter_pause(PCNT_UNIT_ENG);
  pcnt_counter_clear(PCNT_UNIT_ENG);
  ESP_ERROR_CHECK(pcnt_event_enable(PCNT_UNIT_ENG, PCNT_EVT_H_LIM));

  esp_err_t e = pcnt_isr_service_install(0);
  if (e != ESP_OK && e != ESP_ERR_INVALID_STATE)
    ESP_ERROR_CHECK(e);
  ESP_ERROR_CHECK(pcnt_isr_handler_add(PCNT_UNIT_ENG, pcnt_hlim_isr, NULL));
  ESP_ERROR_CHECK(pcnt_intr_enable(PCNT_UNIT_ENG));

  pcnt_counter_resume(PCNT_UNIT_ENG);
}

//------------------------------------------------------------
// Настройка шаговика
//------------------------------------------------------------
void setup_stepper()
{
  // Инициализация аппаратного движка ШИМ/RMT для шаговика
  engine.init();
  stepper = engine.stepperConnectToPin(THC_Z_STEP_PIN);

  if (stepper)
  {
    stepper->setDirectionPin(THC_Z_DIR_PIN);
    // Аппаратный разгон NEMA 23 (шагов/сек^2) — чтобы не было клина при резкой смене направления
    stepper->setAcceleration(8000);
  }
}
// ------------------------------------------------------------------ ИЗМЕРЕНИЕ
// Возвращает true, если частота обновилась.
static bool freq_update()
{
  uint64_t p;
  int64_t t;
  uint32_t ev;
  bool reb;

  portENTER_CRITICAL(&s_mux);
  p = s_total;
  t = s_lastUs;
  ev = s_events;
  reb = s_rebase;
  s_rebase = false;
  portEXIT_CRITICAL(&s_mux);

  if (reb || !g_haveBase)
  { // новая опорная точка
    if (ev == 0)
      return false;
    g_baseP = p;
    g_baseT = t;
    g_haveBase = true;
    return false;
  }

  int64_t now = esp_timer_get_time();

  // нет импульсов — обнуляем и расширяем чувствительность
  if (now - t > TIMEOUT_US)
  {
    g_freqHz = 0.0f;
    if (s_N > N_MIN)
      pcnt_apply_N(N_MIN);
    g_baseP = p;
    g_baseT = t;
    return false;
  }

  uint64_t dP = p - g_baseP;
  int64_t dT = t - g_baseT;

  if (dP == 0 || dT <= 0)
    return false; // новых событий ещё нет
  if (dT < (TARGET_WINDOW_US / 2))
    return false; // копим до половины окна

  float f = (float)((double)dP * 1000000.0 / (double)dT);

  if (EMA_ALPHA > 0.0f && g_freqHz > 0.0f)
    g_freqHz += EMA_ALPHA * (f - g_freqHz);
  else
    g_freqHz = f;

  g_baseP = p;
  g_baseT = t; // следующее окно стык в стык

  // подгоняем N так, чтобы событие приходило примерно раз в TARGET_WINDOW_US
  uint32_t want = (uint32_t)(f * (TARGET_WINDOW_US / 1000000.0f));
  if (want < N_MIN)
    want = N_MIN;
  if (want > N_MAX)
    want = N_MAX;
  uint32_t cur = s_N;
  if (want > cur * 2 || want < cur / 2)
    pcnt_apply_N(want);

  return true;
}

// ------------------------------------------------------------------ MAIN
void setup()
{
  setlocale(LC_ALL, "en_US.UTF-8");
  Serial.begin(115200);
  delay(1500);
  // Причина сброса
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.print("Reset reason: ");
  Serial.println(reason);

  Serial.println("\n=== Reciprocal frequency meter (PCNT + esp_timer) ===");
  pcnt_init();
  Serial.printf("GPIO=%d  unit=%d  filter=%d tick  N0=%u\n",
                PIN_PULSE, (int)PCNT_UNIT_ENG, FILTER_TICKS, s_N);

  // инициализируем номера pin на кнопках
  BtnEnc.ModePin(NOM_PIN_ENC_BTN); // Кнопка. Энкодера"
  Enc.setTickMode(AUTO);
  BuzzerWarning.Init(1000, 1000, 4);

  setCpuFrequencyMhz(240);

  ReservMemoryForScrDisplay();

  EEPROM.begin(512); // резервируем блок во флэш памяти
                     //  DefaultValueInit();            // Функция для инициализации значений по умолчанию
                     // SaveValueToFlash(false, true); // Функция сохранения/выгрузки значения из eeprom

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
  AInSetVoltage.InitUnreliability(50, 8100); // Проиницилизируем диапазон недостоверности слайдера
  setup_stepper();

  Serial.print("Свободно памяти: ");
  Serial.println(ESP.getFreeHeap());

  Serial.print("Макс. непрерывный блок: ");
  Serial.println(ESP.getMaxAllocHeap());

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

  freq_update();

  static uint32_t t0 = 0;
  if (millis() - t0 >= PRINT_MS)
  {
    t0 = millis();
    float Volt = 0.0;
    if (g_freqHz >= 15.0)
    {
      Volt = 79.4678 * g_freqHz / 1000.0f + 20.1050;
    }

    AInArcVoltage._KoeffFiltr = map(Ref.Sensitivity, 0, 10000.0, 0, 100.0); // переведем коэфициент фильтрации из % в 0-10000
    ArcVoltage = AInArcVoltage.Filtr1th(Volt);                              // вызываем функцию фильтрации напряяжения дуги

    // Serial.printf("F = %10.3f Hz | Напряжение = %9.2f | N = %-6u | events = %u\n",
    //               g_freqHz, ArcVoltage, s_N, s_events);
  }
  vTaskDelay(pdMS_TO_TICKS(1));
}

//------------------------------------------------------------
// Функция циклически вызываемая по приоритету
//------------------------------------------------------------
void TaskDisplayCicle(void *pvParameters)
{
  for (;;)
  {
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
    if (Screens[0][7].ExitFromParam and Screens[0][7].BoolParam) // при выходе из параметра возврата к заводским настройкам
    {
      Screens[0][7].BoolParam = false;
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
  // PutGetInit();
  for (;;)
  {
    // PutGetCycle();
    delay(20);
  }
  vTaskDelete(NULL);
}
//------------------------------------------------------------

//------------------------------------------------------------
// Функция прорисовки на OLED
//------------------------------------------------------------
void Display()
{
  float CurrentSpeedHZ = stepper->getCurrentSpeedInMilliHz() / 1000.0;                 // Переводим миллиГерцы в нормальные импульсы в секунду (Гц)
  Screens[2][0].Param = Screens[0][4].Param = (CurrentSpeedHZ * Ref.StepAngle) / 60.0; // Для отображения скорости об/мин

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

//============================================================
// Вызов функции циклически
//============================================================
void Task2code(void *pvParameters)
{
  for (;;)
  {
    ;
    // // TestLive();

    bool torchOnSignal = TorchOn.ReadDIn();                                                                             // Считываем сигнал включения дуги
    AInSetVoltage.Init(NOM_PIN_SLIDER, 3000, 50, 8100, Ref.TechMinVoltagePotenciometr, Ref.TechMaxVoltagePotenciometr); // каждый раз переинициализируем в зависимости от значений миниммума максимума
    AInSetVoltage.Value();                                                                                              // Считываем значение слайдера

    torchOnSignal = true;
    TmrPiercing.TONTmr(currentState == PIERCING, Ref.TimePiercing);                   // Запускаем таймер задержки после пробивки металла
    TmrBeforeControlZ.TONTmr(currentState == ACTIVE_CUTTING, Ref.TimeBeforeControlZ); // Запускаем таймер задержки перед началом контроля Z

    switch (currentState)
    {
    case IDLE:

      if (torchOnSignal) // Если сигнал включения дуги появился, то переходим в состояние пробивки металла
      {

        digitalWrite(PIN_RELAY_SELECT, HIGH); // Мгновенно забираем контроль над Z у FluidNC
        currentState = PIERCING;
      }
      break;

    case PIERCING:        //
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
        digitalWrite(PIN_ARC_OK_OUTPUT, LOW);
        digitalWrite(PIN_RELAY_SELECT, LOW); // Отдаем Z обратно FluidNC для холостых переездов
        currentState = IDLE;
        break;
      }

      break;
    }

    // Получаем отфильтрованное значение напряжения дуги
    int currentArcVoltage = ArcVoltage;

    // currentArcVoltage = 1000;

    // if (FregTest > 2500)
    // {
    //   TempBool = true;
    // }
    // if (FregTest < 1500)
    // {
    //   TempBool = false;
    // }

    // if (!TempBool)
    //   FregTest++;
    // else
    //   FregTest--;

    // Serial.print("FregTest = ");
    // Serial.println(FregTest);

    // Считаем отклонение от центральной точки
    int deviation = currentArcVoltage - AInSetVoltage.QValue;

    // deviation = FregTest - CENTER_VAL;

    // Проверяем, вышли ли мы за пределы допуска (TOLERANCE)
    if (abs(deviation) > Ref.DeltaVoltage)
    {
      // Карта скорости: чем больше отклонение, тем выше частота импульсов ШИМ
      long SetSpeedRPM = map(abs(deviation), Ref.DeltaVoltage, 20.0, Ref.MinSpeed, Ref.MaxSpeed); // получим значение скорости от напряжения дуги (чем дальше - тем выше скорость)
      SetSpeedRPM = constrain(SetSpeedRPM, 0, Ref.MaxSpeed);                                      // обрежем скорость, если выше максимальной

      float ImpPerSec = 60.0 * SetSpeedRPM / Ref.StepAngle; // Пересчет количество импульсов/секунду из об/мин

        
      Serial.println("========");
      Serial.print("Ref.DeltaVoltage = ");
      Serial.println(Ref.DeltaVoltage);
      Serial.print("AInSetVoltage.QValue = ");
      Serial.println(AInSetVoltage.QValue);
      Serial.print("SetSpeedRPM = ");
      Serial.println(SetSpeedRPM);
      Serial.print("ImpPerSec = ");
      Serial.println(ImpPerSec);
      Serial.println("");

      stepper->setSpeedInHz(ImpPerSec); // Меняем частоту на лету (аппаратно)

      if (deviation > 0)
      {
        // Сигнал ушел вверх — крутим вперед бесконечным аппаратным ШИМ с разгоном
        stepper->runForward();
      }
      else
      {
        // Сигнал ушел вниз — крутим назад
        stepper->runBackward();
      }
    }
    else
    {
      // Сигнал внутри допуска — даем команду аппаратного плавного торможения
      stepper->stopMove();
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
      Ref.DeltaVoltage = FlashMemory.DeltaVoltage; // Заданное напряжение, В. Дельта напряжения, В
      Ref.Sensitivity = FlashMemory.Sensitivity;   // Заданное напряжение, В. Чувствительность, 50-100%

      Ref.TechMaxVoltagePotenciometr = FlashMemory.TechMaxVoltagePotenciometr; // Заданное напряжение, В. Технический максимум напряжения, при макс значении на потенциометре, В
      Ref.TechMinVoltagePotenciometr = FlashMemory.TechMinVoltagePotenciometr; // Заданное напряжение, В. Технический минимум напряжения, при мин значении на потенциометре, В
      Ref.MinVoltageControlZ = FlashMemory.MinVoltageControlZ;                 // Заданное напряжение, В. Минимальное напряжение выше которого идет контроль оси Z

      Ref.StepAngle = FlashMemory.StepAngle;       // Скорость, 50-100%. Угол шага, °
      Ref.Acceleration = FlashMemory.Acceleration; // Скорость, 50-100%. Ускорение
      Ref.MaxSpeed = FlashMemory.MaxSpeed;         // Скорость, 50-100%. Максимальная скорость, об/мин
      Ref.MinSpeed = FlashMemory.MinSpeed;         // Скорость, 50-100%. Минимальная скорость, об/мин

      Ref.TimePiercing = FlashMemory.TimePiercing;             // Временные задержки. Время пробития, мс
      Ref.TimeBeforeControlZ = FlashMemory.TimeBeforeControlZ; // Временные задержки. Время перед началом контроля Z, мс
    }
  }
  else if (DefaultValue) // инициализация значениями по умолчанию
  {
    DefaultValueInit();

    Ref.DeltaVoltage = FlashMemory.DeltaVoltage; // Заданное напряжение, В. Дельта напряжения, В
    Ref.Sensitivity = FlashMemory.Sensitivity;   // Заданное напряжение, В. Чувствительность, 50-100%

    Ref.TechMaxVoltagePotenciometr = FlashMemory.TechMaxVoltagePotenciometr; // Заданное напряжение, В. Технический максимум напряжения, при макс значении на потенциометре, В
    Ref.TechMinVoltagePotenciometr = FlashMemory.TechMinVoltagePotenciometr; // Заданное напряжение, В. Технический минимум напряжения, при мин значении на потенциометре, В
    Ref.MinVoltageControlZ = FlashMemory.MinVoltageControlZ;                 // Заданное напряжение, В. Минимальное напряжение выше которого идет контроль оси Z

    Ref.StepAngle = FlashMemory.StepAngle;       // Скорость, 50-100%. Угол шага, °
    Ref.Acceleration = FlashMemory.Acceleration; // Скорость, 50-100%. Ускорение
    Ref.MaxSpeed = FlashMemory.MaxSpeed;         // Скорость, 50-100%. Максимальная скорость, об/мин
    Ref.MinSpeed = FlashMemory.MinSpeed;         // Скорость, 50-100%. Минимальная скорость, об/мин

    Ref.TimePiercing = FlashMemory.TimePiercing;             // Временные задержки. Время пробития, мс
    Ref.TimeBeforeControlZ = FlashMemory.TimeBeforeControlZ; // Временные задержки. Время перед началом контроля Z, мс
                                                             //  Memory.updateNow();                                  // запишем во флэш памяти
  }
  else // просто сохраним во флэш память
  {
    FlashMemory.DeltaVoltage = Ref.DeltaVoltage; // Заданное напряжение, В. Дельта напряжения, В
    FlashMemory.Sensitivity = Ref.Sensitivity;   // Заданное напряжение, В. Чувствительность, 50-100%

    FlashMemory.TechMaxVoltagePotenciometr = Ref.TechMaxVoltagePotenciometr; // Заданное напряжение, В. Технический максимум напряжения, при макс значении на потенциометре, В
    FlashMemory.TechMinVoltagePotenciometr = Ref.TechMinVoltagePotenciometr; // Заданное напряжение, В. Технический минимум напряжения, при мин значении на потенциометре, В
    FlashMemory.MinVoltageControlZ = Ref.MinVoltageControlZ;                 // Заданное напряжение, В. Минимальное напряжение выше которого идет контроль оси Z

    FlashMemory.StepAngle = Ref.StepAngle;       // Скорость, 50-100%. Угол шага, °
    FlashMemory.Acceleration = Ref.Acceleration; // Скорость, 50-100%. Ускорение
    FlashMemory.MaxSpeed = Ref.MaxSpeed;         // Скорость, 50-100%. Максимальная скорость, об/мин
    FlashMemory.MinSpeed = Ref.MinSpeed;         // Скорость, 50-100%. Минимальная скорость, об/мин

    FlashMemory.TimePiercing = Ref.TimePiercing;             // Временные задержки. Время пробития, мс
    FlashMemory.TimeBeforeControlZ = Ref.TimeBeforeControlZ; // Временные задержки. Время перед началом контроля Z, мс
    Memory.updateNow();                                      // запишем во флэш памяти
  }
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
    SaveValueToFlash(false, false); // вызов функции Записи значений
    // BlinkBlueSaveToFlash.Start = true; // запустим моргалку синего диода
  }
  else if (ExitSaveCalibr) // при выходе из параметра без сохранения
  {
    ExitSaveCalibr = false; // сбросим что вышли из параметра
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
}

//------------------------------------------------------------
// Функция для инициализации значений по умолчанию
//------------------------------------------------------------
void DefaultValueInit()
{
  FlashMemory.DeltaVoltage = 3.0;  // Заданное напряжение, В. Дельта напряжения, В
  FlashMemory.Sensitivity = 100.0; // Заданное напряжение, В. Чувствительность, 50-100%

  FlashMemory.TechMaxVoltagePotenciometr = 200.0; // Заданное напряжение, В. Технический максимум напряжения, при макс значении на потенциометре, В
  FlashMemory.TechMinVoltagePotenciometr = 100.0; // Заданное напряжение, В. Технический минимум напряжения, при мин значении на потенциометре, В
  FlashMemory.MinVoltageControlZ = 30.0;          // Заданное напряжение, В. Минимальное напряжение выше которого идет контроль оси Z

  FlashMemory.StepAngle = 1.8;     // Скорость, 50-100%. Угол шага, °
  FlashMemory.Acceleration = 80.0; // Скорость, 50-100%. Ускорение
  FlashMemory.MaxSpeed = 1200.0;   // Скорость, 50-100%. Максимальная скорость, об/мин
  FlashMemory.MinSpeed = 100.0;    // Скорость, 50-100%. Минимальная скорость, об/мин

  FlashMemory.TimePiercing = 100.0;     // Временные задержки. Время пробития, мс
  FlashMemory.TimeBeforeControlZ = 3.0; // Временные задержки. Время перед началом контроля Z, мс
}
//------------------------------------------------------------
