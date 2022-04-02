#pragma once
#include "GyverPID.h"

class Controller
{
public:
  Controller();
  ~Controller();

  int getTarget();
  float getInput();
  int getSignal();
  
  void setTarget(int t);
  void setInput(float i);

  void setProgram(int i);
 
  

private:
  GyverPID* regulator;
  int target, output;
  float input;
  unsigned long endTime;
  
};
