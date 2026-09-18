#include "Menu.h"

//------------------------------------------------------------
// Объявляем структуру экранов, и заполним её
//------------------------------------------------------------
StructStr Screens[MAX_QTY_SCR][MAX_STR] = // структура экрана
    {
        // Экран 0 - Главный
        {
            /*0*/ {" ", "Т масла наружного воздуха", "", " °C", "", true, true, true, true, 10.0, 7, 1, 0, 150.0, -50.0, 0.1, false, false, false, 0},              // Температура масла
            /*1*/ {" ", "Т воды", "", " °C", "", true, true, true, true, 10.0, 7, 2, 0, 150.0, -50.0, 0.1, false, false, false, 0},               // Температура воды
            /*2*/ {" ", "Т наружного воздуха", "", " °C", "", true, true, true, false, 10.0, 7, 0, 0, 150.0, -50.0, 0.1, false, false, false, 0}, // Темп. нар. воздуха
            /*3*/ {" ", "Обороты двигателя", "", " об/мин", "", true, true, true, true, 10.0, 7, 3, 0, 5000.0, 0.0, 1.0, false, false, false, 0}, // Обороты двигателя
            /*4*/ {" ", "Скорость", "", " км/ч", "", true, true, true, true, 10.0, 7, 4, 0, 150.0, -50.0, 1.0, false, false, false, 0},           // Скорость
            /*5*/ {" ", "Поворотники", "", "", "", false, false, false, true, 10.0, 7, 6, 0, 0.0, 0.0, 0.0, false, false, false, 0},              // Поворотники
            /*6*/ {" ", "Настройки", "", "", "", false, false, false, true, 10.0, 7, 5, 0, 0.0, 0.0, 0.0, false, false, false, 0},                // Настройки
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 7, 0, 0, 100.0, 0.0, 0.1, false, false, false, 0},                      //
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 7, 0, 0, 100.0, 0.0, 0.0, false, false, false, 0},                      //
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 7, 0, 0, 100.0, 0.0, 0.0, false, false, false, 0},                      //
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 7, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       //
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 7, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       //
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 7, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       //
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 7, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       //
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 7, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       //
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 7, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       //
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 7, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       //
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 7, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       //
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 7, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       //
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 7, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}                        //
            //
        },
        // Экран 1 - Т масла
        {
            /*0*/ {" ", "Т масла", "", " °C", "", true, true, true, true, 10.0, 5, 0, 0, 150.0, -50.0, 0.1, false, false, false, 0},              // Т масла
            /*1*/ {" ", "T Включения", "", " °C", " ", true, false, true, false, 82.0, 5, 0, 0, 150.0, -50.0, 1.0, false, false, false, 0},       // Т масла. T Включения
            /*2*/ {" ", "T Отключения", "", " °C", " ", true, false, true, false, 80.0, 5, 0, 0, 150.0, -50.0, 1.0, false, false, false, 0},      // Т масла. T Отключения
            /*3*/ {" ", "Мин. Скорость", "", " %", " ", true, false, true, false, 50.0, 5, 0, 0, 100.0, 30.0, 1.0, false, false, false, 0},       // Т масла. Мин. скорость
            /*4*/ {" ", "Макс. Температура", "", " °C", " ", true, false, true, false, 100.0, 5, 0, 0, 100.0, 30.0, 1.0, false, false, false, 0}, // Т масла. Максимальная температура, при которой максимальная скорость вентилятора
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                        // Т масла.
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                        // Т масла.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                        // Т масла.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                        // Т масла.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                        // Т масла.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Т масла.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Т масла.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Т масла.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Т масла.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Т масла.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Т масла.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Т масла.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Т масла.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Т масла.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Т масла.
            //
        },
        // Экран 2 - Т воды
        {
            /*0*/ {" ", "Т воды", "", " °C", "", true, true, true, true, 10.0, 5, 0, 1, 150.0, -50.0, 0.1, false, false, false, 0},                    // Т воды
            /*1*/ {" ", "T Включения 1ого вент", "", " °C", " ", true, false, true, false, 82.0, 5, 1, 0, 150.0, -50.0, 1.0, false, false, false, 0},  // Т воды. T Включения 1ого вент
            /*2*/ {" ", "T Отключения 1ого вент", "", " °C", " ", true, false, true, false, 80.0, 5, 1, 0, 150.0, -50.0, 1.0, false, false, false, 0}, // Т воды. T Отключения 1ого вент
            /*3*/ {" ", "Мин. Скорость 1ого вент", "", " %", " ", true, false, true, false, 50.0, 5, 1, 0, 100.0, 30.0, 1.0, false, false, false, 0},  // Т воды. Мин. Скорость 1ого вент
            /*4*/ {" ", "Макс. Температура", "", " %", " ", true, false, true, false, 100.0, 5, 1, 0, 150.0, 30.0, 1.0, false, false, false, 0},       // Т воды. Максимальная температура, при которой максимальная скорость вентилятора
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                             // Т воды.
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                             // Т воды.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                             // Т воды.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                             // Т воды.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                             // Т воды.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                            // Т воды.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                            // Т воды.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                            // Т воды.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                            // Т воды.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                            // Т воды.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                            // Т воды.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                            // Т воды.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                            // Т воды.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                            // Т воды.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                            // Т воды.
            //
        },
        // Экран 3 - Обороты двигателя
        {
            /*0*/ {" ", "Обороты двигателя", "", " об/мин", "", true, true, true, true, 0.0, 3, 0, 3, 5000.0, 0.0, 1.0, false, false, false, 0}, // Обороты двигателя, об/мин
            /*1*/ {" ", "Отображать на мал. диспл", "", "", " ", true, false, true, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, true, false, false, 0}, // Обороты двигателя, об/мин. Отображать на маленьком дисплее
            /*2*/ {" ", "Коэффициент домножения", "", "", " ", true, false, true, false, 1.0, 3, 0, 0, 60.0, 1.0, 1.0, false, false, false, 0},  // Обороты двигателя, об/мин. Коэффициент домножения
            /*3*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Обороты двигателя, об/мин.
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Обороты двигателя, об/мин.
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Обороты двигателя, об/мин.
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Обороты двигателя, об/мин.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Обороты двигателя, об/мин.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Обороты двигателя, об/мин.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Обороты двигателя, об/мин.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Обороты двигателя, об/мин.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Обороты двигателя, об/мин.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Обороты двигателя, об/мин.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Обороты двигателя, об/мин.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Обороты двигателя, об/мин.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Обороты двигателя, об/мин.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Обороты двигателя, об/мин.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Обороты двигателя, об/мин.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Обороты двигателя, об/мин.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 3, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Обороты двигателя, об/мин.
            //
        },
        // Экран 4 - Скорость
        {
            /*0*/ {" ", "Скорость", "", " км/ч", "", true, true, true, true, 0.0, 5, 0, 4, 150.0, -50.0, 1.0, false, false, false, 0},           // Скорость, км/час
            /*1*/ {" ", "Отображать на мал. диспл", "", "", " ", true, false, true, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, true, false, false, 0}, // Скорость, км/час. Отображать на маленьком дисплее
            /*2*/ {" ", "Диаметр колеса", "", " см", " ", true, false, true, false, 75.0, 5, 0, 0, 100.0, 70.0, 1.0, false, false, false, 0},    // Скорость, км/час. Диаметр колеса, см
            /*3*/ {" ", "Редукция на мосте", "", "", " ", true, false, true, false, 3.0, 5, 0, 0, 6.0, 0.00, 0.1, false, false, false, 0},       // Скорость, км/час. Редукция на мосте
            /*4*/ {" ", "Коэффициент домножения", "", "", " ", true, false, true, false, 1.0, 5, 0, 0, 60.0, 1.0, 1.0, false, false, false, 0},  // Скорость, км/час. Коэффициент домножения
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Скорость, км/час.
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Скорость, км/час.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Скорость, км/час.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Скорость, км/час.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                       // Скорость, км/час.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Скорость, км/час.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Скорость, км/час.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Скорость, км/час.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Скорость, км/час.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Скорость, км/час.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Скорость, км/час.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Скорость, км/час.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Скорость, км/час.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Скорость, км/час.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 5, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                      // Скорость, км/час.
            //
        },
        // Экран 5 - Настройки
        {
            /*0*/ {" ", "Настройки", "", "", "", false, false, false, true, 10.0, 6, 0, 6, 0.0, 0.0, 0.0, false, false, false, 0},                        // Настройки
            /*1*/ {" ", "Сохранить в EEPROM", "", "", " ", true, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, true, false, false, 0},               // Настройки. Сохранить в EEPROM
            /*2*/ {" ", "Моргание при старте", "", "", " ", true, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, true, false, false, 0},              //. Моргание стробосками и поворотниками при старте
            /*3*/ {" ", "Время вкл.строб", "", " мс", " ", true, false, true, false, 1000.0, 6, 0, 0, 4000.0, 0.0, 100.0, false, false, false, 0},        //. Время включения стробоскопов, мсек
            /*4*/ {" ", "Время вкл.поворотников", "", " мс", " ", true, false, true, false, 1000.0, 6, 0, 0, 4000.0, 0.0, 100.0, false, false, false, 0}, //. Время вкл.поворотников, мсек
            /*5*/ {" ", "Заводские настройки", "", "", " ", true, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, true, false, false, 0},              //. Загрузить заводские настройки
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                //.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                //.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                //.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                //.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                               //.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                               //.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                               //.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                               //.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                               //.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                               //.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                               //.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                               //.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                               //.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 6, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                               //.
            //
        },
        // Экран 6 - Поворотники
        {
            /*0*/ {" ", "Поворотники", "", "", "", false, false, false, true, 10.0, 4, 0, 5, 0.0, 0.0, 0.0, false, false, false, 0},                        // Поворотники
            /*1*/ {" ", "Время в откл состоянии", "", " мс", " ", true, false, true, false, 1000.0, 4, 0, 0, 4000.0, 0.0, 100.0, false, false, false, 0},   // Поворотники. Время в отключенном состоянии, мс
            /*2*/ {" ", "Время в вкл. состоянии", "", " мс", " ", true, false, true, false, 1000.0, 4, 0, 0, 4000.0, 0.0, 100.0, false, false, false, 0},   // Поворотники. Время в включенном состоянии, мс
            /*3*/ {" ", "Время выдачи звука на буззер", "", " мс", " ", true, false, true, false, 10.0, 4, 0, 0, 500.0, 0.0, 10.0, false, false, false, 0}, // Поворотники. Время выдачи звука на буззер, мс
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                  // Поворотники.
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                  // Поворотники.
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                  // Поворотники.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                  // Поворотники.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                  // Поворотники.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                  // Поворотники.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                 // Поворотники.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                 // Поворотники.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                 // Поворотники.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                 // Поворотники.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                 // Поворотники.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                 // Поворотники.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                 // Поворотники.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                 // Поворотники.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                 // Поворотники.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},                                 // Поворотники.
            //
        },
        // Экран 7 -
        {
            /*0*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 5, 0.0, 0.0, 0.0, false, false, false, 0},        //
            /*1*/ {" ", "", "", "", "", false, false, false, false, 1000.0, 4, 0, 0, 4000.0, 0.0, 100.0, false, false, false, 0}, //.
            /*2*/ {" ", "", "", "", "", false, false, false, false, 1000.0, 4, 0, 0, 4000.0, 0.0, 100.0, false, false, false, 0}, //.
            /*3*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 500.0, 0.0, 10.0, false, false, false, 0},     //.
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},        //.
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},        //.
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},        //.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},        //.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},        //.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},        //.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},       //.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},       //.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},       //.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},       //.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},       //.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},       //.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},       //.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},       //.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},       //.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 4, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},       //.
            //
        },
        // Экран 8 -
        {
            /*0*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*1*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*2*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*3*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 0, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            //
        },
        // Экран 9 -
        {
            /*0*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 100.0, 0.0, 0.1, false, false, false, 0}, //
            /*1*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},   //.
            /*2*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},   //.
            /*3*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 2, 0, 0.0, 0.0, 0.0, false, false, false, 0},   //.
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},   //.
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},   //.
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},   //.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},   //.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},   //.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},   //.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            //
        },
        // Экран 10 -
        {
            /*0*/ {" ", "", "", "", "", false, false, false, true, 10.0, 1, 0, 0, 100.0, 0.0, 0.1, false, false, false, 0}, //
            /*1*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*2*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*3*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 2, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            //
        },
        // Экран 11 -
        {
            /*0*/ {" ", "", "", "", "", false, false, false, true, 10.0, 1, 0, 0, 100.0, 0.0, 0.1, false, false, false, 0}, //
            /*1*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*2*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*3*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 2, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            //
        },
        // Экран 12 -
        {
            /*0*/ {" ", "", "", "", "", false, false, false, true, 10.0, 1, 0, 0, 100.0, 0.0, 0.1, false, false, false, 0}, //
            /*1*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*2*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*3*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 2, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            //
        },
        // Экран 13 -
        {
            /*0*/ {" ", "", "", "", "", false, false, false, true, 10.0, 1, 0, 0, 100.0, 0.0, 0.1, false, false, false, 0}, //
            /*1*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*2*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*3*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 2, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            //
        },
        // Экран 14 -
        {
            /*0*/ {" ", "", "", "", "", false, false, false, true, 10.0, 1, 0, 0, 100.0, 0.0, 0.1, false, false, false, 0}, //
            /*1*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*2*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*3*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 2, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            //
        },
        // Экран 15 -
        {
            /*0*/ {" ", "", "", "", "", false, false, false, true, 10.0, 1, 0, 0, 100.0, 0.0, 0.1, false, false, false, 0}, //
            /*1*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*2*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*3*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 2, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //.
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //.
            //
        },
        // Экран 16 -
        {
            /*0*/ {" ", "", "", "", "", false, false, false, true, 10.0, 1, 0, 0, 100.0, 0.0, 0.1, false, false, false, 0}, //
            /*1*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*2*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*3*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 2, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}  //
            //
        },
        // Экран 17 -
        {
            /*0*/ {" ", "", "", "", "", false, false, false, true, 10.0, 1, 0, 0, 100.0, 0.0, 0.1, false, false, false, 0}, //
            /*1*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*2*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*3*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 2, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}  //
            //
        },
        // Экран 18 -
        {
            /*0*/ {" ", "", "", "", "", false, false, false, true, 10.0, 1, 0, 0, 100.0, 0.0, 0.1, false, false, false, 0}, //
            /*1*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*2*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*3*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 2, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}  //
            //
        },
        // Экран 19 -
        {
            /*0*/ {" ", "", "", "", "", false, false, false, true, 10.0, 1, 0, 0, 100.0, 0.0, 0.1, false, false, false, 0}, //
            /*1*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*2*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*3*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 2, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*4*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*5*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*6*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*7*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*8*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*9*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0},  //
            /*10*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*11*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*12*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*13*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*14*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*15*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*16*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*17*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*18*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}, //
            /*19*/ {" ", "", "", "", "", false, false, false, false, 10.0, 1, 0, 0, 0.0, 0.0, 0.0, false, false, false, 0}  //
            //
        }
        //
}; // Структура экранов
//------------------------------------------------------------

