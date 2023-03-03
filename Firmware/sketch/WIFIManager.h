#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include "settings.h"



class WIFIManager
{
private:
    int current_wifimode;
public:
    WIFIManager()
    {
        current_wifimode = WIFI_AP;
        enableAP();
    };
    WIFIManager(bool mode)
    {
    };
    ~WIFIManager();

protected:
    int enableAP()
    {
        WiFi.mode(WIFI_AP);
        WiFi.begin(WIFI_SELF_SSID, WIFI_SELF_PASS);

        byte i = 0, dtk = 100;

        while ( (WiFi.status() != WL_CONNECTED) && (i < 20 * 1000 / dtk) )
            delay(100);
            i++;
        
        if (WiFi.status() == WL_CONNECTED)
            return 0;
        else
            return -1;

        Serial.println(WiFi.localIP());
        
    };
};




#endif