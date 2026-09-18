#ifndef _BLINK_STROB_START
#define _BLINK_STROB_START

#include "Timers.h"

class BlinkStrob
{
public:
    BlinkStrob() { ; }
    void Blink();
    bool BlinkStart;           // включить
    bool StrobOn;              // включаем стробоскоп
    bool TurnOn;               // включаем поворотники
    bool BuzzerOn;             // включить буззер
    uint32_t TimeStrob = 1000; // время работы стробоскопа
    uint32_t TimeTurn = 1000;  // время работы повортников
    uint32_t TimeBuzzer = 500; // время работы буззера

private:
    TON TonStrob;  // таймер длительности включения стробоскопа
    TON TonTurn;   // таймер длительностивключения поворотников
    TON TonBuzzer; // таймер длительностивключения буззера
};

#endif