//------------------------------------------------------------

//--------------------------------------
void Screen::Print()
{
  for (i = 0; i < MAX_STR_DISPLAY; i++)
  {
    for (j = 0; j < 5; j++)
    {
      Serial.print(CurrentScreen.Str[i][j]);
    }
    Serial.println();
  }

  Serial.println("===================");
}
//--------------------------------------

//--------------------------------------
uint16_t Screen::QtyStr() // определение количества строк на данном экране
{
  return Screens[NumCurrentScr][NumCurrentStr].QtyStr;
}
//--------------------------------------

//--------------------------------------
void Screen::SearchPrevNextStr() // поиск предыдущей и следующей строк, куда можно перейти на данном экране
{
  _QtyStr = QtyStr(); // вызов функции определения количества строк в текущем экране

  // поиск следующей строки для перехода
  for (i = ((NumCurrentStr + 1) >= _QtyStr) ? 0 : (NumCurrentStr + 1); i < _QtyStr; i++)
  {
    if ((Screens[NumCurrentScr][i].WithParam and !Screens[NumCurrentScr][i].WithParamReadOnly) or Screens[NumCurrentScr][i].WithGoOtherScreen) // если строка с параметром или с переходом на другой экран
    {
      NumStrNext = i; // номер следующей строки
      // Serial.println("NumStrNext Found UP");
      break;
    }
  }
  if (i == _QtyStr) // если дошли до конца строк на экране и не нашли строки с параметром или с переходом на другой экран
  {
    for (i = 0; i < NumCurrentStr; i++)
    {
      if ((Screens[NumCurrentScr][i].WithParam and !Screens[NumCurrentScr][i].WithParamReadOnly) or Screens[NumCurrentScr][i].WithGoOtherScreen) // если строка с параметром или с переходом на другой экран
      {
        NumStrNext = i; // номер следующей строки
        // Serial.println("NumStrNext Found DOWN");
        break;
      }
    }
    if (i == NumCurrentStr) // если вообще не нашли строки с параметром или с переходом на другой экран
    {
      NumStrNext = i; // номер следующей строки по факту совпадет с текущей строкой
      Serial.println("NumStrNext Not found");
    }
  }

  // поиск предыдущей строки для перехода
  for (i = (NumCurrentStr == 0) ? (_QtyStr - 1) : (NumCurrentStr - 1); i > 0; i--)
  {
    if ((Screens[NumCurrentScr][i].WithParam and !Screens[NumCurrentScr][i].WithParamReadOnly) or Screens[NumCurrentScr][i].WithGoOtherScreen) // если строка с параметром или с переходом на другой экран
    {
      NumStrPrev = i; // номер следующей строки
      // Serial.println("NumStrPrev Found UP");
      break;
    }
  }

  if (i == 0) // если дошли до конца строк на экране и не нашли строки с параметром или с переходом на другой экран
  {
    if ((Screens[NumCurrentScr][i].WithParam and !Screens[NumCurrentScr][i].WithParamReadOnly) or Screens[NumCurrentScr][i].WithGoOtherScreen) // если строка с параметром или с переходом на другой экран
    {
      NumStrPrev = i; // номер следующей строки
    }
    else
    {
      for (i = (_QtyStr - 1); i > NumCurrentStr; i--)
      {
        if ((Screens[NumCurrentScr][i].WithParam and !Screens[NumCurrentScr][i].WithParamReadOnly) or Screens[NumCurrentScr][i].WithGoOtherScreen) // если строка с параметром или с переходом на другой экран
        {
          NumStrPrev = i; // номер следующей строки
          break;
        }
      }
    }
    if (i == NumCurrentStr) // если вообще не нашли строки с параметром или с переходом на другой экран
    {
      NumStrPrev = i; // номер следующей строки по факту совпадет с текущей строкой
      Serial.println("NumStrPrev Not found");
    }
  }
}
//--------------------------------------

