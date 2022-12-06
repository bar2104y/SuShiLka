#ifndef SUSHILKA_SETTINGS_H
#define SUSHILKA_SETTINGS_H
#include <Arduino.h>

struct settings{
    bool debug;
    // char program1_name, program2_name, program3_name;
    String ssid, pass;
    
    float kp,kd,ki;

    program_settings program1, program2, program3;
};

struct program_settings{
    String title;
    byte temperature,
        time_days, time_hours, time_minutes, time_seconds; 
}

struct ntcsettings{

    int thermistor_nominal,     // Номинальное сопротивление резистора
    temperature_nominal,        // Температура, при которой зафиксировано номинальное сопротивление
    b_coeff,                    // b - коэффициент
    seriesresistor;             // сопротивление делителя
}

#endif