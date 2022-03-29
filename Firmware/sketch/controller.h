#pragma once
#include "GyverPID.h"

class Controller
{
  public:
  Controller(float* a, int b);
  ~Controller();

  int getSignal();
  
  private:
  GyverPID* regulator;

  int target;
  int output;
  float* input;
  
};