//--------------------------------------
void Screen::RefreshData(bool BtnEnc, bool isLeftRotate, bool isRightRotate)
{
  _AutoLightOff(!isRightRotate and !isLeftRotate and !BtnEnc);                                                                                // вызов функции автоматического отключения подсветки экрана
  _AutoGotoBeginScr(!isRightRotate and !isLeftRotate and !BtnEnc);                                                                            // вызов функции автоматического перехода на начальный экран
  TonShiftStr.TONTmr(!TmrShiftBeginSymbolIn and !TonShiftStr.Q and !isRightRotate and !isLeftRotate, TIME_SHIFT_STR);                         // вызов запуск таймера сдвига строки
  TmrShiftBeginSymbol.TONTmr(TmrShiftBeginSymbolIn and !TmrShiftBeginSymbol.Q and !isRightRotate and !isLeftRotate, TIME_SHIFT_BEGIN_SYMBOL); // запуск таймера на задержку сдвига первого символа
  UpdateParam();                                                                                                                              // вызов функции обновления параметра

  if (BtnEnc) // если нажата кнопка энкодера
  {
    if (Screens[NumCurrentScr][NumCurrentStr].WithParam and !Screens[NumCurrentScr][NumCurrentStr].WithParamReadOnly and !Screens[NumCurrentScr][NumCurrentStr].WithGoOtherScreen) // если строка содержит параметр и параметр не для чтения и нет перехода на другой экран
    {
      if (ChangeItem) // если параметр еще выбран
      {
        ChangeItem = false; // сбросим, что изменяем пункты
        Screens[NumCurrentScr][NumCurrentStr].Str[0] = SYMBOL_CHOISE_STR;
        Screens[NumCurrentScr][NumCurrentStr].Str[4] = " ";
        NeedUpdateDisplay = true;                                   // нужно обновить дисплей
        Screens[NumCurrentScr][NumCurrentStr].ExitFromParam = true; // установим флаг что вышли из параметра
        // Serial.println(CurrentChoiseItem);
      }
      else
      {
        ChangeItem = true; // установим, что изменяем пункты
        Screens[NumCurrentScr][NumCurrentStr].Str[0] = " ";
        Screens[NumCurrentScr][NumCurrentStr].Str[4] = SYMBOL_CHOISE_PARAM;
        Screens[NumCurrentScr][NumCurrentStr].ExitFromParam = false; // сбросим флаг что вышли из параметра
        NeedUpdateDisplay = true;                                    // нужно обновить дисплей
      }
    }
    else if (!Screens[NumCurrentScr][NumCurrentStr].WithParam and !Screens[NumCurrentScr][NumCurrentStr].WithGoOtherScreen) // если экран не содержит параметр и нет перехода на другой экран
    {
      ChangeItem = false; // сбросим, что изменяем пункты
    }
    else if ((Screens[NumCurrentScr][NumCurrentStr].WithParam and Screens[NumCurrentScr][NumCurrentStr].WithParamReadOnly and Screens[NumCurrentScr][NumCurrentStr].WithGoOtherScreen) // если экран содержит параметр и параметр для чтения и есть переход на другой экран
             or (!Screens[NumCurrentScr][NumCurrentStr].WithParam and Screens[NumCurrentScr][NumCurrentStr].WithGoOtherScreen))                                                        // или если не содержит параметр и есть переход на другой экран
    {
      ChangeItem = false;                                 // сбросим, что изменяем пункты
      Screens[NumCurrentScr][NumCurrentStr].Str[0] = " "; // стираем стрелку со строки текущего экрана
      uint16_t tmpNumCurrentStr = Screens[NumCurrentScr][NumCurrentStr].NomStrGoto;
      NumCurrentScr = Screens[NumCurrentScr][NumCurrentStr].NomScrGoto;
      NumCurrentStr = tmpNumCurrentStr;
      // Serial.print("NumCurrentStr = "); Serial.println(NumCurrentStr);
      // Serial.print("NumCurrentScr = "); Serial.println(NumCurrentScr);
      Screens[NumCurrentScr][NumCurrentStr].Str[0] = SYMBOL_CHOISE_STR; // отобразим стрелку у строки на экране, куда перешли

      NeedUpdateDisplay = true; // нужно обновить дисплей
    }
  }

  if (isLeftRotate) // если поворот энкодера против часовой
  {
    SearchPrevNextStr(); // поиск предыдущей и следующей строк, куда можно перейти на данном экране
    // Serial.print("NumStrNext = "); Serial.println(NumStrNext);
    // Serial.print("NumStrPrev = "); Serial.println(NumStrPrev);

    if (!ChangeItem) // если параметр еще не выбран
    {
      Screens[NumCurrentScr][NumCurrentStr].Str[0] = " ";
      Screens[NumCurrentScr][NumStrPrev].Str[0] = SYMBOL_CHOISE_STR;
      NumCurrentStr = NumStrPrev;
      // Serial.print("NumCurrentStr = "); Serial.println(NumCurrentStr);
      NeedUpdateDisplay = true; // нужно обновить дисплей
    }
    else
    {
      if (Screens[NumCurrentScr][NumCurrentStr].isBoolParam) // если двоичный параметр
      {
        Screens[NumCurrentScr][NumCurrentStr].BoolParam = !Screens[NumCurrentScr][NumCurrentStr].BoolParam;
      }
      else // параметр float
      {
        Screens[NumCurrentScr][NumCurrentStr].Param -= Screens[NumCurrentScr][NumCurrentStr].DeltaChangeParam;
        if (Screens[NumCurrentScr][NumCurrentStr].Param < Screens[NumCurrentScr][NumCurrentStr].MinValueParam) // проверяем на выходы за пределы
        {
          Screens[NumCurrentScr][NumCurrentStr].Param = Screens[NumCurrentScr][NumCurrentStr].MaxValueParam;
        }
        // Screens[NumCurrentScr][NumCurrentStr].Str[2] = Screens[NumCurrentScr][NumCurrentStr].Param;
        NeedUpdateDisplay = true; // нужно обновить дисплей
      }
    }
  }

  if (isRightRotate) // если поворот энкодера по часовой
  {
    SearchPrevNextStr(); // поиск предыдущей и следующей строк, куда можно перейти на данном экране
    // Serial.print("NumStrNext = "); Serial.println(NumStrNext);
    // Serial.print("NumStrPrev = "); Serial.println(NumStrPrev);

    if (!ChangeItem) // если параметр еще не выбран
    {
      Screens[NumCurrentScr][NumCurrentStr].Str[0] = " ";
      Screens[NumCurrentScr][NumStrNext].Str[0] = SYMBOL_CHOISE_STR;
      NumCurrentStr = NumStrNext;
      // Serial.print("NumCurrentStr = ");
      // Serial.println(NumCurrentStr);
      NeedUpdateDisplay = true; // нужно обновить дисплей
    }
    else
    {
      if (Screens[NumCurrentScr][NumCurrentStr].isBoolParam) // если двоичный параметр
      {
        Screens[NumCurrentScr][NumCurrentStr].BoolParam = !Screens[NumCurrentScr][NumCurrentStr].BoolParam;
      }
      else // параметр float
      {
        Screens[NumCurrentScr][NumCurrentStr].Param += Screens[NumCurrentScr][NumCurrentStr].DeltaChangeParam;
        if (Screens[NumCurrentScr][NumCurrentStr].Param > Screens[NumCurrentScr][NumCurrentStr].MaxValueParam) // проверяем на выходы за пределы
        {
          Screens[NumCurrentScr][NumCurrentStr].Param = Screens[NumCurrentScr][NumCurrentStr].MinValueParam;
        }
        // Screens[NumCurrentScr][NumCurrentStr].Str[2] = Screens[NumCurrentScr][NumCurrentStr].Param;
        NeedUpdateDisplay = true; // нужно обновить дисплей
      }
    }
  }
  if (NeedUpdateDisplay) // если нужно обновить дисплей
  {
    _QtyStr = QtyStr();        // вызов функции определения количества строк в текущем экране
    FormCurrentScreen();       // сформируем текущий экран для дисплея
    NeedUpdateDisplay = false; // сбросим, что нужно обновить дисплей
  }
}
//--------------------------------------

