#ifndef _GENERAL_SETTINGS_H_
#define _GENERAL_SETTINGS_H_

#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include "ModuleSettings.h"
#include "TimeSettings.h"
#include "ESP8266WebServerEx.h"
#include "SerialDebug.h"

class General {
  int checkMinute;
  int weekend[7]={2,3,5,7,11,13,17};
public:
  General();
  bool static ParseData(ESP8266WebServerEx* server);
  void static GetGeneralData(ESP8266WebServerEx* server, String& result);
  void CheckPinSetOnOff();
  bool correctWeekend(int wkday, unsigned long wkend);
};

#endif



