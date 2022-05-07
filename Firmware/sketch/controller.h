#pragma once
#include "RTClib.h"
#include "temperature_a.h"

#include "GyverPID.h"

extern Interface interface;
extern RTC_DS1307 rtc;

class SushilkaController
{
    public:
        SushilkaController()
        {
            TemperatureManager = new TemperatureAnalog(THERMISTOR_PIN);
            emergency = false;
            TimerON = false;
            setStatesToNone();
            is_MainMenu = true;
            temperatureToSet=0;
            stateString = "Hello, mr Yury";

            // Таймер по умолчанию
            timeToSet.d = 0;
            timeToSet.h = 0;
            timeToSet.m = 0;
            timeToSet.s = 0;

            PID = new GyverPID(10, 1.0, 1.0, 500);
            PID->setDirection(NORMAL); // направление регулирования (NORMAL/REVERSE). ПО УМОЛЧАНИЮ СТОИТ NORMAL
            PID->setLimits(0, 255);    // пределы (ставим для 8 битного ШИМ). ПО УМОЛЧАНИЮ СТОЯТ 0 И 255
            PID->setpoint = 0;

            pinMode(RELE_PIN, OUTPUT);
            digitalWrite(RELE_PIN, 0);
        }
        ~SushilkaController(){}

        // 0 - ok; 1- большая 2-маленькая
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

        void set_timer_to(int h, int m, int s)
        {
          nowTime = rtc.now();
          endTime = nowTime+TimeSpan(0,h,m,s);
          return;
        }

        void tick()
        {
            // Чтение температуры
            curTemp =int(TemperatureManager->getTemperature());
            
            // Чтение времени
            nowTime = rtc.now();

            // Обработка таймера и выдача управляющего сигнала
            if (nowTime.unixtime() >= endTime.unixtime())
                TimerON = false;
            
            if ( TimerON )
            {
                PID->setpoint = targetTemp;
                PID->input = curTemp;
                byte s = PID->getResultTimer();

                digitalWrite(RELE_PIN, s);

                Serial.print(targetTemp);
                Serial.print(" ");
                Serial.print(curTemp);
                Serial.print(" ");
                Serial.println(s);
            }
            else
            {
                PID->setpoint = 0;
                digitalWrite(RELE_PIN, 0);
            }


            // Отображение информации
            if (is_MainMenu)
                interface.drawMainPage(curTemp,targetTemp, endTime-nowTime, stateString);
            else if(is_FirsLevel)
                interface.drawFirstLevel(CursorLine);
            else if(is_Program)
                interface.drawProgram(CursorLine);
            else if(is_Temperature)
                interface.drawTemperature(temperatureToSet);
            else if (is_Time)
                interface.drawTime(CursorLine, timeToSet);
        }

        void held()
        {
            if ( is_Time )
            {
                endTime = nowTime + TimeSpan(timeToSet.d,timeToSet.h,timeToSet.m,timeToSet.s);
                TimerON = true;
                setStatesToNone();
                is_MainMenu = true;
                stateString = "Timer was setted";
                interface.drawMainPage(curTemp,targetTemp, endTime-nowTime, stateString);
            }
            else
            {
                stateString = "ABORT";
            }
        }

        void click()
        {
            if (is_MainMenu)
            {
                CursorLine = 0;
                setStatesToNone();
                is_FirsLevel = true;
                interface.drawFirstLevel(CursorLine);
            }
            else if (is_FirsLevel)
            {
                switch (CursorLine)
                {
                    case 0:
                        setStatesToNone();
                        is_MainMenu = true;
                        interface.drawMainPage(curTemp,targetTemp, endTime-nowTime, stateString);
                        break;
                    case 1:
                        CursorLine = 0;
                        setStatesToNone();
                        is_Program = true;
                        interface.drawProgram(CursorLine);
                        break;
                    case 2:
                        setStatesToNone();
                        is_Temperature = true;
                        temperatureToSet = 0;
                        interface.drawTemperature(temperatureToSet);
                        break;
                    case 3:
                        setStatesToNone();
                        is_Time = true;
                        CursorLine = 0;
                        timeToSet.d = 0; timeToSet.h = 0; timeToSet.m = 0; timeToSet.s = 0;
                        interface.drawTime(CursorLine, timeToSet);
                        break;
                    default:
                        break;
                }
            }
            else if(is_Program)
            {
                switch (CursorLine)
                {
                      case 0:
                          CursorLine = 0;
                          setStatesToNone();
                          is_FirsLevel = true;
                          interface.drawFirstLevel(CursorLine);
                          break;
                      case 1:
                          setStatesToNone();
                          is_MainMenu = true;
                          TimerON = true;
                          //PLA
                          targetTemp = 50;
                          set_timer_to(3,30,0);
                          stateString = "Start PLA";
                          interface.drawMainPage(curTemp,targetTemp, endTime-nowTime, stateString);
                          break;
                      case 2:
                          setStatesToNone();
                          is_MainMenu = true;
                          TimerON = true;
                          //PETG
                          targetTemp = 60;
                          set_timer_to(3,30,0);
                          stateString = "Start PETG";
                          interface.drawMainPage(curTemp,targetTemp, endTime-nowTime, stateString);
                          break;
                      case 3:
                          setStatesToNone();
                          is_MainMenu = true;
                          TimerON = true;
                          //ABS
                          targetTemp = 70;
                          set_timer_to(4,0,0);
                          stateString = "Start ABS";
                          interface.drawMainPage(curTemp,targetTemp, endTime-nowTime, stateString);
                          break;
                      default:
                          break;
                }
            }
            else if (is_Temperature)
            {
                targetTemp = temperatureToSet;

                setStatesToNone();
                is_Time = true;
                interface.drawTime(CursorLine, timeToSet);
                
            }
            else if (is_Time)
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

        void rotate(bool dir)
        {
            if (is_Program || is_FirsLevel)
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
            else if (is_Temperature)
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
            else if ( is_Time )
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


    private:
    void setStatesToNone()
    {
      is_MainMenu = false;
      is_FirsLevel = false;
      is_Program = false;
      is_Temperature = false;
      is_Time = false;
    }

    protected:
    bool emergency, TimerON;
    int curTemp, targetTemp;
    DateTime nowTime, endTime;
    String stateString;
    Temperature* TemperatureManager;

    //Контроль уровней меню, курсора итд
    bool is_MainMenu, is_FirsLevel, is_Program, is_Temperature, is_Time;
    int CursorLine; // Указывает выбранную строку, для выбора температуры - выбранный радряд
    int temperatureToSet;
    UserTime timeToSet;

    // Управляющий сигнал
    GyverPID* PID;
};
