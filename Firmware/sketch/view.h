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

    // Первый(основной) уровень меню
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
        //display->setCursor(0,0);
        //display->fillRect(0, 0, 24, 64, SSD1306_BLACK);
      
        // Установка положения курсора, его отрисовка и отображение на дисплее
        display->setCursor(0, 16*selected_line);
        display->print(">");
        display->display();
    }

    // Меню выбора программ
    void drawProgram(int selected_line)
    {
        clearDisplay();
        display->print("  UP\n"
                "  PLA\n"
                "  PETG\n"
                "  ABS\n"
        );
        display->setCursor(0, 16*selected_line);
        display->print(">");
        display->display();
    }

    void drawTemperature(int t_temp)
    {
        clearDisplay();
        display->println("Set temp:\n");
        
        display->print("     ");
        display->print(t_temp);
        display->println(" C");

        display->display();
    }

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
