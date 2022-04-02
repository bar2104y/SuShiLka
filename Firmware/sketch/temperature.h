/*
* Родительский класс - обертка для удобной работы с различными датчиками температуры
* 
*/
class Temperature
{
  public:
  Temperature();
  ~Temperature();

  virtual float getTemperature();
  
};
