#ifndef ___INPUT_PIN_MGMT_H___
#define ___INPUT_PIN_MGMT_H___

#include <ESP8266WiFi.h>
#include "ModuleSettings.h"
#include "SendEmailSms.h"

#define MAX_INPUT_PIN_STATUS  5000

class InputPinMagement {
  unsigned long lastRun;
  int inputPinStatus[INPUT_PIN_COUNT];
  bool lastPinStatus[INPUT_PIN_COUNT];
  bool TestInputHigh(int pin);
  bool TestInputLow(int pin);
  void SendLowSmsEmail();
public:
  void SendEmailAndSms(int pin);
  InputPinMagement();
  void checkInputPinStatus();
  int GetInputPinStatus(int pin) { return lastPinStatus[pin] ? 1 : 0; }
};

#endif

