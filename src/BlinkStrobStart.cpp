#include "BlinkStrobStart.h"

void BlinkStrob::Blink()
{
    TonStrob.TONTmr(BlinkStart, TimeStrob);   // запускаем таймер работы стробоскопа
    TonTurn.TONTmr(TonStrob.Q, TimeTurn);     // запускаем таймер работы поворотников
    TonBuzzer.TONTmr(BlinkStart, TimeBuzzer); // запускаем таймер работы буззера
    StrobOn = false;                          // сбрасываем включение стробоскопа
    TurnOn = false;                           // сбрасываем включение поворотников
    BuzzerOn = false;                         // сбрасываем включение буззера
    if (!TonStrob.Q and BlinkStart)           // если нет сработки таймера работы стробоскопа и не завершилось моргание
    {   
        StrobOn = true; // включаем стробоскоп
       
        if (!TonBuzzer.Q) // если не отработал таймер буззера
        {
            BuzzerOn = true;// включаем буззер
        }
    }
    else if (TonStrob.Q) // если сработал таймер работы стробоскопа
    {
        if (!TonTurn.Q) // если нет сработки таймера работы поворотников
        {
            TurnOn = true; // включаем поворотники
        }
        else
        {
            BlinkStart = false; // снимаем флаг, что завершили стартовое моргание
        }
    }

}
