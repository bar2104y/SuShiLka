#ifndef TEMPERATURE_H
#define TEMPERATURE_H

/*
* Базовый класс - обертка для удобной работы с различными датчиками температуры
*/
class Temperature
{
  public:
  Temperature(){};
  ~Temperature(){};

  virtual float getTemperature()=0;
};
#endif