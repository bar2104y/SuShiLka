/*
Copyright 2022 Yury Barichev

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once
#include "RTClib.h"
#include "temperature_a.h"

#include "GyverPID.h"
#include "PIDtuner.h"

// Подключение глобальных переменных
extern Interface interface;
extern RTC_DS1307 rtc;


class SushilkaController
{
    public:
        // Конструктор - инициализация параметров
        SushilkaController()
        {
            stateString = START_MESSAGE;                                // Строка приветствия на главном экране
            TemperatureManager = new TemperatureAnalog(THERMISTOR_PIN); // инициализация интерфейса для получения температуры
            emergency = false;                                          // Сигнал об аварийной остановке
            TimerON = false;                                            // Флаг включения таймера (нагрева)
            setStatesToNone();                                          // Сброс информации о текущей странице интерфейса
            is_MainMenu = true;                                         // Установка главной страницы интерфеса к выводу4
            
            temperatureToSet=0;                                         // Переменная для хранения выбранной температуры до ее установки
            
            timeToSet.d = 0;                                            // Переменная для хранения выбранного
            timeToSet.h = 0;                                            // времени таймера до его установки
            timeToSet.m = 0;
            timeToSet.s = 0;

            // Инициализация ПИД-регулятора
            KP = 10.0;
            KI = 1.0;
            KD = 1.0;
            PID = new GyverPID(100, 0, 0, 500);
            PID->setDirection(NORMAL); // направление регулирования (NORMAL/REVERSE). ПО УМОЛЧАНИЮ СТОИТ NORMAL
            PID->setLimits(0, 255);    // пределы (ставим для 8 битного ШИМ). ПО УМОЛЧАНИЮ СТОЯТ 0 И 255
            PID->setpoint = 0;

            // PID_tuner
            PID_tuner = new PIDtuner();
            PID_tuner->setParameters(NORMAL, 245, 10, 5000, 2 ,5000, 100);
            TunerON = false;

            // Инициализация управляюшего вывода и его выключения
            pinMode(RELE_PIN, OUTPUT);
            digitalWrite(RELE_PIN, 0);
        }
        ~SushilkaController(){} // Деструктор - на всякий пожарный

        // 0 - ok; 1- большая 2-маленькая
        /*
        int set_curTemp(int t)
        {
            curTemp = t;

            if (curTemp < MIN_TEMP) return 2;
            if (curTemp > MAX_TEMP) return 1;
            
            return 0;
        }

        int set_targetTemp(int t)
        {
            if (targetTemp < MIN_TEMP) return 2;
            if (targetTemp > MAX_TEMP) return 1;
            targetTemp = t;
            return 0;
        }
        */

        // Установка таймера на h часов, m минут и s секунд
        int set_timer_to(int d, int h, int m, int s, bool go_to_main = true )
        {
            //Если таймер требуется на отрицательное время - выход с ошибкой
            if ( h < 0 || m < 1 || s < 0 || d < 0 )
                return 1;
            nowTime = rtc.now();                  // Обновление текущего времени
            endTime = nowTime+TimeSpan(d,h,m,s);  // Вычисление конечного времени
            TimerON = true;                       // Включение таймера

            // Логика выхода в главное меню (по требованию)
            if (go_to_main)
            {
                setStatesToNone();
                is_MainMenu = true;
                stateString = "Timer was setted";
            }
            return 0;
        }


        // Один тик работы контроллера
        void tick()
        {
            static byte s;
            curTemp =int(TemperatureManager->getTemperature());   // Чтение температуры

            if (TunerON)
            {
              PID_tuner->setInput(curTemp);
              PID_tuner->compute();
              digitalWrite(RELE_PIN, PID_tuner->getOutput());

              PID_tuner->debugPlot();
              //PID_tuner->debugText();
            }
            
            nowTime = rtc.now();                                  // Чтение времени

            if (nowTime.unixtime() >= endTime.unixtime())         // Обработка таймера и выдача управляющего сигнала
                TimerON = false;

            if ( TimerON ) // Если включен таймер (нагрев)
            {
                PID->setpoint = targetTemp;                       // Установка целевой температуры
                PID->input = curTemp;                             // Установка текущей температуры
                s = PID->getResultTimer();                        // Получение управляющего сигнала
                digitalWrite(RELE_PIN, s);                        // Вывод управляющего сигнала на реле
            }
            else // Если нагрев выключен
            {
                endTime = nowTime;
                targetTemp = MIN_TEMP;
                PID->setpoint = MIN_TEMP;                         // Установка целевой температуры в  минимальное значение
                digitalWrite(RELE_PIN, 0);                        // Выключение реле
            }

            if (DRAW_INFO_TO_SERIAL_MONITOR)                      // Если включен вывод графиков - отправка данных в порт
            {
                Serial.print(targetTemp);
                Serial.print(" ");
                Serial.print(curTemp);
                Serial.print(" ");
                Serial.println(s);
            }

            // Отображение информации на основе установленного состояния
            if (is_MainMenu)
                interface.drawMainPage(curTemp,targetTemp, endTime-nowTime, stateString);
            else if (is_MainMenu2)
                interface.drawMainMenu2(nowTime);
            else if(is_PIDPage)
                interface.drawPIDPage(KP,KI,KD, TunerON, PID_tuner->getAccuracy());
            else if(is_FirstLevel)
                interface.drawFirstLevel(CursorLine);
            else if(is_FirstLevel2)
                interface.drawFirstLevel2(CursorLine);
            else if(is_Program)
                interface.drawProgram(CursorLine);
            else if(is_Temperature)
                interface.drawTemperature(temperatureToSet);
            else if (is_Time)
                interface.drawTime(CursorLine, timeToSet);
        }

        // Обработка удержания
        void held()
        {
            if ( is_Time )
                set_timer_to(timeToSet.d,timeToSet.h,timeToSet.m,timeToSet.s, true); // Включение таймера
            else if (is_FirstLevel2)
                openMainMenu2();
            else
                EMERGENCY();
        }

        // 
        void click()
        {
            if (is_MainMenu)                             // Клик в главном меню - переход на первый уровень меню
                openFirstLevel();
            else if (is_MainMenu2)
                openFirstLevel2();
            else if (is_PIDPage)
            {
              TunerON = !TunerON;
              openMainMenu(); 
            }
            else if (is_FirstLevel)                       // Клик в первом уровне меню
                switch (CursorLine)
                {
                    case 0: openMainMenu();     break;    // Открыть главную страницу
                    case 1: openProgram();      break;    // Открыть выбор программ
                    case 2: openTemperature();  break;    // Открыть настройку тмператур
                    case 3: openTime();         break;    // Открыть настройку таймера
                    default: break;
                }
            else if (is_FirstLevel2)
                switch(CursorLine)
                {
                    case 0: openMainMenu2();    break;
                    case 1: openPIDPage();      break;
                }
            else if(is_Program) // Клик в выборе программ
                switch (CursorLine)
                {
                    case 0: openFirstLevel();   break;    // Подняться вверх - первый уровень меню

                    // Если определена программа 1
                    #ifdef PROGRAM_1
                    case 1:
                        //PROGRAM 1
                        targetTemp = PROGRAM_1_TEMP;
                        set_timer_to(PROGRAM_1_TIME_D, PROGRAM_1_TIME_H, PROGRAM_1_TIME_M, PROGRAM_1_TIME_S);
                        stateString = PROGRAM_1_TITLE;
                        openMainMenu();
                        break;
                    #endif
                    
                    // Если определена программа 2
                    #ifdef PROGRAM_2
                    case 2:
                        //PROGRAM 2
                        targetTemp = PROGRAM_2_TEMP;
                        set_timer_to(PROGRAM_2_TIME_D, PROGRAM_2_TIME_H, PROGRAM_2_TIME_M, PROGRAM_2_TIME_S);
                        stateString = PROGRAM_2_TITLE;
                        openMainMenu();
                        break;
                    #endif

                    // Если определена программа 3
                    #ifdef PROGRAM_3
                    case 3:
                        //PROGRAM 3
                        targetTemp = PROGRAM_3_TEMP;
                        set_timer_to(PROGRAM_3_TIME_D, PROGRAM_3_TIME_H, PROGRAM_3_TIME_M, PROGRAM_3_TIME_S);
                        stateString = PROGRAM_3_TITLE;
                        openMainMenu();
                        break;
                    #endif
                    default:
                        break;
            }
            else if (is_Temperature)                            // Клик в установке температуры
            {
                targetTemp = temperatureToSet;
                openTime();                
            }
            else if (is_Time)                                   // Клик в установке таймера
            {
                #ifdef ENABLE_DAYS_IN_TIMER
                    #define LINES_IN_TIMER 3
                #else
                    #define LINES_IN_TIMER 2
                #endif
                CursorLine++;
                if (CursorLine > LINES_IN_TIMER) CursorLine = 0;
            }
        }

        // Обработка поворотов энкодера
        void rotate(bool dir)
        {
            
            if (is_MainMenu)
                openMainMenu2();
            else  if (is_MainMenu2)
                openMainMenu();
            else
            
            if (is_Program || is_FirstLevel || is_FirstLevel2 )  // Обработка на страницах с 4 пунктами меню
                if (dir)
                {
                    CursorLine++;
                    if (CursorLine > 3) CursorLine=3;
                }
                else
                {
                    CursorLine--;
                    if (CursorLine < 0) CursorLine=0;
                }
            else if (is_Temperature)                                // Обработка на странице установки температуры
                if (dir)
                {
                    temperatureToSet+=5;
                    if (temperatureToSet > MAX_TEMP)
                        temperatureToSet = MAX_TEMP;
                }
                else
                {
                    temperatureToSet-=5;
                    if ( temperatureToSet < MIN_TEMP)
                        temperatureToSet = MIN_TEMP;
                }
            else if ( is_Time )                             // Обработка на странице установки времени
            {
                int multiplicator = -1;
                if (dir) multiplicator = 1;

                // Включение и выключение установки дней для таймера (оно же ограничение в 24 часа)
                #ifdef ENABLE_DAYS_IN_TIMER
                switch (CursorLine)
                {
                    // !!!!!!!ВНИМАНИЕ !!!!!!!!! БЫДЛОКОД!!!!!!!!
                    case 0:
                        timeToSet.d += multiplicator;
                        if ( timeToSet.d < 0) timeToSet.d = 0;
                        break;
                    case 1:
                        timeToSet.h += multiplicator;
                        if ( timeToSet.h < 0) timeToSet.h = 0;
                        else if ( timeToSet.h > 23 )
                        {
                            timeToSet.h = 0;
                            timeToSet.d++;
                        }
                        break;
                    case 2:
                        timeToSet.m += multiplicator;
                        if ( timeToSet.m < 0) timeToSet.m = 0;
                        else if ( timeToSet.m > 59 )
                        {
                            timeToSet.m = 0;
                            timeToSet.h++;
                            if (timeToSet.h > 23) {timeToSet.h = 0; timeToSet.d++;}
                        }
                        break;
                    case 3:
                        timeToSet.s += multiplicator;
                        if ( timeToSet.s < 0) timeToSet.s = 0;
                        else if ( timeToSet.s > 59 )
                        {
                            timeToSet.s = 0;
                            timeToSet.m++;
                            if (timeToSet.m > 59) {timeToSet.m = 0; timeToSet.h++;}
                            if (timeToSet.h > 23) {timeToSet.h = 0; timeToSet.d++;}
                        }
                        break;
                    default: break;
                }
                #else
                switch (CursorLine)
                {
                    // !!!!!!!ВНИМАНИЕ !!!!!!!!! БЫДЛОКОД!!!!!!!!
                    case 0:
                        timeToSet.h += multiplicator;
                        if ( timeToSet.h < 0) timeToSet.h = 0;
                        else if ( timeToSet.h > 24 )
                            timeToSet.h = 24;
                        break;
                    case 1:
                        timeToSet.m += multiplicator;
                        if ( timeToSet.m < 0) timeToSet.m = 0;
                        else if ( timeToSet.m > 59 )
                        {
                            timeToSet.m = 0; timeToSet.h++;
                            if (timeToSet.h > 24) {timeToSet.h = 24;}
                        }
                        break;
                    case 2:
                        timeToSet.s += multiplicator;
                        if ( timeToSet.s < 0) timeToSet.s = 0;
                        else if ( timeToSet.s > 59 )
                        {
                            timeToSet.s = 0; timeToSet.m++;
                            if (timeToSet.m > 59) {timeToSet.m = 0; timeToSet.h++;}
                            if (timeToSet.h > 24) {timeToSet.h = 24;}
                        }
                        break;
                    default: break;
                }
                #endif
            }
        }

    /*
    void debug()
    {
    }
    */

    private:
    void setStatesToNone()
    {
      is_MainMenu = false;
      is_FirstLevel = false;
      is_Program = false;
      is_Temperature = false;
      is_Time = false;
      is_PIDPage = false;
      is_FirstLevel2 = false;
      is_MainMenu2 = false;
    }

    void openMainMenu()
    {
        setStatesToNone();
        is_MainMenu = true;
    }

    void openMainMenu2()
    {
        setStatesToNone();
        is_MainMenu2 = true;
    }

    void openPIDPage()
    {
        setStatesToNone();
        is_PIDPage = true;
    }

    void openFirstLevel()
    {
        CursorLine = 0;
        setStatesToNone();
        is_FirstLevel = true;
    }

    void openFirstLevel2()
    {
        CursorLine = 0;
        setStatesToNone();
        is_FirstLevel2 = true;
    }

    void openProgram()
    {
        CursorLine = 0;
        setStatesToNone();
        is_Program = true;
    }

    void openTemperature()
    {
        setStatesToNone();
        is_Temperature = true;
        temperatureToSet = 0;
    }

    void openTime()
    {
        setStatesToNone();
        is_Time = true;
        CursorLine = 0;
        timeToSet.d = 0; timeToSet.h = 0; timeToSet.m = 0; timeToSet.s = 0;
    }
    
    void EMERGENCY()
    {
      digitalWrite(RELE_PIN, 0);
      
      targetTemp = MIN_TEMP;
      PID->setpoint = MIN_TEMP;
      endTime = nowTime;
      TimerON = false;
      emergency = true;
      stateString = "!!!ERROR!!!STOP!!!";
    }

    
    
    protected:
    bool emergency, TimerON, TunerON;
    int curTemp, targetTemp;
    //byte s; // Управляющий сиграл
    DateTime nowTime, endTime;
    String stateString;
    Temperature* TemperatureManager;

    
    bool is_MainMenu, is_FirstLevel, is_Program, is_Temperature, is_Time,
         is_PIDPage, is_MainMenu2, is_FirstLevel2;                         //Контроль состояния уровней меню
    int CursorLine;                                                       // Указывает выбранную строку, для выбора температуры - выбранный радряд
    int temperatureToSet;
    UserTime timeToSet;


    float KP,KI,KD;
    GyverPID* PID;                                                        // PID-регулятор
    PIDtuner* PID_tuner;                                                  // Автоматическая настройка PID
};
