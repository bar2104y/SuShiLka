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
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"


class Interface
{
public:
    Interface(Adafruit_SSD1306* d)
    {
      display = d;
    };
    ~Interface(){};

    //Вывод главной страницы (температуры, время, состояние)
    void drawMainPage(int curT, int tT, TimeSpan dt, String text="")
    {
        if(dt.totalseconds()<0)
          dt = TimeSpan(0,0,0,0);
        clearDisplay();
        char buffer[10];
        sprintf(buffer, "%03d/%03d C", curT, tT);
        
        display->println(buffer);
        sprintf(buffer, "%02d:%02d:%02d", int(dt.hours()), int(dt.minutes()), int(dt.seconds()));
        display->println(buffer);
        display->setTextSize(1);
        display->println(text);

        display->display();
    }

    // Первый(основной) уровень меню (страница задания параметров)
    void drawFirstLevel(int selected_line)
    {
        // Очистка дисплея, установка размера шрифта, цвета
        clearDisplay();
        
        // Вывод списка меню
        display->print("  UP\n"
                "  Program\n"
                "  Temp\n"
                "  Time\n"
        );
      
        // Установка положения курсора, его отрисовка и отображение на дисплее
        display->setCursor(0, 16*selected_line);
        display->print(">");
        display->display();
    }

    // Меню выбора программ
    void drawProgram(int selected_line)
    {
        clearDisplay();
        display->print("  UP\n  ");
        display->print(PROGRAM_1_TITLE);
        display->print("\n  ");
        display->print(PROGRAM_2_TITLE);
        display->print("\n  ");
        display->print(PROGRAM_3_TITLE);
        
        display->setCursor(0, 16*selected_line);
        display->print(">");
        display->display();
    }

    // Меню настройки температуры
    void drawTemperature(int t_temp)
    {
        clearDisplay();
        display->println("Set temp:\n");
        
        display->print("    ");
        display->print(t_temp);
        display->println(" C");

        display->display();
    }

    // Меню настройки  таймера
    void drawTime(int line, UserTime time)
    {
        clearDisplay();
        display->println("Set timer:\n");

        char buffer[10];
        
        
        #ifdef ENABLE_DAYS_IN_TIMER
            sprintf(buffer, "%02d:", time.d);
            display->print(buffer);
        #endif
        sprintf(buffer, "%02d:%02d:%02d", time.h, time.m, time.s);
        display->println(buffer);

        switch (line)
        {
            case 0: display->println("__"); break;
            case 1: display->println("   __"); break;
            case 2: display->println("      __"); break;
            case 3: display->println("         __"); break;
        }

        display->display();
    }

    // Вторая страница главного  меню
    void drawMainMenu2(DateTime time)
    {
        clearDisplay();
        display->print("SETTINGS\n");

        char buffer[20];
        // ВРЕМЯ ДОБАВТЬ!!!
        sprintf(buffer, "%02d.%02d.%02d\n  %02d:%02d:%02d", time.day(),time.month(),time.year(), time.hour(), time.minute(),time.second());

        display->print(buffer);
        display->display();
    }

    // Первый(основной) уровень меню (страница настроек)
    void drawFirstLevel2(int selected_line)
    {
        clearDisplay();
        
        display->print("  UP\n"
                "  PID\n"
                "  ###\n"
                "  ###\n"
        );

        display->setCursor(0, 16*selected_line);
        display->print(">");
        display->display();
    }

    // Страница настройки параметров PID
    void drawPIDPage(float kp, float ki, float kd, bool flag=false, int accurancy=0)
    {
        clearDisplay();
        if (flag)
        {
            display->print("Tune in progress:\n");
            display->print(accurancy);
            display->print("%\n Click to stop");
        }
        else
        {
            //display->println("PID:");
            
            display->print(" KP: ");
            display->println(kp);
            display->print(" KI: ");
            display->println(ki);
            display->print(" KD: ");
            display->println(kd);
            display->print("Click to tune");
        }
        display->display();
    }

private:
    Adafruit_SSD1306* display;

    void clearDisplay()
    {
        display->clearDisplay();
        display->setCursor(0,0);
        display->setTextColor(SSD1306_WHITE);
        display->setTextSize(2);
    }
};
