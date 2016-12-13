#include "ArmLogic.h"
#include "ModuleSettings.h"

extern ModuleSettings g_ModuleSettings;

ArmLogic::ArmLogic(int inputPin, int outputPin){
  _inputPin = inputPin;
  _bellPin = outputPin;
  _waitingInput = true;
  _outputActive = true;
  _bellState = LOW;
  pinMode(_bellPin,OUTPUT);
  digitalWrite(_bellPin,_bellState);
  _lastTimeLogic = millis();
}

void ArmLogic::resetLogic(){
  _waitingInput = true;
  _outputActive = true;
  _bellState = LOW;
}
void ArmLogic::disableBell(){
  _outputActive = false;
}
bool ArmLogic::getOutputActive(){
  return _outputActive;
}
void ArmLogic::runLogic(){
  if(_waitingInput&&digitalRead(_inputPin)){
    _waitingInput = false;
    _bellState = HIGH;
    _lastTimeLogic = millis();
  }
  if(!_waitingInput){
    if(millis()-_lastTimeLogic >= g_ModuleSettings.data.ArmLogicTime*60000){
      _lastTimeLogic = millis();
      _bellState = !_bellState;
    }
  }else{
    _bellState = LOW;
  }
  if(_outputActive) digitalWrite(_bellPin,_bellState);
  else digitalWrite(_bellPin,LOW);
}
