#pragma once
#include "temperature.h"


class TemperatureAnalog: public Temperature
{
public:
    TemperatureAnalog(int pin)
    {
        thermistor_pin = pin;
        thermistor_nominal = 1000;
        temperature_nominal = 25;
        b_coeff = 4300;
        seriesresistor = 10000;
        vin = 3.3;
    };
    ~TemperatureAnalog();

    float getTemperature()
    {
        float voltage = analogRead(thermistor_pin) * vin / 1024.0;
        float r1 = voltage / (vin - voltage);

        return 1./( 1./(b_coeff)*log(r1)+1./(25. + 273.) ) - 273.;
    };
private:
    int thermistor_pin,
    thermistor_nominal,
    temperature_nominal,
    b_coeff,
    seriesresistor;
    float vin;

};
