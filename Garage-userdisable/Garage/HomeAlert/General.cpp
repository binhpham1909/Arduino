#include "General.h"
#include "ArmLogic.h"

extern ModuleSettings g_ModuleSettings;
extern int g_outputPins[];
extern ArmLogic g_ArmLogic;
extern int g_ARM_OFF_PIN;
General::General() {
  checkMinute = 0;  
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// a+pinName = hour turn on, e.g. a10=5 is pin 10 turn on at 5 hour
// m+pinName = minute turn on, e.g. m10=15 is pin 10 turn on at 15 minute
// x+pinName = 1 or 0 for enable/disable of turn on
//
// b+pinName = hour turn off, e.g. a10=5 is pin 10 turn off at 5 hour
// n+pinName = minute turn off, e.g. m10=15 is pin 10 turn off at 15 minute
// y+pinName = 1 or 0 for enable/disable of turn off
//////////////////////////////////////////////////////////////////////////////////////////////////
bool General::ParseData(ESP8266WebServerEx* server) {
  if ( !server ) return false;
  int i;
  char buf[8];  
  
  if ( !server->hasArg("cat") || !g_ModuleSettings.SetCategories( atoi(server->arg("cat").c_str()) ) ) return false;
  if ( !server->hasArg("device") || !g_ModuleSettings.SetModuleName(server->arg("device").c_str()) ) return false;
  if ( !server->hasArg("password") || !g_ModuleSettings.SetLoginPassword(server->arg("password"),server->GetTag()==ADMIN_USER_ID) ) return false;
  if ( !server->hasArg("alarmPulse") || !g_ModuleSettings.ArmLogicTime(atoi(server->arg("alarmPulse").c_str())) ) return false;
  if ( !server->hasArg("inputPulse") || !g_ModuleSettings.InputPulseTime(atoi(server->arg("inputPulse").c_str())) ) return false;
  for (i=0; i<OUTPUT_PIN_COUNT + NUMBER_VIRTUAL_PIN; i++) {
  	delay(0);
    if(i<OUTPUT_PIN_COUNT){
      sprintf(buf,"a%d",g_outputPins[i]);
    }else{
      sprintf(buf,"a%d",g_ARM_OFF_PIN);
    } 
    if ( server->hasArg(buf) && !g_ModuleSettings.SetTurnOnHour(i,atoi(server->arg(buf).c_str())) ) return false;
    buf[0]='m';
    if ( server->hasArg(buf) && !g_ModuleSettings.SetTurnOnMinute(i,atoi(server->arg(buf).c_str())) ) return false;
    buf[0]='b';
    if ( server->hasArg(buf) && !g_ModuleSettings.SetTurnOffHour(i,atoi(server->arg(buf).c_str())) ) return false;
    buf[0]='n';
    if ( server->hasArg(buf) && !g_ModuleSettings.SetTurnOffMinute(i,atoi(server->arg(buf).c_str())) ) return false;
    buf[0]='x';
    if ( server->hasArg(buf) && !g_ModuleSettings.SetTurnOnEnable(i,atoi(server->arg(buf).c_str())!=0) ) return false;
    buf[0]='y';
    if ( server->hasArg(buf) && !g_ModuleSettings.SetTurnOffEnable(i,atoi(server->arg(buf).c_str())!=0) ) return false;
    buf[0]='w';
    if ( server->hasArg(buf) && !g_ModuleSettings.SetWeekend(i,atol(server->arg(buf).c_str())) ) return false;  
  }
  
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// return to the client
// {"device":"Device name","cat":"0","h120":"10:15","c120":true,"h121":"10:15","c121":false
// 0 is turn on, 1 is turn off
//////////////////////////////////////////////////////////////////////////////////////////////////
void General::GetGeneralData(ESP8266WebServerEx* server, String& result) {
  char buf[20];
  int i;
  static const char sTrue[]="true";
  static const char sFalse[]="false";
  
  result.reserve(320);
  result = "{\"device\":\""; 
  result += g_ModuleSettings.data.ModuleName;
  result += "\",\"password\":\"";
  result += g_ModuleSettings.GetLoginPassword(server->GetTag()==ADMIN_USER_ID);
  result += "\",\"cat\":\"";
  result += g_ModuleSettings.data.Category;
  result += "\",\"alarmPulse\":\"";
  result += g_ModuleSettings.data.ArmLogicTime;
  result += "\",\"inputPulse\":\"";
  result += g_ModuleSettings.data.InputPulseTime;
  result += "\"";

  for (i=0; i<OUTPUT_PIN_COUNT + NUMBER_VIRTUAL_PIN; i++) {
  	delay(0);
   if(i<OUTPUT_PIN_COUNT){
      sprintf(buf,",\"h%d0\":\"%02d:%02d\"",g_outputPins[i],g_ModuleSettings.data.TurnOnHour[i],g_ModuleSettings.data.TurnOnMinute[i]);
      result += buf;
      sprintf(buf,",\"c%d0\":",g_outputPins[i]);
      result += buf;
      result += g_ModuleSettings.data.TurnOnEnable[i] ? sTrue : sFalse;   
      sprintf(buf,",\"h%d1\":\"%02d:%02d\"",g_outputPins[i],g_ModuleSettings.data.TurnOffHour[i],g_ModuleSettings.data.TurnOffMinute[i]);
      result += buf;
      sprintf(buf,",\"c%d1\":",g_outputPins[i]);
      result += buf;
      result += g_ModuleSettings.data.TurnOffEnable[i] ? sTrue : sFalse;  
      sprintf(buf,",\"w%d\":\"%ld\"",g_outputPins[i],g_ModuleSettings.data.Weekend[i]);
      result += buf;      
   }else{
      sprintf(buf,",\"h%d0\":\"%02d:%02d\"",g_ARM_OFF_PIN,g_ModuleSettings.data.TurnOnHour[i],g_ModuleSettings.data.TurnOnMinute[i]);
      result += buf;
      sprintf(buf,",\"c%d0\":",g_ARM_OFF_PIN);
      result += buf;
      result += g_ModuleSettings.data.TurnOnEnable[i] ? sTrue : sFalse;   
      sprintf(buf,",\"h%d1\":\"%02d:%02d\"",g_ARM_OFF_PIN,g_ModuleSettings.data.TurnOffHour[i],g_ModuleSettings.data.TurnOffMinute[i]);
      result += buf;
      sprintf(buf,",\"c%d1\":",g_ARM_OFF_PIN);
      result += buf;
      result += g_ModuleSettings.data.TurnOffEnable[i] ? sTrue : sFalse;  
      sprintf(buf,",\"w%d\":\"%ld\"",g_ARM_OFF_PIN,g_ModuleSettings.data.Weekend[i]);
      result += buf;     
   }
  }
  result += "}"; 
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// check schedule on/off every minute and set the pin accordingly
void General::CheckPinSetOnOff() {
  int curMin = minute();

  if ( curMin==checkMinute ) return;
  checkMinute = curMin;
  
  time_t local = TimeSettings::GetLocalTime(g_ModuleSettings.data.TimeZone);
  int hr = hour(local);
  int mm = minute(local);
  int wk = weekday(local);
  //TRACE2("Current hour: ",hr);
  //TRACE2("Current minute: ",mm);

  for (int i=0; i<OUTPUT_PIN_COUNT + NUMBER_VIRTUAL_PIN; i++) {
  	delay(0);
    if ( g_ModuleSettings.data.TurnOffEnable[i] ) {
      if(correctWeekend(wk, g_ModuleSettings.data.Weekend[i])){
        if ( g_ModuleSettings.data.TurnOffHour[i]==hr && g_ModuleSettings.data.TurnOffMinute[i]==mm ) {
          if(i<OUTPUT_PIN_COUNT){
            TRACE2("Turn off pin: ",g_outputPins[i]);
            digitalWrite(g_outputPins[i],LOW);            
          }else{
            TRACE("ARM Deactived");
            g_ArmLogic.disableBell();
          }
          continue;
        }        
      }
    }
    if ( g_ModuleSettings.data.TurnOnEnable[i] ) {
      if(correctWeekend(wk, g_ModuleSettings.data.Weekend[i])){
        if ( g_ModuleSettings.data.TurnOnHour[i]==hr && g_ModuleSettings.data.TurnOnMinute[i]==mm ) {
          if(i<OUTPUT_PIN_COUNT){
            TRACE2("Turn on pin: ",g_outputPins[i]);
            digitalWrite(g_outputPins[i],HIGH);
          }else{
            TRACE("ARM Actived");
            g_ArmLogic.resetLogic();
          }
        }
      }
    }         
  }
}
bool General::correctWeekend(int wkday, unsigned long wkend){
  bool retVal;
  switch (wkday){
    case 1: {
      if((wkend % (unsigned long)weekend[0])==0) retVal = true;
      else retVal = false;
      break;
    }
    case 2: {
      if((wkend % (unsigned long)weekend[1])==0) retVal = true;
      else retVal = false;
      break;
    }  
    case 3: {
      if((wkend % (unsigned long)weekend[2])==0) retVal = true;
      else retVal = false;
      break;
    }  
    case 4: {
      if((wkend % (unsigned long)weekend[3])==0) retVal = true;
      else retVal = false;
      break;
    }  
    case 5: {
      if((wkend % (unsigned long)weekend[0])==0) retVal = true;
      else retVal = false;
      break;
    }  
    case 6: {
      if((wkend % (unsigned long)weekend[5])==0) retVal = true;
      else retVal = false;
      break;
    }  
    case 7: {
      if((wkend % (unsigned long)weekend[6])==0) retVal = true;
      else retVal = false;
      break;
    }  
  }
  return retVal;
}
