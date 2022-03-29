#pragma once
#include "GyverPID.h"

class Controller
{
  public:
  Controller(int* a, int b);
  ~Controller();

  int GetSignal();
  
  private:
  GyverPID* regulator;

  int target;
  int output;
  int* input;
  
};
