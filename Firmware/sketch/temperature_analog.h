class TemperatureAnalog: public Temperature
{
  public:
  TemperatureAnalog(int a);
  ~TemperatureAnalog();

  float getTemperature();
  private:
  int thermistor_pin,
      thermistor_nominal,
      temperature_nominal,
      b_coeff = 4300,
      seriesresistor;
  float vin;
  
};
