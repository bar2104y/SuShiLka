#ifndef TEMPERATURE_NTC_H
#define TEMPERATURE_NTC_H


#pragma once
#include "temperature.h"

class TemperatureNTC: public Temperature
{
public:
    TemperatureNTC(int pin)
    {
        thermistor_pin = pin;
        b_coeff = 3950;              // B-коэффициент
        seriesresistor = 10000;      // сопротивление последовательного резистора, 10 кОм
        thermistor_nominal = 100000; // номинальное сопротивления термистора, 100 кОм
        temperature_nominal = 25;    // номинальная температура (при которой TR = 100 кОм)
    };
    ~TemperatureNTC();

    float getTemperature()
    {
        float tr = 1023.0 / analogRead( thermistor_pin ) - 1;
        tr = seriesresistor / tr;

        float steinhart;
        steinhart = tr / thermistor_nominal; // (R/Ro)
        steinhart = log(steinhart); // ln(R/Ro)
        steinhart /= b_coeff; // 1/B * ln(R/Ro)
        steinhart += 1.0 / (temperature_nominal + 273.15); // + (1/To)
        steinhart = 1.0 / steinhart; // Invert
        steinhart -= 273.15;

        return (steinhart);
    };
    
private:
    int thermistor_pin,
    thermistor_nominal,
    temperature_nominal,
    b_coeff,
    seriesresistor;
    float vin;

};

#endif
