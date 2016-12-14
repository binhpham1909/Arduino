#ifndef _ARM_LOGIC_H_
#define _ARM_LOGIC_H_
#include <Arduino.h>

class ArmLogic{
  public:
    ArmLogic(int inputPin, int outputPin);
    void resetLogic();
    void disableBell();
    void runLogic();
    bool getOutputActive();
  private:
    uint32_t _lastTimeLogic;
    bool _outputActive, _bellState, _waitingInput;
    int _inputPin, _bellPin;
};


#endif
