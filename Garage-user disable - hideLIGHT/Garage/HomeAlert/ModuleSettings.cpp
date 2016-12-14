#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "ModuleSettings.h"
#include "SerialDebug.h"
#include "TimeSettings.h"

extern int g_outputPins[];

static const char default_apiKey[] = "db67b42f-96ea-49a0-aacb-3ab958cdeadc";
static const char default_SMSApiKey[] = "f8bdca74";
static const char default_SMSApiSecret[] = "f00c543ee476ad85";
#ifdef PHASE2_CODE
const char default_email[] = "sextroys@yahoo.com";
const char *emailContentsPower = "23454578111@alarmmail.lab33.co";
const char *emailContentsAlarm = "23454578112@alarmmail.lab33.co";
const char *emailContentsFault = "23454578113@alarmmail.lab33.co";
#endif


ModuleSettings::ModuleSettings() {
  LoadDefaultSettings(); 
}

String ModuleSettings::CalcMd5() {
  MD5Builder builder;
  int md5len = sizeof(data.md5);
  uint8_t *p = (uint8_t*)(&data);
  p += md5len;
  delay(0);
  builder.begin();
  builder.add(p,sizeof(ModuleSettings)-md5len);
  builder.calculate();
  return builder.toString();
}

void ModuleSettings::UpdateMd5() {
  strncpy(data.md5,CalcMd5().c_str(),32);
}

bool ModuleSettings::IsDirty() {
  TRACE2("MD5 of Settings: ",data.md5);
  TRACE2("Calculated MD5:  ",CalcMd5());
  return CalcMd5()!=data.md5;
}

bool ModuleSettings::LoadSettings() {  
  int n = sizeof(SettingData);
  uint8_t *p = (uint8_t*)(&data);

  TRACE("Loading setting from EEPROM");
  EEPROM.begin(n);
  for (int i=0; i<n; i++) {
    delay(0);
    *p = EEPROM.read(i);
    p++;
  }

  EEPROM.end();

  if ( IsDirty() ) {
    TRACE("Invalid EEPROM data. Load default setting instead.");
    LoadDefaultSettings(); 
    return false;
  }
  return true;
}

bool ModuleSettings::SaveSettings() {

  if ( !IsDirty() ) {
    TRACE("Nothing to save as no change");
    return true;
  }
  CleanSettings();
  UpdateMd5();
  
  int n = sizeof(SettingData);
  uint8_t *p = (uint8_t*)(&data);

  EEPROM.begin(n);
  for (int i=0; i<n; i++) {
    delay(0);
    EEPROM.write(i,*p);
    p++;
  }

  bool bRvl = EEPROM.commit();
  if ( bRvl ) {
    TRACE("Settings saved succesfully.");
  }
  else {
    TRACE("Error saving settings.");
  }
  EEPROM.end();
  return bRvl;
}
void ModuleSettings::CleanSettings(){
  int n = sizeof(SettingData);
  EEPROM.begin(n);
  for (int i=0; i<sizeof(SettingData); i++) {
    delay(0);
    EEPROM.write(i,0);
  }
  bool bRvl = EEPROM.commit();
  if ( bRvl ) {
    TRACE("Clean succesfully.");
  }
  else {
    TRACE("Error Clean settings.");
  }
  EEPROM.end();    
}
void ModuleSettings::LoadDefaultSettings() {
  memset(&data,0,sizeof(SettingData));
#ifdef PHASE2_CODE
  strcpy(data.Email,default_email);
#endif 
  int i;
  strncpy(data.apiKey,default_apiKey,API_KEY_LENGTH);
  strncpy(data.SMSApiKey,default_SMSApiKey,API_KEY_LENGTH);
  strncpy(data.SMSApiSecret,default_SMSApiSecret,API_KEY_LENGTH);
  strcpy(data.adminPassword,"Sesame1+");
  strcpy(data.userPassword,"12345678");

  for (i=0; i<OUTPUT_PIN_COUNT; i++) {
  	delay(0);
    data.TurnOnHour[i] = 0;
    data.TurnOnMinute[i] = 0;
    data.TurnOffHour[i] = 0;
    data.TurnOffMinute[i] = 0;
    data.TurnOffEnable[i] = false;
    data.TurnOnEnable[i] = false;
    data.Weekend[i] = 0;
  }
  data.ArmLogicTime = 5; // default 5 minutes
  
  data.TimeZone = 9; // US&Canada Central time
  data.Category = 0;
  strcpy(data.TimeServer,"pool.ntp.org");
  String mac = WiFi.softAPmacAddress();
  mac.replace(':','_');
  strcpy(data.ModuleName,mac.c_str());

  strcpy(data.PowerMsg,"Power error");
  strcpy(data.PowerMsgLow,"Power off");
  strcpy(data.AlarmMsg,"Alarm!");
  strcpy(data.AlarmMsgLow,"Alarm OFF!");
  strcpy(data.FaultMsg,"Fault!");
  strcpy(data.FaultMsgLow,"Fault OFF!");
  data.EmailEnable = false;
  data.PhoneEnable = false;
  data.SmsAlert = 20;
  data.SmsCount = 1000;
  data.SmsRt = 2;

  data.ssid[0] = 0;
  data.pw[0] = 0;
  data.dhcp = true;
  data.ip[0] = 0;
  data.mask[0] = 0;
  data.gw[0] = 0;
  data.port = 80;
  data.InputPulseTime=5;
  data.SmsInput=2;
  data.md5[33] = 0;
  UpdateMd5();
}
bool ModuleSettings::SetWifiSsid(const String& value) {
  strncpy(data.ssid,value.c_str(),MAX_SSID_LENGTH);
  return true;
}
bool ModuleSettings::SetWifiPassword(const String& value) {
  strncpy(data.pw,value.c_str(),MAX_WIFI_PASSWORD);
  return true;
}
bool ModuleSettings::SetWifiIp(const String& value) {
  strncpy(data.ip,value.c_str(),MAX_IP_LENGTH);
  return true;
}
bool ModuleSettings::SetWifiMask(const String& value) {
  strncpy(data.mask,value.c_str(),MAX_IP_LENGTH);
  return true;
}
bool ModuleSettings::SetWifiGw(const String& value) {
  strncpy(data.gw,value.c_str(),MAX_IP_LENGTH);
  return true;
}
bool ModuleSettings::SetWifiDhcp(int value) {
  data.dhcp = (bool)value;
  return true;
}

