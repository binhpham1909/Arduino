#include "SendEmailSms.h"

const char* SendMailSms::host = "api.elasticemail.com";
const char* SendMailSms::sendEmailPath = "/v2/email/send?";
const char* SendMailSms::sendSmsPath = "/v2/sms/send?";

extern ModuleSettings g_ModuleSettings;

/////////////////////////////////////////////////////////////////////////
void SendMailSms::AddParam(const char *paramName, const char *paramValue) {
  static const char hex[] = "0123456789abcdef"; 
  char buf[4];
  unsigned int i, c;
  int n = strlen(paramValue);

  if ( m_bParamAdded ) m_Data += "&";
  m_Data += paramName;
  m_Data += "=";

  for (i=0; i<n; i++) {
      delay(0);
      c = paramValue[i];
      if( ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') ) {
        m_Data += (char)c;
      }
      else {
        buf[0] = '%';
        buf[1] = hex[(c>>4)&0xf];
        buf[2] = hex[c&0xf];
        buf[3] = 0;
        m_Data += buf;
      }
  }
  m_bParamAdded = true;
}
/////////////////////////////////////////////////////////////////////////
String SendMailSms::FormatPhoneNumber(const char* number) {
  String phone = "";
  if ( number==NULL ) return phone;
  int len = strlen(number);
  if ( len<10 ) return phone;
  
  phone.reserve(len+1);
  int i = 0;
  for (i=0; i<len; i++) {
    delay(0);
    if ( (number[i]>='0' && number[i]<='9') || number[i]=='+' ) phone += number[i];
  }
  if ( phone.charAt(0)=='+' ) return phone;
  len = phone.length();
  if ( len==10 ) return String("+1") + phone;
  if ( len>11 && phone.startsWith("00") ) return String("+") + phone.substring(2);
  return String("+") + phone;
}
/////////////////////////////////////////////////////////////////////////
bool SendMailSms::PrepareConnection(const char* path) {
  /*
  TRACE2("Heap size: ",ESP.getFreeHeap());
  m_Data.reserve(150);
  m_Data = "http://";
  m_Data += g_ModuleSettings.GetSMSServer();
  m_Data +="?";
  AddParam("type","email");
  AddParam("email_api_key",g_ModuleSettings.GetApiKey().c_str());
 */
  TRACE2("Heap size: ",ESP.getFreeHeap());
  
  m_Data.reserve(150);
  //m_Data = "https://";
  m_Data = "http://";
  m_Data += host;
  m_Data += path;
  AddParam("apikey",g_ModuleSettings.GetApiKey().c_str());
  return true;
}
/////////////////////////////////////////////////////////////////////////
bool SendMailSms::FinalizeConnection() {
  HTTPClient http;  

  ESP.wdtDisable();
  TRACE2("Open URL: ",m_Data);
  if ( !http.begin(m_Data) ) {
    TRACE2("Error opening URL: ",m_Data);
    ESP.wdtEnable(10000);
    return false;
  }
  int httpCode = http.GET();
  TRACE2(F("HTTP Code: "),httpCode);
  bool bRvl = false;
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    TRACE(payload);
    if (payload.indexOf("{\"success\":true")>=0) bRvl = true;
  }
  http.end();
  ESP.wdtEnable(10000);
  return bRvl; 
}
/////////////////////////////////////////////////////////////////////////
bool SendMailSms::PrepareSMSConnection() {

  TRACE2("Heap size: ",ESP.getFreeHeap());
  m_Data.reserve(150);
  m_Data = "http://";
  m_Data += g_ModuleSettings.GetSMSServer();
  m_Data +="?";
  AddParam("type","sms");
  AddParam("sms_api_key",g_ModuleSettings.GetSMSApiKey().c_str());
  AddParam("sms_api_secret",g_ModuleSettings.GetSMSApiSecret().c_str());
  
/*
  m_Data.reserve(150);
  //m_Data = "https://";
  m_Data = F("http://api.thingspeak.com/apps/thinghttp/send_request?");
  AddParam("api_key",g_ModuleSettings.GetSMSApiKey().c_str());
//  AddParam("api_secret",g_ModuleSettings.GetSMSApiSecret().c_str());
*/
  return true;
}
/////////////////////////////////////////////////////////////////////////
bool SendMailSms::FinalizeSMSConnection() {
  HTTPClient http;  

  ESP.wdtDisable();
  TRACE2("Open URL: ",m_Data);
  if ( !http.begin(m_Data) ) {
 // if ( !http.begin(m_Data) ) {
    TRACE2("Error opening URL: ",m_Data);
    ESP.wdtEnable(10000);
    return false;
  }

  int httpCode = http.GET();
  TRACE2(F("HTTP Code: "),httpCode);
  bool bRvl = false;
  if (httpCode = HTTP_CODE_OK) {
    String payload = http.getString();
    TRACE(payload);
    if (payload.indexOf("queued")>0) bRvl = true;
  }
  http.end();
  ESP.wdtEnable(10000);
  return bRvl; 
}
/////////////////////////////////////////////////////////////////////////
bool SendMailSms::SendEmail(const char* from, const char* to, const char* subject, const char* body) {
  PrepareConnection(sendEmailPath);
  AddParam("from", from);
  AddParam("sender", from);
  AddParam("replyTo", from);
  AddParam("subject", subject);
  AddParam("to", to);
  AddParam("bodyText", body);
  return FinalizeConnection();
}
/////////////////////////////////////////////////////////////////////////
bool SendMailSms::SendSms(const char* to, const char* body) {
  if ( !to || !body ) return false;
  PrepareSMSConnection();
//  AddParam("from", "13303623231");
  AddParam("to", FormatPhoneNumber(to).c_str());
  AddParam("text", body);
  return FinalizeSMSConnection();
}


