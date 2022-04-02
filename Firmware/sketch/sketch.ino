/*
 * SU***KA
 * 2022
 * version 0.0.2
*/


/**************** LIBS ************************/
#include "controller.h"
#include "temperature.h"
#include "temperature_analog.h"
#include "interface.h"

#include <EncButton.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


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

/**************** SETTINGS *********************/
#define SERIAL_SPEED 9600 // Скорость последовательного порта

// Экран 
#define SCREEN_WIDTH 128    // Ширина, пиксели
#define SCREEN_HEIGHT 64    // Высота, пиксели
#define OLED_RESET     -1   // Reset pin # (отключен: -1)
#define SCREEN_ADDRESS 0x3C // Адрес экрана (0x3C или 0x3D), смотреть DATASHEET

#define EB_BETTER_ENC       // Улучшенный алгоритм опроса энкодера. Добавит 16 байт SRAM при подключении библиотеки
#define EB_HALFSTEP_ENC     // Полушаговый энкодер - опционально


/************ GLOBAL VARIABLES *****************/
EncButton<EB_TICK, ENCODER_A, ENCODER_B, ENCODER_KEY> enc;  // Энкодер с кнопкой <A, B, KEY>
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Interface* menu;
Controller* releController;
TemperatureAnalog* temperatureController;



// Функция установки
void setup() {
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, 0);
  
  Serial.begin(SERIAL_SPEED); // Запуск последовательного порта

  // Если дисплей не запущен - программа не запускается (уходит в бесконечный цикл)
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Ошибка инициализации дисплея"));
    for(;;);
  }

  // Таймаут загрузки экрана
  display.display();
  delay(2000); // Pause for 2 seconds

  // Отчистка экрана
  display.clearDisplay();
  display.display();

  // Создание экземпляра контроллера экрана
  menu = new Interface(&display);
  temperatureController = new TemperatureAnalog(THERMISTOR_PIN);
  releController = new Controller();
  menu->drawMainPage();   // Отрисовка главной страницы
}

void loop() {
  enc.tick();                       // Проверка состояния энкодера

  if (enc.left()) menu->Next();     // Поворот налево
  if (enc.right()) menu->Prev();    // Поворот направо
  if (enc.click()) menu->Click();   // Однократное нажание
  if (enc.held()) menu->Held();     // Удержание

   

  // конструкция программного таймера на 800 мс
  static uint32_t tmr;

  // Уменьшаем частоту проведения измеренийй и изменения управляющего сигнала
  if (millis() - tmr >= 500) {
    tmr = millis();

    // Получаем текущую температуру
    float temperature = temperatureController->getTemperature();
    releController->setInput(temperature); // Устанавливаем текущую температуру
    menu->update(); // обновляем информацию на экране
    

    // Вывод данных в консоль (для плоттера)
    Serial.print("currently_temp ");
    Serial.print(releController->getInput());
    Serial.print(" target_type ");
    Serial.print(releController->getTarget());
    Serial.print(" signal ");
    Serial.println(releController->getSignal());
    
    // Устанавливаем управляющее воздействие
    digitalWrite(RELE_PIN, releController->getSignal());
  }
}