bool ModuleSettings::SetHttpPort(int value) {
  if ( value>=80 && value<=65535 ) {
    data.port = value;
    return true;
  }
  return false;
}

bool ModuleSettings::SetTimeServer(const char* timeServer) {
  if ( !timeServer ) return false;
  if ( !timeServer[0] ) return false;
  strncpy(data.TimeServer,timeServer,TIME_SERVER_MAX);
  return true;
}

bool ModuleSettings::SetTurnOnOff(int pin, int onHour, int onMin, bool enaOn, int offHour, int offMin, bool enaOff) {
  int i;

  if ( onHour<0 || onHour>23 || onMin<0 || onMin>59 ) return false;
  if ( offHour<0 || offHour>23 || offMin<0 || offMin>59 ) return false;

  // looking for the pin index and set the value
  for (i=0; i<OUTPUT_PIN_COUNT + NUMBER_VIRTUAL_PIN; i++) {
  	delay(0);
    if ( g_outputPins[i]==pin ) {
      data.TurnOnHour[i]= onHour;
      data.TurnOnMinute[i]= onMin;
      data.TurnOffHour[i]= offHour;
      data.TurnOffMinute[i]= offMin;
      data.TurnOnEnable[i] = enaOn;
      data.TurnOffEnable[i] = enaOff;
      return true;
    }
  }
  return false;
}

