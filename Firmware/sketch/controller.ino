Controller::Controller(int* a, int b)
{
  this->regulator = new GyverPID(0.1, 0.1, 0.1, 500);
  this->regulator->setDirection(NORMAL); // направление регулирования (NORMAL/REVERSE). ПО УМОЛЧАНИЮ СТОИТ NORMAL
  this->regulator->setLimits(0, 255);    // пределы (ставим для 8 битного ШИМ). ПО УМОЛЧАНИЮ СТОЯТ 0 И 255
  this->regulator->setpoint = 0;

  this->input = a;
  this->output = b;

  this->regulator->setpoint = b;
}
Controller::~Controller()
{
  free(this->regulator);
}

int Controller::GetSignal()
{
  return this->regulator->getResultTimer();
}
