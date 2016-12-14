#include "EmailSms.h"

extern ModuleSettings g_ModuleSettings;
extern int g_inputPins[];

EmailSmsSettings::EmailSmsSettings() {
  
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//  data["power"], data["alarm"], data["fault"], data["email"], data["emailEna"] (1/0)
//  data["phone"], data["phoneEna"] (1/0), data["smsCount"], data["smsAlert"], data["smsRt"]
//////////////////////////////////////////////////////////////////////////////////////////////////  
bool EmailSmsSettings::ParseData(ESP8266WebServerEx* server) {
  if ( !server->hasArg("power") || !g_ModuleSettings.SetPowerMsg(server->arg("power").c_str()) ) return false;
  if ( !server->hasArg("alarm") || !g_ModuleSettings.SetAlarmMsg(server->arg("alarm").c_str()) ) return false;
//  if ( !server->hasArg("fault") || !g_ModuleSettings.SetFaultMsg(server->arg("fault").c_str()) ) return false;
  if ( !server->hasArg("powerlow") || !g_ModuleSettings.SetPowerMsgLow(server->arg("powerlow").c_str()) ) return false;
  if ( !server->hasArg("alarmlow") || !g_ModuleSettings.SetAlarmMsgLow(server->arg("alarmlow").c_str()) ) return false;
//  if ( !server->hasArg("faultlow") || !g_ModuleSettings.SetFaultMsgLow(server->arg("faultlow").c_str()) ) return false;
  if ( !server->hasArg("email") || !g_ModuleSettings.SetEmail(server->arg("email").c_str()) ) return false;
  if ( !server->hasArg("emailEna") || !g_ModuleSettings.SetEmailEnable(atoi(server->arg("emailEna").c_str())) ) return false;
  if ( !server->hasArg("phone") || !g_ModuleSettings.SetPhone(server->arg("phone").c_str()) ) return false;
  if ( !server->hasArg("phoneEna") || !g_ModuleSettings.SetPhoneEnable(atoi(server->arg("phoneEna").c_str())) ) return false;
  if ( !server->hasArg("smsCount") || !g_ModuleSettings.SetSmsCount(atoi(server->arg("smsCount").c_str())) ) return false;
  if ( !server->hasArg("smsRt") || !g_ModuleSettings.SetSmsRt(atoi(server->arg("smsRt").c_str())) ) return false;
  if ( !server->hasArg("smsAlert") || !g_ModuleSettings.SetSmsAlert(atoi(server->arg("smsAlert").c_str())) ) return false;
  if ( !server->hasArg("apikey") || !g_ModuleSettings.SetApiKey(server->arg("apikey")) ) return false;
  if ( !server->hasArg("sms_server") || !g_ModuleSettings.SetSMSServer(server->arg("sms_server")) ) return false;
  if ( !server->hasArg("sms_api_key") || !g_ModuleSettings.SetSMSApiKey(server->arg("sms_api_key")) ) return false;
  if ( !server->hasArg("sms_api_secret") || !g_ModuleSettings.SetSMSApiSecret(server->arg("sms_api_secret")) ) return false;
  g_ModuleSettings.SaveSettings();
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// return Email and Sms settings
// {gpi0: "sfsdf", gpi1: "sfdsdf", gpi2: "sdfsdf", txtEmail: "john@foo.com", enaEmail:0, txtPhone:"405 455 4556", enaPhone:1, smsAlert:20,smsCount:1000,smsRt:2};
//////////////////////////////////////////////////////////////////////////////////////////////////
void EmailSmsSettings::GetEmailSmsSettings(ESP8266WebServerEx* server, String& result) { 
  int i;
  result.reserve(150 + strlen(g_ModuleSettings.data.PowerMsg) + strlen(g_ModuleSettings.data.AlarmMsg) + strlen(g_ModuleSettings.data.FaultMsg)
    + strlen(g_ModuleSettings.data.Email) + strlen(g_ModuleSettings.data.Phone) );

  result = "{\"role\":"; 
  result += server->GetTag();
  result += ",\"apikey\":\"";
  result += g_ModuleSettings.GetApiKey();
  result += "\",\"sms_server\":\"";
  result += g_ModuleSettings.GetSMSServer();
  result += "\",\"sms_api_key\":\"";
  result += g_ModuleSettings.GetSMSApiKey();
  result += "\",\"sms_api_secret\":\"";
  result += g_ModuleSettings.GetSMSApiSecret();
  result += "\",";
  
  for (i=0; i<INPUT_PIN_COUNT; i++) {
    delay(0);
    result += "\"gpi";
    result += i;
    result += "\":\"";
    switch (i) {
    case 0:
      result += g_ModuleSettings.data.PowerMsg;
      break;
    case 1:
      result += g_ModuleSettings.data.AlarmMsg;
      break;
    case 2:
      result += g_ModuleSettings.data.FaultMsg;
      break;
    default:
      break;
    }
    result += "\",";
  }

    for (i=0; i<INPUT_PIN_COUNT; i++) {
    delay(0);
    result += "\"gpi";
    result += i;
    result += "0\":\"";
    switch (i) {
    case 0:
      result += g_ModuleSettings.data.PowerMsgLow;
      break;
    case 1:
      result += g_ModuleSettings.data.AlarmMsgLow;
      break;
    case 2:
      result += g_ModuleSettings.data.FaultMsgLow;
      break;
    default:
      break;
    }
    result += "\",";
  }

  result += "\"txtEmail\":\"";
  result += g_ModuleSettings.data.Email;
  result += "\",\"enaEmail\":";
  result += g_ModuleSettings.data.EmailEnable ? "1" : "0";
  result += ",\"txtPhone\":\"";
  result += g_ModuleSettings.data.Phone;
  result += "\",\"enaPhone\":";
  result += g_ModuleSettings.data.PhoneEnable ? "1" : "0";
  result += ",\"smsCount\":";
  result += g_ModuleSettings.data.SmsCount;
  result += ",\"smsAlert\":";
  result += g_ModuleSettings.data.SmsAlert;
  result += ",\"smsRt\":";
  result += g_ModuleSettings.data.SmsRt;
  result += "}";
}