//--------------------------------------
void Screen::FormCurrentScreen() // сформируем текущий экран для дисплея
{
  for (i = 0; i < MAX_STR_DISPLAY; i++) // по количеству строк
  {
    for (j = 0; j < 5; j++) // по количеству столбцов
    {
      if (((NumCurrentStr / (MAX_STR_DISPLAY - 1)) + i) <= _QtyStr)
      {
        ;
        CurrentScreen.Str[i][j] = Screens[NumCurrentScr][(NumCurrentStr / MAX_STR_DISPLAY) * MAX_STR_DISPLAY + i].Str[j];
      }
      else
      {
        CurrentScreen.Str[i][j] = ""; // STR_FROM_SPACE;
      }
    }
  }
  _RunningStrUTF8();
  FormCurrentScreen4Str();
}
//--------------------------------------

//--------------------------------------
uint16_t Screen::_StrLengthUTF8(String(&SourceStr)) // функция определения длины строки в символах, кодировка utf-8
{
  uint16_t IndexChar = 0;
  uint16_t StrLength = 0;

  while (IndexChar < SourceStr.length() and IndexChar < MAX_LENGHT_STR)
  {
    if (SourceStr[IndexChar] <= 127)
    {
      // ArrStr[StrLength] = SourceStr[IndexChar];
      // Serial.print("ArrStr[");
      // Serial.print(StrLength);
      // Serial.print("] = ");
      // Serial.println(ArrStr[StrLength]);
      IndexChar++;
      StrLength++;
    }
    else if ((SourceStr[IndexChar] & 0xE0) == 0xC0) //(SourceStr[IndexChar] == 208 or SourceStr[IndexChar] == 209)
    {
      // ArrStr[StrLength] = SourceStr[IndexChar];
      IndexChar++;
      // ArrStr[StrLength] += SourceStr[IndexChar];
      // Serial.print("ArrStr[");
      // Serial.print(StrLength);
      // Serial.print("] = ");
      // Serial.println(ArrStr[StrLength]);
      IndexChar++;
      StrLength++;
    }
  }
  return StrLength;
}
//--------------------------------------

