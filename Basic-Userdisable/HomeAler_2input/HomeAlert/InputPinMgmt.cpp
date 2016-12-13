#include "InputPinMgmt.h"

extern int g_inputPins[INPUT_PIN_COUNT];
extern ModuleSettings g_ModuleSettings;

#ifdef PHASE2_CODE
extern const char *emailContentsPower;
extern const char *emailContentsAlarm;
extern const char *emailContentsFault;
#endif

InputPinMagement::InputPinMagement() {
  lastRun = 0;
  for (int i=0; i<INPUT_PIN_COUNT; i++) {
    delay(0);
    inputPinStatus[i] = 0;
    lastPinStatus[i] = digitalRead(g_inputPins[i]);
  }
}

bool InputPinMagement::TestInputHigh(int pin) {
  if ( digitalRead(g_inputPins[pin])==LOW ) return false;
  delay(1);
  if ( digitalRead(g_inputPins[pin])==LOW ) return false;
  delay(1);
  if ( digitalRead(g_inputPins[pin])==LOW ) return false;
  return true;
}
bool InputPinMagement::TestInputLow(int pin) {
  if ( digitalRead(g_inputPins[pin])==HIGH ) return false;
  delay(1);
  if ( digitalRead(g_inputPins[pin])==HIGH ) return false;
  delay(1);
  if ( digitalRead(g_inputPins[pin])==HIGH ) return false;
  return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// check input pin status and fill in a map of input pin
//////////////////////////////////////////////////////////////////////////////////////////////////
void InputPinMagement::checkInputPinStatus() {
  int i;
  unsigned long curRun = millis();
  
  for (i=0; i<INPUT_PIN_COUNT; i++) {
/* remove it because it only active when on
    delay(0);
    if ( inputPinStatus[i] ) {
      if  ( digitalRead(g_inputPins[i])==LOW ) inputPinStatus[i]=MAX_INPUT_PIN_STATUS;
      else {
        int delta = 1;
        if ( curRun>=lastRun ) if ( curRun-lastRun<10000 ) delta = (int)(curRun-lastRun); else delta = 10000;
        if ( inputPinStatus[i]>=delta ) inputPinStatus[i] -= delta; else inputPinStatus[i] = 0;
        lastRun = curRun;
      }
    } else {
      if ( TestInputHigh(i) ) {
        inputPinStatus[i]=MAX_INPUT_PIN_STATUS;
        SendEmailAndSms(i);
      }
    }
 change by bellow function: */
    // Input
    if(lastPinStatus[i]){
      if(TestInputLow(i)){
        lastPinStatus[i] = LOW;
        SendEmailAndSms(i);
      }
    }else{
      if(TestInputHigh(i)){
        lastPinStatus[i] = HIGH;
        SendEmailAndSms(i);
      }
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void InputPinMagement::SendLowSmsEmail() {
  SendMailSms snd;
  if ( g_ModuleSettings.data.SmsCount != g_ModuleSettings.data.SmsAlert ) return;
  
  String s = g_ModuleSettings.data.ModuleName;
  s += " - Warning: Low SMS count. Remain: ";
  s += (int)(g_ModuleSettings.data.SmsCount);
  delay(0);
  // send sms alert for low credit
  if ( snd.SendSms(g_ModuleSettings.data.Phone,s.c_str()) && g_ModuleSettings.data.SmsCount ) g_ModuleSettings.data.SmsCount--;
  delay(0);          
  snd.SendEmail(FROM_EMAIL_ADDRESS,FROM_EMAIL_ADDRESS,g_ModuleSettings.data.ModuleName,s.c_str()); 
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void InputPinMagement::SendEmailAndSms(int pin) {  
  SendMailSms snd;
  String msg;
  #ifdef PHASE2_CODE
  String emailMsg;
  #endif
  String subject; 

  msg = g_ModuleSettings.data.ModuleName;
  msg += ": ";
  subject = msg;
  #ifdef PHASE2_CODE
  emailMsg = msg;
  #endif
  if ( pin==0 ) {
    if(digitalRead(g_inputPins[pin])){
      msg += g_ModuleSettings.data.PowerMsg;
    }else{
      msg += g_ModuleSettings.data.PowerMsgLow;
    }
    #ifdef PHASE2_CODE
    emailMsg += emailContentsPower;
    #endif
    subject += "Zone 1";
  }
  else if ( pin==1 ) {
    if(digitalRead(g_inputPins[pin])){
      msg += g_ModuleSettings.data.AlarmMsg;
    }else{
      msg += g_ModuleSettings.data.AlarmMsgLow;
    }
    #ifdef PHASE2_CODE
    emailMsg += emailContentsAlarm;
    #endif
    subject += "Zone 2";
  }
  else if ( pin==2 ) {
    if(digitalRead(g_inputPins[pin])){
      msg += g_ModuleSettings.data.FaultMsg;
    }else{
      msg += g_ModuleSettings.data.FaultMsgLow;
    }
    #ifdef PHASE2_CODE
    emailMsg += emailContentsFault;
    #endif
    subject += "Fault!";
  }
  else return;

  if ( g_ModuleSettings.data.EmailEnable ) {
    delay(0);
    #ifdef PHASE2_CODE
    snd.SendEmail(FROM_EMAIL_ADDRESS,g_ModuleSettings.data.Email,subject.c_str(),emailMsg.c_str());
    #else
    snd.SendEmail(FROM_EMAIL_ADDRESS,g_ModuleSettings.data.Email,subject.c_str(),msg.c_str());
    #endif
    delay(0);
  }

  if ( g_ModuleSettings.data.PhoneEnable && g_ModuleSettings.data.SmsCount>0 ) {
    for (int i=0; i<g_ModuleSettings.data.SmsRt; i++) {
      delay(0);
      if ( snd.SendSms(g_ModuleSettings.data.Phone,msg.c_str()) ) {
        if ( g_ModuleSettings.data.SmsCount ) g_ModuleSettings.data.SmsCount--;
        SendLowSmsEmail();
        break;
      }
      delay(0);
    }    
  }
}

