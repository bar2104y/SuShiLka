#ifndef ComandsParser_h
#define ComandsParser_h

//https://github.com/tgolla/
//https://github.com/tgolla/
#pragma once
#include "GCode.h"
#include "RTClib.h"
#include <Arduino.h>

class ComandsParser
{
public:
  ComandsParser(SushilkaController* contrl)
  {
    controller = contrl;
    parser = GCodeParser();
  };
  ~ComandsParser(){};

  void tick()
  {
    if (Serial.available() > 0)
    {
      if (parser.AddCharToLine(Serial.read()))
      {
        parser.ParseLine();
        parser.RemoveCommentSeparators();

        int temperature = isSetTemperature();
        int* timer = isSetTimer();
        if (temperature)
        {
          
          Serial.print("Set Temperature: ");
          Serial.println(temperature);
          controller->set_targetTemp(temperature);
        }
        else if (!NULL)
        {
          Serial.println("Установить время");
          controller->set_timer_to(timer[0], timer[1], timer[2], timer[3]);
        }
      }
      
    }
  };

  // Возвращает требуемую температуру
  int isSetTemperature(){
    int temp = 0;
  
    if ( parser.HasWord('M') || parser.HasWord('m') )
      if ( (int)(parser.GetWordValue('M')) == 141 || (int)(parser.GetWordValue('m')) == 141 )
      {
        if (parser.HasWord('S'))
          temp = (int)(parser.GetWordValue('S'));
        else if (parser.HasWord('s'))
          temp = (int)(parser.GetWordValue('s'));
      }
      return temp;
    return false;
  };

  // M75
  // Возвращает объект времени
  int* isSetTimer(){
    int d, h, m, s;
    int res[4];
  
    if ( parser.HasWord('M') || parser.HasWord('m') )
      if ( (int)(parser.GetWordValue('M')) == 75 || (int)(parser.GetWordValue('m')) == 75 )
      {
        // День
        if (parser.HasWord('D'))
          d = (int)(parser.GetWordValue('D'));
        else if (parser.HasWord('d'))
          d = (int)(parser.GetWordValue('d'));
  
        // Час
        if (parser.HasWord('H'))
          d = (int)(parser.GetWordValue('H'));
        else if (parser.HasWord('h'))
          d = (int)(parser.GetWordValue('h'));

        // Минуты
        if (parser.HasWord('M'))
          d = (int)(parser.GetWordValue('M'));
        else if (parser.HasWord('m'))
          d = (int)(parser.GetWordValue('m'));

        // Секунды
        if (parser.HasWord('S'))
          d = (int)(parser.GetWordValue('S'));
        else if (parser.HasWord('s'))
          d = (int)(parser.GetWordValue('s'));
      }

      res[0] = d;
      res[1] = h;
      res[2] = m;
      res[3] = s;
    
      return res;
    
    return NULL;
  };
private:
  GCodeParser parser;
  SushilkaController* controller;
};

#endif