//--------------------------------------
void Screen::_TransferStrToArrStr(String(&SourceStr), uint16_t NumElemArray) // функция перекладки строки в символах в массив строк, кодировка utf-8
{
  uint16_t IndexChar = 0;
  uint16_t StrLength = 0;

  if (!SourceStr.isEmpty())
  {
    while (IndexChar < SourceStr.length() and IndexChar < MAX_LENGHT_STR) // пока не дошли до конца исходной строки и индекс не превысил максимальную длину строки SourceStr[IndexChar] != 0) //
    {
      if ((SourceStr[IndexChar] <= 127) and ((StrLength + NumElemArray) < MAX_LENGHT_STR)) // если это однобайтовый символ
      {
        ArrStr[StrLength + NumElemArray] = SourceStr[IndexChar];
        IndexChar++;
        StrLength++;
      }
      else if (((SourceStr[IndexChar] & 0xE0) == 0xC0) and ((StrLength + NumElemArray) < MAX_LENGHT_STR)) //(SourceStr[IndexChar] == 208 or SourceStr[IndexChar] == 209) // если это двухбайтовый символ
      {
        ArrStr[StrLength + NumElemArray] = SourceStr[IndexChar];
        IndexChar++;
        ArrStr[StrLength + NumElemArray] += SourceStr[IndexChar];
        IndexChar++;
        StrLength++;
      }
    }
  }
  for (IndexChar = StrLength + NumElemArray; IndexChar < MAX_LENGHT_STR; IndexChar++) // цикл от того где закончился предудщий цикл While, до конца
  {
    ArrStr[IndexChar] = " "; // обнулим массив до конца
  }
}
//--------------------------------------

