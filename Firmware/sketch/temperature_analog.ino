TemperatureAnalog::TemperatureAnalog(int a)
{
  this->thermistor_pin = a;
  this->thermistor_nominal = 1000;
  this->temperature_nominal = 25;
  this->b_coeff = 4300;
  this->seriesresistor = 10000;
  this->vin = 3.3;
}
TemperatureAnalog::~TemperatureAnalog(){}


float TemperatureAnalog::getTemperature()
{
  float voltage = analogRead(this->thermistor_pin) * this->vin / 1024.0;
  float r1 = voltage / (this->vin - voltage);

  return 1./( 1./(this->b_coeff)*log(r1)+1./(25. + 273.) ) - 273.;
}
