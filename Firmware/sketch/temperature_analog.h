class TemperatureAnalog: public Temperature
{
public:
  TemperatureAnalog(int a);
  ~TemperatureAnalog();

  virtual float getTemperature();
private:
  int thermistor_pin,
      thermistor_nominal,
      temperature_nominal,
      b_coeff,
      seriesresistor;
  float vin;
  
};