//--------------------------------------
void Screen::_RunningStr() // функция бегущая строка
{
  for (i = 0; i < MAX_STR_DISPLAY; i++) // по количеству строк
  {
    if (!_LengthStrOk(CurrentScreen.Str[i][1], CurrentScreen.Str[i][0].length() + CurrentScreen.Str[i][2].length() + CurrentScreen.Str[i][3].length() + CurrentScreen.Str[i][4].length() + 1, _QtySymbolsForView)) // если строка не уберется
    {
      uint16_t LengthSourceStr = CurrentScreen.Str[i][1].length(); // Длина исходной строки
      InsertStrBeginSymbolSecondStr(CurrentScreen.Str[i][1], CurrentScreen.Str[i][1], _QtySymbolsForView, _RunningCount[i]);
      // Serial.println(CurrentScreen.Str[i][1]);
      if ((CurrentScreen.Str[i][0] == SYMBOL_CHOISE_STR) or (CurrentScreen.Str[i][4] == SYMBOL_CHOISE_PARAM)) // если строка или параметр выбран
      {
        if ((LengthSourceStr - _RunningCount[i]) > _QtySymbolsForView) // если строка до своего конца не сдвинулась
        {
          if (TonShiftStr.Q) // таймер сдвига строки сработал
          {
            _RunningCount[i]++; // увеличим счетчик сдвига строки
          }
        }
        else // если строка сдвинулась до конца
        {
          if (TonShiftStr.Q) // таймер сдвига строки сработал
          {
            _RunningCount[i] = 0; // сбросим счетчик сдвига строки
          }
        }
      }
      else // если параметр не выбран
      {
        _RunningCount[i] = 0; // сбросим счетчик сдвига строки
      }
    }
    else // если строка уберется
    {
      _RunningCount[i] = 0; // сбросим счетчик сдвига строки
    }
  }
}
//--------------------------------------

//--------------------------------------
void Screen::_RunningStrUTF8() // функция бегущая строка UTF8
{
  // Serial.println("===");
  for (i = 0; i < MAX_STR_DISPLAY; i++) // по количеству строк
  {
    if (!_LengthStrOkUTF8(CurrentScreen.Str[i][1], _StrLengthUTF8(CurrentScreen.Str[i][0]) + _StrLengthUTF8(CurrentScreen.Str[i][2]) + _StrLengthUTF8(CurrentScreen.Str[i][3]) + _StrLengthUTF8(CurrentScreen.Str[i][4]) + 1, _QtySymbolsForView)) // если строка не уберется
    {
      uint16_t LengthSourceStr = _StrLengthUTF8(CurrentScreen.Str[i][1]); // Длина исходной строки в символах
      _SubStrUTF8(CurrentScreen.Str[i][1], _RunningCount[i], _RunningCount[i] + _QtySymbolsForView);
      if ((CurrentScreen.Str[i][0] == SYMBOL_CHOISE_STR) or (CurrentScreen.Str[i][4] == SYMBOL_CHOISE_PARAM)) // если строка или параметр выбран
      {
        if (_RunningCount[i] == 0) // если еще не сдвигались
        {
          TmrShiftBeginSymbolIn = true; // запустим таймер на задержку сдвига первого символа
          if (TmrShiftBeginSymbol.Q)    // если сработал таймер на задержку сдвига первого символа
          {
            TmrShiftBeginSymbolIn = false; // сбросил таймер на задержку сдвига первого символа
            _RunningCount[i]++;            // увеличим счетчик сдвига строки
          }
        }
        else if ((LengthSourceStr - _RunningCount[i]) > _QtySymbolsForView) // если строка до своего конца не сдвинулась
        {
          if (TonShiftStr.Q) // таймер сдвига строки сработал
          {
            _RunningCount[i]++; // увеличим счетчик сдвига строки
          }
        }
        else // если строка сдвинулась до конца
        {
          if (TonShiftStr.Q) // таймер сдвига строки сработал
          {
            _RunningCount[i] = 0; // сбросим счетчик сдвига строки
          }
        }
      }
      else // если параметр не выбран
      {
        _RunningCount[i] = 0; // сбросим счетчик сдвига строки
      }
    }
    else // если строка уберется
    {
      _RunningCount[i] = 0; // сбросим счетчик сдвига строки
    }
  }
}
//--------------------------------------

