/*
Copyright 2022 Yury Barichev

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/*
 * SU***KA
 * 2022
 * version 0.2.0
*/
#define VER "0.2.0"


/**************** SETTINGS *********************/
#define SERIAL_SPEED 9600 // Скорость последовательного порта

// Раскомментировать, если используется модуль реального времени
//#define USE_RTC
//#define USE_RTC_1307
//#define USE_RTC_3231

#ifdef USE_RTC
  #define NOWTIME rtc.now()
#else
  #define NOWTIME uint32_t(millis() / 1000ul)
#endif


// Энкодер с кнопкой
#define EB_BETTER_ENC       // Улучшенный алгоритм опроса энкодера. Добавит 16 байт SRAM при подключении библиотеки
#define EB_HALFSTEP_ENC     // Полушаговый энкодер - опционально

#define MIN_TEMP 0
#define MAX_TEMP 120

//#define ENABLE_DAYS_IN_TIMER // Включить выбор дней при установке таймера (раскоментировать)
#define START_MESSAGE "Hello, mr. Yury!"
#define DRAW_INFO_TO_SERIAL_MONITOR false

//Программы
#define PROGRAM_1
#define PROGRAM_1_TITLE  "PLA"  // Максимум 8 символов!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define PROGRAM_1_TEMP   50
#define PROGRAM_1_TIME_D 0
#define PROGRAM_1_TIME_H 3
#define PROGRAM_1_TIME_M 1
#define PROGRAM_1_TIME_S 1

#define PROGRAM_2
#define PROGRAM_2_TITLE  "PETG"
#define PROGRAM_2_TEMP   60
#define PROGRAM_2_TIME_D 0
#define PROGRAM_2_TIME_H 3
#define PROGRAM_2_TIME_M 30
#define PROGRAM_2_TIME_S 0

#define PROGRAM_3
#define PROGRAM_3_TITLE  "ABS"
#define PROGRAM_3_TEMP   80
#define PROGRAM_3_TIME_D 0
#define PROGRAM_3_TIME_H 4
#define PROGRAM_3_TIME_M 0
#define PROGRAM_3_TIME_S 0


#define WIFI_SELF_SSID "SUSHILKA"
#define WIFI_SELF_PASS "11223344"


/****************** PINS ***********************/
/* Ecoder:
 *  CLK | D0
 *  DT  | D5
 *  SW  | D3
 *  +   | 3.3
 *  GND | GND
 */
 #define ENCODER_A 16
 #define ENCODER_B 14
 #define ENCODER_KEY 0

 /* Display oled 128x64 and RTC I2C DS1307 module
  *  GND | GND
  *  VCC | VCC
  *  SCL | D1
  *  SDA | D2
  */
// Настройки экрана
#define SCREEN_WIDTH 128    // Ширина, пиксели
#define SCREEN_HEIGHT 64    // Высота, пиксели
#define OLED_RESET     -1   // Reset pin # (отключен: -1)
#define SCREEN_ADDRESS 0x3C // Адрес экрана (0x3C или 0x3D), смотреть DATASHEET
// 50 68


/* Analog thermistor
 *  A0
 */
#define THERMISTOR_PIN A0

/* Твердотельное реле (-)
 *  D4
 */
#define RELE_PIN 2


// Структруа, используемая для хранения установленного времени во время выбора таймера
struct UserTime{int d, h, m, s;};


/**************** LIBS ************************/
#pragma once
#include <EncButton.h>              // Для работы энкодера(крутилки)
#include "GyverPID.h"               //
//#include "PIDtuner.h"               //
#include <SPI.h>
#include <Wire.h>                   // Для работы дисплея
#include <RTClib.h>                 // Для работы модуля реального времени
#include <Adafruit_GFX.h>           // Для работы дисплея
#include <Adafruit_SSD1306.h>       // Для работы дисплея



#include "view.h"
#include "controller.h"
#include "GCodeParser.h"
#include "WIFIManager.h"

/************ GLOBAL VARIABLES *****************/
EncButton<EB_TICK, ENCODER_A, ENCODER_B, ENCODER_KEY> enc;                                      // Энкодер с кнопкой <A, B, KEY>

#ifdef USE_RTC
  #ifdef USE_RTC_1307
    RTC_DS1307 rtc;                                                                             //Модуль реального времени
  #endif
  #ifdef USE_RTC_1307
    RTC_DS3231 rtc;
  #endif
#endif


Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);    // Дисплей
Interface interface = Interface(&display);                                                      // Контроллер вывода на экран
SushilkaController controller = SushilkaController();                                           // Главный контроллер
ComandsParser parser = ComandsParser(&controller);
WIFIManager wifi = WIFIManager();


// Функция установки выполняется при запске устройства
void setup() {
  
    Serial.begin(SERIAL_SPEED); // Запуск последовательного порта

    // Остановка программы при сбое в запуске экрана
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("Couldn't find display");
        for(;;);
    }
    
    display.display();
    delay(500);

    #ifdef USE_RTC
      // Запуск модуля реального времени, в случае незапуска - прекращение работы
      if (! rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
      }
      if (! rtc.isrunning())
        Serial.println("RTC is NOT running!");
      
      // Установка текущей даты и времени при прошивке
      rtc.adjust(DateTime(__DATE__, __TIME__));
    #endif
}


// Цикл программы - аналог for (;;) - бесконечного цикла
void loop() {
    static uint32_t tmr;                          // Переменная для хранения времени
    //static uint32_t tmr2;
  
    enc.tick();                                   // Проверка состояния энкодера
    if (enc.click()) controller.click();          // Событие при клике
    if (enc.left())  controller.rotate(true);     // Событие при повороте налево
    if (enc.right()) controller.rotate(false);    // Событие при повороте направо
    if (enc.held())  controller.held();           // Событие при удержании

    parser.tick();

    // Запуск действий по таймеру
    if ( millis() - tmr >= 50) {
        tmr = millis();
        controller.tick();
    }

}
