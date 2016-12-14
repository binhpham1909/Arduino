#ifndef __EMAIL_SMS_H__
#define __EMAIL_SMS_H__

#include <ESP8266WiFi.h>
#include "ModuleSettings.h"
#include "ESP8266WebServerEx.h"
#include "SerialDebug.h"

class EmailSmsSettings {
public:
  EmailSmsSettings();
  void static GetEmailSmsSettings(ESP8266WebServerEx* server, String& result);
  bool static ParseData(ESP8266WebServerEx* server);
};

#endif