//--------------------------------------
void Screen::_SubStrUTF8(String(&SourceStr), uint16_t BeginPosition, uint16_t EndPosition) // функция обрезки строки от начальной до конечной позиции UTF8
{
  uint16_t i = 0;
  uint16_t StrLength = 0;
  if (BeginPosition <= EndPosition and BeginPosition >= 0 and EndPosition >= 0) // если не херню ввели
  {
    _TransferStrToArrStr(SourceStr, 0); // переложим строку в массив начиная с номера символа BeginPosition
    for (i = 0; i <= EndPosition; i++)  // цикл до конца отображаемой строки
    {
      if ((i + EndPosition) < MAX_LENGHT_STR)
      {
        ArrStr[i] = ArrStr[i + BeginPosition]; // сдвигаем строку на количество символом указанных в переменной BeginPosition
      }
    }
    for (i = EndPosition - BeginPosition; i < MAX_LENGHT_STR; i++) // цикл до конца массива
    {
      ArrStr[i] = " "; // забъем массив пробелами
    }

    SourceStr.remove(0); // обнулим исходную строку
    for (i = 0; i <= EndPosition; i++)
    {
      SourceStr += ArrStr[i];
    }
  }
}
//--------------------------------------

//--------------------------------------
bool Screen::_LengthStrOk(String(&Item), uint16_t LengthSub, uint16_t(&QtySymbols)) // функция проверка что строка уберется
{
  QtySymbols = MAX_COLUMNS_LCD - LengthSub; // Количество символов для отображения в поле "Наименование"
  return (QtySymbols >= Item.length());     // уберется или нет строка в поле "Наименование"
}
//--------------------------------------

//--------------------------------------
bool Screen::_LengthStrOkUTF8(String(&Item), uint16_t LengthSub, uint16_t(&QtySymbols)) // функция проверка что строка уберется в UTF8
{
  QtySymbols = MAX_COLUMNS_LCD - LengthSub;    // Количество символов для отображения в поле "Наименование"
  return (QtySymbols >= _StrLengthUTF8(Item)); // уберется или нет строка в поле "Наименование"
}
//--------------------------------------

//--------------------------------------
void Screen::FormCurrentScreen4Str() // сформируем текущий экран для дисплея из 4х строк
{
  String SymbolChoiseParam = SYMBOL_CHOISE_PARAM; // объявим строку и присвоим ей символ выбора параметра, нужно для дальнейшего определения длины
  for (i = 0; i < MAX_STR_DISPLAY; i++)           // по количеству строк
  {
    {
      StrForLCD[i].remove(0); // очистим строку
      uint16_t len2 = _StrLengthUTF8(CurrentScreen.Str[i][2]);
      uint16_t len3 = _StrLengthUTF8(CurrentScreen.Str[i][3]);
      uint16_t len4 = _StrLengthUTF8(CurrentScreen.Str[i][4]);

      _TransferStrToArrStr(CurrentScreen.Str[i][0], 0);                                       // поле "выбор строки"
      _TransferStrToArrStr(CurrentScreen.Str[i][1], _StrLengthUTF8(CurrentScreen.Str[i][0])); // поле "наименование"
      _TransferStrToArrStr(CurrentScreen.Str[i][2], MAX_COLUMNS_LCD - len2 - len3 - len4);    // поле "параметр"
      _TransferStrToArrStr(CurrentScreen.Str[i][3], MAX_COLUMNS_LCD - len3 - len4);           // поле "единица измерения"
      _TransferStrToArrStr(CurrentScreen.Str[i][4], MAX_COLUMNS_LCD - len4);                  // поле "выбор параметра"

      for (j = 0; j <= MAX_COLUMNS_LCD; j++) // по количеству символов в строке наименование
      {
        ;
        StrForLCD[i] += ArrStr[j]; // переложим из массива строк в строку
      }
      StrEqual(StrForLCDOld[i], StrForLCD[i], NeedUpdateStrDisplay[i]);
    }
  }
  // Serial.print("StrForLCD = "); Serial.println(StrForLCD[1]);
}
//--------------------------------------

//--------------------------------------
void Screen::InsertStrBeginSymbolSecondStr(String(&ResultStr), String SecondStr, uint16_t SizeResultStr, uint16_t BeginPositionSecondStr)
{
  // Serial.println("------------");
  uint16_t k;
  String StrTmp = ""; // создание строки из "пробелов" количеством SizeResultStr
  StrTmp.reserve(SizeResultStr);
  for (k = 0; k < SizeResultStr; k++)
  {
    if ((BeginPositionSecondStr + k) < SecondStr.length()) // если вторая строка не закончилась
    {
      StrTmp += SecondStr[BeginPositionSecondStr + k];
    }
    else // если вторая строка закончилась
    {
      StrTmp += ' '; // заполним пробелами
    }
    // Serial.print("Str = "); Serial.println(SourceStr);
  }
  ResultStr = StrTmp; // переложим строку обратно
}
//--------------------------------------

//--------------------------------------
void Screen::InsertStr(String(&SourceStr), String(&SecondStr), uint16_t FromPosition)
{
  // Serial.println("------------");
  uint16_t k;
  if (!SecondStr.isEmpty()) // если строка не пустая
  {
    for (k = 0; k < SecondStr.length(); k++)
    {
      SourceStr[FromPosition + k] = SecondStr[k];
      // Serial.print("Str = "); Serial.println(SourceStr);
    }
  }
}
//--------------------------------------

//--------------------------------------
void Screen::StrEqual(String &OldStr, String &NewStr, bool &NeedUpdateStr) // функция сравнения двух строк
{
  if (OldStr != NewStr) // если строки отличаются
  {
    NeedUpdateStr = true; // выставим признак необходимости обновить строку

    OldStr = NewStr;
    /*   Serial.println("------------");
       Serial.print("NewStr = ");
       Serial.println(NewStr);
       Serial.print("OldStr = ");
       Serial.println(OldStr);*/
  }
  else
  {
    NeedUpdateStr = false; // выставим признак необходимости обновить строку
  }
}
//--------------------------------------

