#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Interface
{
  public:
  Interface(Adafruit_SSD1306* d);
  ~Interface();

  void drawMainPage();
  void drawCursor();
  void drawProgram();
  void drawTime();
  void drawTemperature();
  void Show();

  void drawFirstLevel();

  int Next();
  int Prev();
  int Click();
  int Held();

  void PrintState();
  
  private:
  Adafruit_SSD1306* display;
  int cursor_pointer;

  void setStatesToNone();

  bool is_FirstLevel,
        is_MainMenu,
        is_Program,
        is_Temperature,
        is_Time;
  void clearDisplay();

};
