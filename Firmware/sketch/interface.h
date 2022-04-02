#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Interface
{
  public:
  Interface(Adafruit_SSD1306* d/*, Controller* c*/);
  ~Interface();

  float currently_temperature;
  int target_temperature;

  void drawMainPage();
  void drawCursor();
  void drawProgram();
  void drawTime();
  void drawTemperature();
  void Show();
  void update();

  void drawFirstLevel();

  int Next();
  int Prev();
  int Click();
  int Held();

  void PrintState();
  
  private:
  Adafruit_SSD1306* display;
  Controller* controller;
  int cursor_pointer;

  void setStatesToNone();

  bool is_FirstLevel,
        is_MainMenu,
        is_Program,
        is_Temperature,
        is_Time;
  void clearDisplay();

};
