/*
 * #include "GCodeParser.h"

//https://github.com/tgolla/
#pragma once
#include "GCode.h"
#include "RTClib.h"

ComandsParser::ComandsParser(){
  parser = GCodeParser();
}

void ComandsParser::tick()
{
  /*if (Serial.available() > 0)
  {
    if (parser.AddCharToLine(Serial.read()))
    {
      parser.ParseLine();
      parser.RemoveCommentSeparators();

      if (isSetTemperature())
      {
        //Serial.print("Set Temperature: ");
        //Serial.print(isSetTemperature());
      }
    }
  }**
}


// M141 [S<temp>]
int ComandsParser::isSetTemperature()
{
  /*
   * int temp = 0;
  
  if ( parser.HasWord('M') || parser.HasWord('m') )
    if ( (int)(parser.GetWordValue('M')) == 141 || (int)(parser.GetWordValue('m')) == 141 )
    {
      if (parser.HasWord('S'))
        temp = (int)(parser.GetWordValue('S'));
      else if (parser.HasWord('s'))
        temp = (int)(parser.GetWordValue('s'));
    }
  return temp;
}


// M75
/*
TimeSpan ComandsParser::isSetTimer()
{
  /*int d, h, m, s;
  
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
  
  return TimeSpan(d,h,m,s);*
  
}


/*
void dddd() {
  if (Serial.available() > 0)
  {
    if (GCode.AddCharToLine(Serial.read()))
    {
      GCode.ParseLine();
      // Code to process the line of G-Code here…

      Serial.print("Command Line: ");
      Serial.println(GCode.line);

      GCode.RemoveCommentSeparators();

      Serial.print("Comment(s): ");
      Serial.println(GCode.comments);
      
      if (GCode.HasWord('G'))
      {
        Serial.print("Process G code: ");
        Serial.println((int)GCode.GetWordValue('G'));
        Serial.println((int)GCode.GetWordValue('X'));
        Serial.println((int)GCode.GetWordValue('Y'));
      }
    }
  }
}
*/
