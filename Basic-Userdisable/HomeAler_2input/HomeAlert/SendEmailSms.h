#ifndef __SEND_SMS_H__
#define __SEND_SMS_H__

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "ModuleSettings.h"
#include "SerialDebug.h"

class SendMailSms {
protected:
  static const char* apiKey;
  static const char* fingerprint;
  static const char* SMSfingerprint;
  static const char* host;
  static const int httpsPort;
  static const char* sendEmailPath;
  static const char* sendSmsPath;
  
  bool m_bParamAdded;
  String m_Data;
  
  void AddParam(const char *paramName, const char *paramValue);
  bool PrepareConnection(const char* path);
  bool FinalizeConnection();
  bool PrepareSMSConnection();
  bool FinalizeSMSConnection();
  String FormatPhoneNumber(const char* number);
public:
  SendMailSms() {}
  bool SendEmail(const char* from, const char* to, const char* subject, const char* body);
  bool SendSms(const char* to, const char* body);
};

#endif

