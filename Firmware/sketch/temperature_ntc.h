#pragma once
#include "temperature.h"

class TemperatureNTC: public Temperature
{
public:
    TemperatureNTC(int pin)
    {
        thermistor_pin = pin;
        #define B 3950 // B-коэффициент
        #define SERIAL_R 10000 // сопротивление последовательного резистора, 10 кОм
        #define THERMISTOR_R 100000 // номинальное сопротивления термистора, 100 кОм
        #define NOMINAL_T 25 // номинальная температура (при которой TR = 100 кОм)
    };
    ~TemperatureNTC();

    float getTemperature()
    {
        float tr = 1023.0 / analogRead( thermistor_pin ) - 1;
        tr = SERIAL_R / tr;

        float steinhart;
        steinhart = tr / THERMISTOR_R; // (R/Ro)
        steinhart = log(steinhart); // ln(R/Ro)
        steinhart /= B; // 1/B * ln(R/Ro)
        steinhart += 1.0 / (NOMINAL_T + 273.15); // + (1/To)
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