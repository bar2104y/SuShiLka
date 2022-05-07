/*
 * SU***KA
 * 2022
 * version 0.1.0
*/

/**************** SETTINGS *********************/
#define SERIAL_SPEED 9600 // Скорость последовательного порта

// Экран 
#define SCREEN_WIDTH 128    // Ширина, пиксели
#define SCREEN_HEIGHT 64    // Высота, пиксели
#define OLED_RESET     -1   // Reset pin # (отключен: -1)
#define SCREEN_ADDRESS 0x3C // Адрес экрана (0x3C или 0x3D), смотреть DATASHEET
// 50 68

#define EB_BETTER_ENC       // Улучшенный алгоритм опроса энкодера. Добавит 16 байт SRAM при подключении библиотеки
#define EB_HALFSTEP_ENC     // Полушаговый энкодер - опционально

#define MIN_TEMP 0
#define MAX_TEMP 120

//#define ENABLE_DAYS_IN_TIMER // Включить выбор дней при установке таймера



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

 /* Display oled 128x64
  *  GND | GND
  *  VCC | VCC
  *  SCL | D1
  *  SDA | D2
  */

/* Analog thermistor
 *  A0
 */
#define THERMISTOR_PIN A0

/* Твердотельное реле (-)
 *  D4
 */
#define RELE_PIN 2

struct UserTime
{
  int d;
  int h;
  int m;
  int s;
};

/**************** LIBS ************************/

#include <EncButton.h>
#include "GyverPID.h"
#include "PIDtuner.h"
#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#include "view.h"
#include "controller.h"





/************ GLOBAL VARIABLES *****************/
EncButton<EB_TICK, ENCODER_A, ENCODER_B, ENCODER_KEY> enc;  // Энкодер с кнопкой <A, B, KEY>
RTC_DS1307 rtc;

Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Interface interface = Interface(&display);
SushilkaController controller = SushilkaController();




// Функция установки
void setup() {
    Serial.begin(SERIAL_SPEED); // Запуск последовательного порта
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("err"));
        for(;;);
    }
    display.display();
    delay(500);
    // Запуск модуля реального времени 
    if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      while (1);
    }

    // Если модуль не найден
    if (! rtc.isrunning()) {
      Serial.println("RTC is NOT running!");
    }
    // Установка текущей даты и времени
    rtc.adjust(DateTime(__DATE__, __TIME__));

    //Serial.println("200");

    controller.set_timer_to(0,2,0);
}

void loop() {
  static uint32_t tmr;
    /* События
    * *поворот налево
    * *поворот направо
    * *клик
    * *ужержание
    */
  enc.tick();                       // Проверка состояния энкодера
  if (enc.click())
    controller.click();
  if (enc.left()) controller.rotate(true);     // Поворот налево
  if (enc.right()) controller.rotate(false);
  if (enc.held()) controller.held();

  if ( millis() - tmr >= 100) {
    tmr = millis();
    controller.tick();
  }
  
}