bool ModuleSettings::SetTurnOnHour(int pinNumber, int value) {
  if ( value<0 || value>23 || pinNumber<0 || pinNumber>=OUTPUT_PIN_COUNT  + NUMBER_VIRTUAL_PIN) return false;
  data.TurnOnHour[pinNumber] = value;
  return true;
}
bool ModuleSettings::SetTurnOnMinute(int pinNumber, int value){
  if ( value<0 || value>59 || pinNumber<0 || pinNumber>=OUTPUT_PIN_COUNT  + NUMBER_VIRTUAL_PIN) return false;
  data.TurnOnMinute[pinNumber] = value;
  return true;  
}
bool ModuleSettings::SetTurnOffHour(int pinNumber, int value){
  if ( value<0 || value>23 || pinNumber<0 || pinNumber>=OUTPUT_PIN_COUNT  + NUMBER_VIRTUAL_PIN) return false;
  data.TurnOffHour[pinNumber] = value;
  return true;
}
bool ModuleSettings::SetTurnOffMinute(int pinNumber, int value){
  if ( value<0 || value>59 || pinNumber<0 || pinNumber>=OUTPUT_PIN_COUNT  + NUMBER_VIRTUAL_PIN) return false;
  data.TurnOffMinute[pinNumber] = value;
  return true;
}
bool ModuleSettings::SetTurnOnEnable(int pinNumber, bool value){
  if ( pinNumber<0 || pinNumber>=OUTPUT_PIN_COUNT  + NUMBER_VIRTUAL_PIN) return false;
  data.TurnOnEnable[pinNumber] = value;
  return true;
}
bool ModuleSettings::SetTurnOffEnable(int pinNumber, bool value){
  if ( pinNumber<0 || pinNumber>=OUTPUT_PIN_COUNT  + NUMBER_VIRTUAL_PIN) return false;
  data.TurnOffEnable[pinNumber] = value;
  return true;
}
bool ModuleSettings::SetWeekend(int pinNumber, unsigned long value){
  if ( pinNumber<0 || pinNumber>=OUTPUT_PIN_COUNT  + NUMBER_VIRTUAL_PIN) return false;
  data.Weekend[pinNumber] = value;
  return true;
}
bool ModuleSettings::ArmLogicTime(int value){
  if(value<=0||value>30)  return false;
  data.ArmLogicTime = value;
  return true;
}
bool ModuleSettings::InputPulseTime(int value){
  if(value<=0||value>30)  return false;
  data.InputPulseTime = value;
  return true;
}
bool ModuleSettings::SetCategories(int cat) {
  if ( cat<0 || cat>MAX_CATEGORIES ) return false;
  data.Category = cat;
  return true;
}

bool ModuleSettings::SetModuleName(const char* moduleName) {
  if ( moduleName==NULL || moduleName[0]==0 ) return false;
  strncpy(data.ModuleName,moduleName,MODULE_NAME_MAX);
  return true;
}

void ModuleSettings::SetTimeZone(int zone) {
  if ( zone>=0 && zone<MAX_TIMEZONE_TABLE ) data.TimeZone = zone;
  
}

bool ModuleSettings::SetTextField(const char *value, char *field, int maxLength) {
  if ( value==NULL ) return false;
  strncpy(field,value,maxLength);
  return true;
}

bool ModuleSettings::SetLoginPassword(const String& pw, bool bAdmin) {
  if ( bAdmin ) strncpy(data.adminPassword,pw.c_str(),MAX_LOGIN_PASSWORD);
  else strncpy(data.userPassword,pw.c_str(),MAX_LOGIN_PASSWORD);
  return true;
}

String ModuleSettings::GetLoginPassword(bool bAdmin) {
  if ( bAdmin ) return String(data.adminPassword);
  else return String(data.userPassword);
}

bool ModuleSettings::SetApiKey(const String& key) {
  if ( key.length()!=API_KEY_LENGTH ) return false;
  if ( key!=data.apiKey ) {
    strncpy(data.apiKey,key.c_str(),API_KEY_LENGTH);
  }
  return true;
}

String ModuleSettings::GetApiKey() {
  return data.apiKey;  
}

bool ModuleSettings::SetSMSApiKey(const String& key) {
  if ( key.length()>=SMS_API_KEY_LENGTH ) return false;
  if ( key!=data.SMSApiKey ) {
    strncpy(data.SMSApiKey,key.c_str(),SMS_API_KEY_LENGTH);
    data.SmsCount = DEFAULT_SMS_COUNT;
  }
  return true;
}

String ModuleSettings::GetSMSApiKey() {
  return data.SMSApiKey;  
}

bool ModuleSettings::SetSMSApiSecret(const String& key) {
  if ( key.length()>=SMS_API_KEY_LENGTH ) return false;
  if ( key!=data.SMSApiSecret ) {
    strncpy(data.SMSApiSecret,key.c_str(),SMS_API_KEY_LENGTH);
  }
  return true;
}

String ModuleSettings::GetSMSApiSecret() {
  return data.SMSApiSecret;  
}
bool ModuleSettings::SetSMSServer(const String& key) {
  if ( key.length()>=SMS_SERVER_LENGTH ) return false;
  if ( key!=data.SMSServer ) {
    strncpy(data.SMSServer,key.c_str(),SMS_SERVER_LENGTH);
  }
  return true;
}

String ModuleSettings::GetSMSServer() {
  return data.SMSServer;  
}
