#ifndef _MENU_H
#define _MENU_H

#include <Arduino.h>
#include "Timers.h"

// ---------------------- Константы ----------------------
#define MAX_LENGHT_STR 100                     // Максимальная длина строки в байтах и это максимальное количество элементов массива ArrStr
#define MAX_STR 20                             // Максимальное количество строк на экране
#define MAX_QTY_SCR 20                         // Максимальное количество экранов
#define MAX_COLUMNS_LCD 21                     // Максимальное количество символов в строке дисплея
#define MAX_STR_DISPLAY 8                      // максимальное количество строк на одном экране
#define NOM_BEGIN_STR 0                        // Номер начальной строки при загрузке LCD
#define NOM_BEGIN_SCR 0                        // Номер начального экрана при загрузке LCD
#define QUALITY_PARAM_LOW_LIMIT 1              // Нижний предел достоверности
#define QUALITY_PARAM_HIGH_LIMIT 2             // Верхний предел достоверности
#define AIN_NORM 0                             // Аналоговый вход. Сигнал в норме
#define SYMBOL_DEEGREE "\xdf"                  // символ градуса °
#define TIME_SHIFT_STR 100                     // время задержки перед сдвигом символа в строке
#define TIME_SHIFT_BEGIN_SYMBOL 1000           // время  задержки перед сдвигом начльного символа в строке
#define SYMBOL_CHOISE_STR ">"                  // символ выбранной строки
#define SYMBOL_CHOISE_PARAM "<"                // символ выбранного параметра
#define SYMBOL_SPACE String(char(32))          // символ "пробел"
#define STR_FROM_SPACE "                     " // строка из пробелов (пустая)

//--------------------------------
//  Структура одной строки
struct StructStr
{
    String Str[5];                // здесь хранится что выводим на дисплей
    const bool WithParam;         // указывает есть ли параметр
    const bool WithParamReadOnly; // указывает есть ли параметр только для чтения
    const bool WithUnits;         // указывает есть ли единицы измерения
    const bool WithGoOtherScreen; // указывает есть ли переход на другой экран с этой строки
    float Param;                  // параметр
    const uint16_t QtyStr;        // количество строк на экране
    const uint16_t NomScrGoto;    // номер экрана, куда переходим
    const uint16_t NomStrGoto;    // номер строки, куда переходим
    float MaxValueParam;          // максимальное значение параметра
    float MinValueParam;          // минимальное значение параметра
    float DeltaChangeParam;       // дельта изменения параметра
    const bool isBoolParam;       // параметр типа bool
    bool BoolParam;               // значение параметра типа bool
    bool ExitFromParam;           // флаг что вышли из параметра
    uint8_t QualityParam;         // качество параметра, 0-норма, 1-НПД, 2-ВПД
};
//--------------------------------

//--------------------------------
// Структура текущего экрана
struct StructCurrentScr
{
    String Str[MAX_STR_DISPLAY][5]; // здесь хранится то, что выводим на дисплей
};
//--------------------------------

// ---------------------- Глобальные данные ----------------------
extern StructStr Screens[MAX_QTY_SCR][MAX_STR]; // объявление (определение в .cpp)

//===================================
class Screen
{
public:
    // конструктор

    Screen(uint16_t QtyScreen)
    {
        ;
        //_QtyStr = QtyStr; // количество строк на экране
        _QtyScr = QtyScreen;           // количество экранов
        NumCurrentStr = NOM_BEGIN_STR; //
        NumCurrentScr = NOM_BEGIN_SCR;
        Screens[NumCurrentScr][NumCurrentStr].Str[0] = SYMBOL_CHOISE_STR; // поставим знак при загруке LCD
        SearchPrevNextStr();
        for (int i = 0; i < MAX_LENGHT_STR; i++) // зарезервируем память на массив строк utf-8, каждый элемент массива это один символ
        {
            ArrStr[i].reserve(3); // Резервируем 3 байта (2 байта на UTF-8 + null-терминатор)
        }
        for (int i = 0; i < MAX_STR_DISPLAY; i++) // зарезервируем память на массив строк utf-8, каждый элемент массива это один символ
        {
            StrForLCD[i].reserve(MAX_LENGHT_STR);    // Резервируем 100 байт на строку экрана
            StrForLCDOld[i].reserve(MAX_LENGHT_STR); // Резервируем 100 байт на строку экрана
        }
    }

    StructCurrentScr CurrentScreen; // структура текущего экрана, состоит из 4х строк в пять рядов

    uint16_t NumCurrentScr; // Номер текущего экрана
    uint16_t NumCurrentStr; // Номер текущей строки