//--------------------------------------
void Screen::UpdateParam()
{
  if ((_QtyScr > 0) and (_QtyScr <= MAX_QTY_SCR))
  {
    for (i = 0; i < _QtyScr; i++)
    {
      for (j = 0; j < MAX_STR; j++)
      {
        if (Screens[i][j].WithParam) // если с параметром
        {
          if (Screens[i][j].isBoolParam) // если двоичный параметр
          {
            StrBoolParam(Screens[i][j].Str[2], Screens[i][j].BoolParam);
          }
          else
          {
            if (StrQualityParam(Screens[i][j].Str[2], Screens[i][j].QualityParam)) // если качество параметра Норма
            {
              CutStr(Screens[i][j].Param, Screens[i][j].Str[2], Screens[i][j].DeltaChangeParam); // переложим параметр в строку
            }
          }
        }
        else // если без параметра
        {
          Screens[i][j].Str[2] = ""; // очистим строку с параметром
        }
      }
    }
  }
  NeedUpdateDisplay = true;
}
//--------------------------------------

//--------------------------------------
void Screen::CutStr(float Param, String(&Str), float DeltaChangeParam) // функция обрезания лишних символов в строке Параметра
{
  String TmpStr;
  TmpStr.reserve(20);
  TmpStr = String(Param);

  if (TmpStr.length() > 10)
  {
    Str = TmpStr.substring(0, 10);
  }
  else
  {
    Str = TmpStr;
  }

  if ((DeltaChangeParam - (int)DeltaChangeParam) == 0.0) // если целое
  {
    Str.remove(Str.length() - 3); // удалим три последних символа (.00), чтобы было до целых
  }
  else
  {
    Str.remove(Str.length() - 1); // удалим последний символ (0), чтобы было до десятки
  }
}
//--------------------------------------

//--------------------------------------
void Screen::StrBoolParam(String(&Str), bool BoolParam)
{
  if (BoolParam)
  {
    Str = "Да";
  }
  else
  {
    Str = "Нет";
  }
}
//--------------------------------------

//--------------------------------------
bool Screen::StrQualityParam(String(&Str), uint8_t QualityParam)
{
  if (QualityParam == QUALITY_PARAM_HIGH_LIMIT)
  {
    Str = "! КЗ ";
    return false;
  }
  else if (QualityParam == QUALITY_PARAM_LOW_LIMIT)
  {
    Str = "! Обрыв ";
    return false;
  }
  else
  {
    return true;
  }
}
//--------------------------------------

//--------------------------------------
// Определения отключения подсветки экрана
//--------------------------------------
void Screen::_AutoLightOff(bool NeedLightOff)
{

  TonOffLight.TONTmr(NeedLightOff, TimeAutoLightOff * 1000); // таймер отключения подсветки
  if (TimeAutoLightOff != 0)
  {
    AutoLightOff = TonOffLight.Q; // бит автоматического отключения подсветки
  }
  else
  {
    AutoLightOff = false; // бит автоматического отключения подсветки
  }
}
//--------------------------------------

//--------------------------------------
// Переход на начальный экран по таймеру
//--------------------------------------
void Screen::_AutoGotoBeginScr(bool NeedGoto)
{
  // запустим таймер автоматического перехода на анчальный экран, необходимо внешнее условие и уставка таймера не "0"
  // и номер текущего жкрана и строки должны быть не равны номерам начального экрана и строки соответсвенно
  TonAutoGotoBeginScr.TONTmr(NeedGoto and (TimeAutoGotoBeginScr != 0) and ((NumCurrentScr != NOM_BEGIN_SCR) or (NumCurrentStr != NOM_BEGIN_STR)), TimeAutoGotoBeginScr * 1000); // таймер перехода на начальный экран

  if (TonAutoGotoBeginScr.Q) // если сработал таймер перехода на начальный экран
  {
    if (ChangeItem) // если изменяли параметр
    {
      ChangeItem = false;                                 // сбросим, что изменяем пункты
      Screens[NumCurrentScr][NumCurrentStr].Str[4] = " "; // стираем стрелку выбранного параметра со строки текущего экрана
    }
    Screens[NumCurrentScr][NumCurrentStr].Str[0] = " ";               // стираем стрелку со строки текущего экрана
    NumCurrentScr = NOM_BEGIN_SCR;                                    // номер текущего экрана присвоим номеру начального
    NumCurrentStr = NOM_BEGIN_STR;                                    // номер текущей строки присвоим номеру начальной
    Screens[NumCurrentScr][NumCurrentStr].Str[0] = SYMBOL_CHOISE_STR; // отобразим стрелку у строки на экране, куда перешли
    NeedUpdateDisplay = true;                                         // нужно обновить дисплей
  }
}
//--------------------------------------

//--------------------------------------
// Переход на экран по номеру экрана
//--------------------------------------
void Screen::GotoScrAndStr(uint16_t NumScr, uint16_t NumStr)
{
  if (NumScr <= MAX_QTY_SCR) // если номер экрана меньше количества экранов
  {
    if (NumStr <= Screens[NumScr][0].QtyStr) // если номер строки меньше количества строк на экране
    {
      if (ChangeItem) // если изменяли параметр
      {
        ChangeItem = false;                                 // сбросим, что изменяем пункты
        Screens[NumCurrentScr][NumCurrentStr].Str[4] = " "; // стираем стрелку выбранного параметра со строки текущего экрана
      }
      Screens[NumCurrentScr][NumCurrentStr].Str[0] = " "; // стираем стрелку со строки текущего экрана
      NumCurrentScr = NumScr;                             // номер текущего экрана присвоим номер куда будем переходить
      NumCurrentStr = NumStr;                             // номер текущей строки присвоим номер строки куда будем переходить
      if ((Screens[NumCurrentScr][NumCurrentStr].WithParam and !Screens[NumCurrentScr][NumCurrentStr].WithParamReadOnly) or Screens[NumCurrentScr][NumCurrentStr].WithGoOtherScreen)
      {
        Screens[NumCurrentScr][NumCurrentStr].Str[0] = SYMBOL_CHOISE_STR; // отобразим стрелку у строки на экране, куда перешли
      }
      NeedUpdateDisplay = true; // нужно обновить дисплей
    }
  }
}
//--------------------------------------
