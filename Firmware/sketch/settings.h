#ifndef SUSHILKA_SETTINGS_H
#define SUSHILKA_SETTINGS_H
#include <Arduino.h>


struct program_settings{
    String title;
    byte temperature,
        time_days, time_hours, time_minutes, time_seconds; 
};

struct ntcsettings{

    int thermistor_nominal,     // Номинальное сопротивление резистора
    temperature_nominal,        // Температура, при которой зафиксировано номинальное сопротивление
    b_coeff,                    // b - коэффициент
    seriesresistor;             // сопротивление делителя
};

struct wifisettings{
    String ssid, pass;
};

struct settings_s{
    bool debug;
    // char program1_name, program2_name, program3_name;
    wifisettings wifi;
    
    float kp,kd,ki;

    program_settings program1, program2, program3;
};
#endif