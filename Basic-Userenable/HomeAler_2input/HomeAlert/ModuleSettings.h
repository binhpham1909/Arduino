#ifndef _MODULE_SETTINGS_H_
#define _MODULE_SETTINGS_H_

//#define PHASE2_CODE

/////////////////////////////////////////////////////////////////////////////////////////
// change this value to enable low SMS alert email to send to
#define FROM_EMAIL_ADDRESS  "info2@nwstm-us.com"

// change this value to set the SMS Count value when updating ApiKey
#define DEFAULT_SMS_COUNT   10000
/////////////////////////////////////////////////////////////////////////////////////////


#define MODULE_NAME_MAX 31
#define TIME_SERVER_MAX 21

#define INPUT_PIN_COUNT 2
#define OUTPUT_PIN_COUNT 3
#define BUTTON_PIN_COUNT 2
#define NUMBER_VIRTUAL_PIN 1

#define MAX_CATEGORIES  3

#define MESSAGE_MAX_LENGTH   38
#define EMAIL_MAX_LENGTH   31
#define PHONE_MAX_LENGTH   15

#define MAX_SSID_LENGTH 33
#define MAX_WIFI_PASSWORD 21
#define MAX_IP_LENGTH 15

#define MAX_LOGIN_PASSWORD 13
#define API_KEY_LENGTH 36
#define SMS_SERVER_LENGTH 48
#define SMS_API_KEY_LENGTH 48

#define ADMIN_USER_ID   1
#define USER_USER_ID    2

typedef struct __attribute__((packed)) {
  char md5[34];
  char adminPassword[MAX_LOGIN_PASSWORD+1];
  char userPassword[MAX_LOGIN_PASSWORD+1];
  
  int Category;
  uint8_t TurnOnHour[OUTPUT_PIN_COUNT + NUMBER_VIRTUAL_PIN];
  uint8_t TurnOnMinute[OUTPUT_PIN_COUNT + NUMBER_VIRTUAL_PIN];
  uint8_t TurnOffHour[OUTPUT_PIN_COUNT + NUMBER_VIRTUAL_PIN];
  uint8_t TurnOffMinute[OUTPUT_PIN_COUNT + NUMBER_VIRTUAL_PIN];
  bool TurnOnEnable[OUTPUT_PIN_COUNT + NUMBER_VIRTUAL_PIN];
  bool TurnOffEnable[OUTPUT_PIN_COUNT + NUMBER_VIRTUAL_PIN];
  char ModuleName[MODULE_NAME_MAX+1];  
  char TimeServer[TIME_SERVER_MAX+1];
  unsigned long Weekend[OUTPUT_PIN_COUNT + NUMBER_VIRTUAL_PIN];
  int ArmLogicTime;
  
  char PowerMsg[MESSAGE_MAX_LENGTH+1];
  char AlarmMsg[MESSAGE_MAX_LENGTH+1];
  char FaultMsg[MESSAGE_MAX_LENGTH+1];
  char PowerMsgLow[MESSAGE_MAX_LENGTH+1];
  char AlarmMsgLow[MESSAGE_MAX_LENGTH+1];
  char FaultMsgLow[MESSAGE_MAX_LENGTH+1];
  char Email[EMAIL_MAX_LENGTH+1];
  char Phone[PHONE_MAX_LENGTH+1];
  bool EmailEnable;
  bool PhoneEnable;
  int SmsAlert;
  int SmsCount;
  int SmsRt;
  int TimeZone;
  int InputPulseTime;
  int SmsInput;
  
  char ssid[MAX_SSID_LENGTH+1];
  char pw[MAX_WIFI_PASSWORD+1];
  char ip[MAX_IP_LENGTH+1];
  char mask[MAX_IP_LENGTH+1];
  char gw[MAX_IP_LENGTH+1];
  int port;
  bool dhcp;
  char apiKey[API_KEY_LENGTH+1];
  char SMSServer[SMS_SERVER_LENGTH+1];
  char SMSApiKey[SMS_API_KEY_LENGTH+1];
  char SMSApiSecret[SMS_API_KEY_LENGTH+1];
  int ending;
} SettingData;