    void RefreshData(bool BtnEnc, bool isLeftRotate, bool isRightRotate);
    void Print();
    bool ChangeItem;                            // признак что изменяем параметр
    bool NeedUpdateDisplay;                     // признак необходимости обновить данные на дисплее
    bool NeedUpdateStrDisplay[MAX_STR_DISPLAY]; // признак необходимости обновить строку на дисплее,чтоб быстрее работало
    void FormCurrentScreen4Str();
    String StrForLCD[MAX_STR_DISPLAY];                                  // массив строк на для LCD
    String StrForLCDOld[MAX_STR_DISPLAY];                               // массив старых строк на для LCD
    void StrEqual(String &OldStr, String &NewStr, bool &NeedUpdateStr); // функция сравнения двух строк
    void UpdateParam();                                                 // функция обновления параметра, вызывать циклически
    bool AutoLightOff;                                                  // Автоматичекое отключение подсветки
    uint16_t TimeAutoLightOff;                                          // Время для автоматического отключения подсветки LCD
    uint16_t TimeAutoGotoBeginScr;                                      // Время для автоматического перехода на начальный экран
    void GotoScrAndStr(uint16_t NumScr, uint16_t NumStr);               // Переход на экран по номеру экрана

private:
    uint16_t i, j;
    uint16_t _QtyStr;         // количество строк на экране
    uint16_t _QtyScr;         // количество экранов
    uint16_t QtyStr();        // определение количества строк на данном экране
    void SearchPrevNextStr(); // поиск предыдущей и следующей строки, куда можно перейти с данного экрана
    uint16_t NumStrNext, NumStrPrev;
    void FormCurrentScreen();                                                                                                          // формирование текущего экрана для дисплея
    void InsertStr(String(&SourceStr), String(&SecondStr), uint16_t FromPosition);                                                     // функция для вставки в исходную строку второй строки начиная с позиции
    void InsertStrBeginSymbolSecondStr(String(&ResultStr), String SecondStr, uint16_t SizeResultStr, uint16_t BeginPositionSecondStr); // функция для вставки в результирующую строку второй строки начиная с позиции второй строки
    TON TonOffLight;                                                                                                                   // таймер задержки отключения подсветки
    TON TonAutoGotoBeginScr;                                                                                                           // таймер задержки перехода на начальный экран и строку
    void CutStr(float Param, String(&Str), float DeltaChangeParam);                                                                    // функция обрезания лишних символов в строке Параметра
    void StrBoolParam(String(&Str), bool BoolParam);                                                                                   // функция формирования строки из параметра типа bool
    bool StrQualityParam(String(&Str), uint8_t QualityParam);                                                                          // функция формирования строки из качества параметра, возвращает True при параметре в норме
    void _AutoLightOff(bool NeedLigthOff);                                                                                             // функция определения отключения подсветки экрана
    TON TonShiftStr;                                                                                                                   // таймер для сдвига строки
    void _RunningStr();                                                                                                                // функция бегущая строка
    bool _LengthStrOk(String(&Item), uint16_t LengthSub, uint16_t(&QtySymbols));                                                       // функция проверка что строка уберется
    uint16_t _RunningCount[MAX_STR_DISPLAY];                                                                                           // счетчик сдвига строки
    uint16_t _QtySymbolsForView;                                                                                                       // количество символов для отображения поля "наименование"
    void _AutoGotoBeginScr(bool NeedGoto);                                                                                             // функция автоматического перехода на начальный экран
    uint16_t _StrLengthUTF8(String(&SourceStr));                                                                                       // функция определения длины строки в символах, кодировка utf-8
    String ArrStr[MAX_LENGHT_STR];                                                                                                     // массив строк
    void _TransferStrToArrStr(String(&SourceStr), uint16_t NumElemArray);                                                              // функция перекладки строки в символах в массив строк, кодировка utf-8
    bool _LengthStrOkUTF8(String(&Item), uint16_t LengthSub, uint16_t(&QtySymbols));                                                   // функция проверка что строка уберется в UTF8
    void _RunningStrUTF8();                                                                                                            // функция бегущая строка в UTF8
    void _SubStrUTF8(String(&SourceStr), uint16_t BeginPosition, uint16_t EndPosition);                                                // функция обрезки строки от начальной до конечной позиции UTF8
    TON TmrShiftBeginSymbol;                                                                                                           // таймер на задержку сдвига первого символа
    bool TmrShiftBeginSymbolIn;                                                                                                        // флаг запуска таймера на задержку сдвига первого символа
};
//===================================
#endif