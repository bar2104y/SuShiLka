Controller::Controller()
{
  this->regulator = new GyverPID(20, .0, .0, 500);
  this->regulator->setDirection(NORMAL); // направление регулирования (NORMAL/REVERSE). ПО УМОЛЧАНИЮ СТОИТ NORMAL
  this->regulator->setLimits(0, 255);    // пределы (ставим для 8 битного ШИМ). ПО УМОЛЧАНИЮ СТОЯТ 0 И 255
  this->regulator->setpoint = 0;

  this->input = 0;
  this->output = 0;
}
Controller::~Controller()
{
  free(this->regulator);
}

// Конечная температура
int Controller::getTarget()
{
  return this->target;
}

// Текущая температура
float Controller::getInput()
{
  return this->input;
}

// Управляющий сигнал
int Controller::getSignal()
{
  return this->regulator->getResultTimer();
}

// Установка конечной температуры
void Controller::setTarget(int t)
{
  this->target = t;
  this->regulator->setpoint = this->target;
}

// Установить текущую температуру
void Controller::setInput(float i)
{
  this->input = i;
  this->regulator->input = this->input;
}

void Controller::setProgram(int i)
{
  switch(i)
  {
    case 1:
      this->setTarget(50);
      break;
    case 2:
      this->setTarget(70);
      break;
    case 3:
      this->setTarget(90);
      break;
    default:
      this->setTarget(0);
      break;
  }
}