#define SET_INT_FIELD(value, field, minNumber, maxNumber) ({if ((value)<(minNumber)||(value)>(maxNumber)) return false; (field)=(value); return true;})

class ModuleSettings {
public:
  SettingData data;

  ModuleSettings();
  void SetTimeZone(int zone);
  void LoadDefaultSettings();
  void CleanSettings();
  bool SetTimeServer(const char* timeServer);
  bool SetTurnOnOff(int pin, int onHour, int onMin, bool enaOn, int offHour, int offMin, bool enaOff);
  bool SetCategories(int cat);
  bool SetModuleName(const char* moduleName);
  bool SetTurnOnHour(int pinNumber, int value);
  bool SetTurnOnMinute(int pinNumber, int value);
  bool SetTurnOffHour(int pinNumber, int value);
  bool SetTurnOffMinute(int pinNumber, int value);
  bool SetTurnOnEnable(int pinNumber, bool value);
  bool SetTurnOffEnable(int pinNumber, bool value);
  bool SetWeekend(int pinNumber, unsigned long value);
  bool ArmLogicTime(int value);
  bool InputPulseTime(int value);

  bool SetPowerMsg(const char* value) { return SetTextField(value,data.PowerMsg,MESSAGE_MAX_LENGTH); }
  bool SetAlarmMsg(const char* value) { return SetTextField(value,data.AlarmMsg,MESSAGE_MAX_LENGTH); }
  bool SetFaultMsg(const char* value) { return SetTextField(value,data.FaultMsg,MESSAGE_MAX_LENGTH); }
  bool SetPowerMsgLow(const char* value) { return SetTextField(value,data.PowerMsgLow,MESSAGE_MAX_LENGTH); }
  bool SetAlarmMsgLow(const char* value) { return SetTextField(value,data.AlarmMsgLow,MESSAGE_MAX_LENGTH); }
  bool SetFaultMsgLow(const char* value) { return SetTextField(value,data.FaultMsgLow,MESSAGE_MAX_LENGTH); }
  bool SetEmail(const char* value) { return SetTextField(value,data.Email,EMAIL_MAX_LENGTH); }
  bool SetPhone(const char* value) { return SetTextField(value,data.Phone,PHONE_MAX_LENGTH); }
  bool SetEmailEnable(int value) { data.EmailEnable = (value!=0); return true; }
  bool SetPhoneEnable(int value) { data.PhoneEnable = (value!=0); return true; }
  bool SetSmsCount(int value) { SET_INT_FIELD(value, data.SmsCount, 0, 99999); }
  bool SetSmsAlert(int value) { SET_INT_FIELD(value, data.SmsAlert, 0, 100); }
  bool SetSmsRt(int value) { SET_INT_FIELD(value, data.SmsRt, 0, 10); }
  bool SetSmsInput(int value) { SET_INT_FIELD(value, data.SmsInput, 0, 10); }
  bool SetWifiSsid(const String& value);
  bool SetWifiPassword(const String& value);
  bool SetWifiIp(const String& value);
  bool SetWifiMask(const String& value);
  bool SetWifiGw(const String& value);
  bool SetHttpPort(int value);
  bool SetWifiDhcp(int value);
  
  bool IsDirty();
  bool LoadSettings();
  bool SaveSettings();

  bool SetLoginPassword(const String& pw, bool bAdmin);
  String GetLoginPassword(bool bAdmin=true);
  bool SetApiKey(const String& key);
  String GetApiKey();
  bool SetSMSApiKey(const String& key);
  String GetSMSApiKey();
  bool SetSMSApiSecret(const String& key);
  String GetSMSApiSecret();
  bool SetSMSServer(const String& key);
  String GetSMSServer();
  
protected:
  bool SetTextField(const char *value, char *field, int maxLength);
//  bool SetIntField(int value, int& field, int minNumber, int maxNumber);

  void UpdateMd5();
  String CalcMd5();
};

#endif

