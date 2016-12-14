#ifndef ___TIME_SETTINGS_H__
#define ___TIME_SETTINGS_H__

#include <TimeLib.h>
#include <Timezone.h>
#include "ESP8266WebServerEx.h"
#include "SerialDebug.h"

#define MAX_TIMEZONE_TABLE    79

class TimeSettings {
protected:
  TimeChangeRule zr[2];  
  bool getTimeZoneRule(int idx);
  TimeSettings();
public:
  time_t static GetLocalTime(int timeZoneIndex); 
  bool static ParseData(ESP8266WebServerEx* server);
  void static GetTimeSettingsData(String& result);
};

#endif

