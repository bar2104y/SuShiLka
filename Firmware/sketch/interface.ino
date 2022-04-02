Interface::Interface(Adafruit_SSD1306* d){
    this->display = d;
    this->cursor_pointer = 0;

    this->setStatesToNone();
    this->is_MainMenu = true;
};
  
Interface::~Interface(){ free(this->display); };


/*--------------------------------------------------------------
--------------------------СТРАНИЦЫ МЕНЮ-------------------------
--------------------------------------------------------------*/

// Отрисовка главной страницы меню (с общей информацией)
void Interface::drawMainPage()
{
  // Очистка дисплея
  this->clearDisplay();

  this->display->print("\n  ");
  this->display->print(int(releController->getInput()));
  this->display->print("/");
  this->display->print(releController->getTarget());
  this->display->print("\n left 5min ");


  this->Show();

  if (!this->is_MainMenu)
  {
    this->setStatesToNone();
    this->is_MainMenu = true;
    this->cursor_pointer = 0;
  }
  
}

// Отрисовка первого уровня меню
void Interface::drawFirstLevel()
{
  // Очистка дисплея, установка размера шрифта, цвета
  this->clearDisplay();
  // Вывод списка меню
  this->display->print("  UP\n"
          "  Program\n"
          "  Temp\n"
          "  Time\n"
  );
  
  
  // Установка состояния (текущего пункта меню)
  if (!this->is_FirstLevel)
  {
    this->setStatesToNone();
    this->is_FirstLevel = true;
    // Отрисовка курсора
    this->cursor_pointer = 0;
    this->drawCursor();
    // Отображение информации производится в drawCursor() неявным образом
  }
}

// Выбор программы
void Interface::drawProgram()
{
  this->clearDisplay();
  this->display->print("  UP\n"
          "  PLA\n"
          "  PETG\n"
          "  ABS\n"
  );
  if (!this->is_Program)
  {
    this->cursor_pointer = 0;
    this->setStatesToNone();
    this->is_Program = true;
  }
  
  this->drawCursor();
}

// Выбор температуры
void Interface::drawTemperature()
{
  this->clearDisplay();
  this->display->print("TEMPERATURE\n");
  
  if (!this->is_Temperature)
  {
    this->cursor_pointer = 0;
    this->setStatesToNone();
    this->is_Temperature = true;
    this->t_temperature = 0;
  }

  this->display->setTextSize(3);
  this->display->print(this->t_temperature);

  this->Show();
}


// Установка времени
void Interface::drawTime()
{
  this->clearDisplay();
  this->display->print("Time");
  
  this->cursor_pointer = 0;
  this->Show();

  this->setStatesToNone();
  this->is_Time = true;
  this->cursor_pointer = 0;
}

/*--------------------------------------------------------------
-------------------Обработчики воздействий----------------------
--------------------------------------------------------------*/

// Следующий пункт меню
int Interface::Next()
{
  if (this->is_MainMenu == false)
  {
    this->cursor_pointer++;
    if (this->cursor_pointer > 3)
      this->cursor_pointer = 0; 
    this->drawCursor();    
  }
  return 0;

}

// Предыдущий пункт меню
int Interface::Prev()
{
  if (this->is_MainMenu == false)
  {
    this->cursor_pointer--;
    if (this->cursor_pointer < 0)
      this->cursor_pointer = 3; 
    this->drawCursor();
  }
  return 0;
}

// Обработчик длинного нажатия
int Interface::Held()
{
  // Если находимся в выборе программы - подняться на уровень выше
  if (this->is_Program)
    this->drawFirstLevel();
  // Если находимся в настройке температуры - подняться выше
  else if (this->is_Temperature)
    this->drawFirstLevel();
  
  return 0;
}

// Обработчик клика
int Interface::Click()
{
  // Если нажата кнопка в главном меню
  if (this->is_MainMenu)
    this->drawFirstLevel();
  // Если нажата из главного меню
  else if (this->is_FirstLevel)
  {
    switch (this->cursor_pointer)
    {
      // Если выделен пункт UP
      case 0:
        this->drawMainPage();
        break;
      // Если выбран пункт Program
      case 1:
        this->drawProgram();
        break;
      // Если выбран пункт Temperature
      case 2:
        this->drawTemperature();
        break;
      // Если выбран пункт Time
      case 3:
        this->drawTime();
        break;
      default:
        break;
    }
  }
  // Если выбираем программу
  else if (this->is_Program)
  {
    switch (this->cursor_pointer)
    {
      case 0:
        this->drawFirstLevel();
        break;
      default:
        releController->setProgram(this->cursor_pointer);
        break;
    }
  }

  return 0;
}


/*--------------------------------------------------------------
-----------------------ВНУТРЕННИЕ ФУНКЦИИ-----------------------
--------------------------------------------------------------*/

// Сброс состояния
void  Interface::setStatesToNone()
{
  this->is_MainMenu = false;
  this->is_Program = false;
  this->is_Temperature = false;
  this->is_Time = false;
  this->is_FirstLevel = false;
}

// Отрисовка курсора
void Interface::drawCursor()
{
  // Закрашиваем область под курсор
  this->display->setCursor(0,0);
  this->display->fillRect(0, 0, 24, 64, SSD1306_BLACK);

  // Установка положения курсора, его отрисовка и отображение на дисплее
  this->display->setCursor(0, 16*this->cursor_pointer);
  this->display->print(">");
  this->Show();
}

//Очистка дисплея
void Interface::clearDisplay()
{
  this->display->clearDisplay();
  this->display->setCursor(0,0);
  this->display->setTextColor(SSD1306_WHITE);
  this->display->setTextSize(2);
}

// Команда на отрисовку экрана
void Interface::Show()
{
  this->display->display();
}

void Interface::update()
{
  if(this->is_MainMenu) this->drawMainPage();
  else if(this->is_Program) this->drawProgram();
  else if(this->is_Temperature) this->drawTemperature();
  else if(this->is_Time) this->drawTime();
  else if(this->is_FirstLevel) this->drawFirstLevel();
}


// Для дебага
void Interface::PrintState()
{
  Serial.print("Is/not main menu: ");
  Serial.print(this->is_MainMenu);
  Serial.println(!this->is_MainMenu);
}
