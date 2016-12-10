#include "TimeSettings.h"
#include "ESP8266WebServerEx.h"
#include "ModuleSettings.h"

extern ModuleSettings g_ModuleSettings;

static const TimeChangeRule ICACHE_RODATA_ATTR g_TimeZoneRule[MAX_TIMEZONE_TABLE*2] {
  {"IDLW", First, Sun, Nov, 2, -720}, {"IDLW", First, Sun, Nov, 2, -720},
  {"CUT11", First, Sun, Nov, 2, -660},{"CUT11", First, Sun, Nov, 2, -660},
  {"HAST", First, Sun, Nov, 2, -600}, {"HAST", First, Sun, Nov, 2, -600},
  {"AKDT", Second, Sun, Mar, 2, -480},{"AKST", First, Sun, Nov, 2, -540},
  {"PDT", Second, Sun, Mar, 2, -420}, {"PST", First, Sun, Nov, 2, -480},
  {"PDT", Second, Sun, Mar, 2, -420}, {"PST", First, Sun, Nov, 2, -480},
  {"MDT", Second, Sun, Mar, 2, -360}, {"MST", First, Sun, Nov, 2, -420},
  {"MDT", Second, Sun, Mar, 2, -360}, {"MST", First, Sun, Nov, 2, -420},
  {"MST", First, Sun, Nov, 2, -420},  {"MST", First, Sun, Nov, 2, -420},
  {"CDT", Second, Sun, Mar, 2, -300}, {"CST", First, Sun, Nov, 2, -360},
  {"CST", First, Sun, Nov, 2, -360},  {"CST", First, Sun, Nov, 2, -360},
  {"CDT", Second, Sun, Mar, 2, -300}, {"CST", First, Sun, Nov, 2, -360},
  {"CST", First, Sun, Nov, 2, -360},  {"CST", First, Sun, Nov, 2, -360},
  {"EDT", Second, Sun, Mar, 2, -240}, {"EST", First, Sun, Nov, 2, -300},
  {"EDT", Second, Sun, Mar, 2, -240}, {"EST", First, Sun, Nov, 2, -300},
  {"ACT", First, Sun, Nov, 2, -300},  {"ACT", First, Sun, Nov, 2, -300},
  {"ADT", Second, Sun, Mar, 2, -240}, {"AST", First, Sun, Nov, 2, -180},
  {"VET", First, Sun, Nov, 2, -270},  {"VET", First, Sun, Nov, 2, -270},
  {"CLT", Second, Sun, May, 0, -240}, {"CLST", Second, Sun, Aug, 0, -180},
  {"PYT", Fourth, Sun, Mar, 0, -240}, {"PYST", First, Sun, Oct, 0, -180},
  {"AMT", Third, Sun, Feb, 0, -240},  {"AMST", Third, Sun, Oct, 0, -180},
  {"GYT", First, Sun, Nov, 2, -240},  {"GYT", First, Sun, Nov, 2, -240},
  {"NDT", Second, Sun, Mar, 2, -150}, {"NST", First, Sun, Nov, 2, -210},
  {"BRT", Third, Sun, Feb, 0, -180},  {"BRST", Third, Sun, Oct, 0, -120},
  {"ART", First, Sun, Nov, 2, -180},  {"ART", First, Sun, Nov, 2, -180},
  {"WGT", Fourth, Sat, Mar, 22, -180},{"WGST", Fourth, Sat, Oct, 23, -120},
  {"MALT", First, Sun, Nov, 2, -120}, {"MALT", First, Sun, Nov, 2, -120},
  {"AZOST", Last, Sun, Mar, 0, 0},  {"AZOT", Last, Sun, Oct, 1, -60},
  {"CVE", First, Sun, Nov, 2, -60}, {"CVE", First, Sun, Nov, 2, -60},
  {"BST", Last, Sun, Mar, 1, 60},   {"GMT", Last, Sun, Oct, 2, 0},
  {"CEST", Last, Sun, Mar, 2, 120}, {"CET", Last, Sun, Oct, 3, 60},
  {"CEST", Last, Sun, Mar, 2, 120}, {"CET", Last, Sun, Oct, 3, 60},
  {"CEST", Last, Sun, Mar, 2, 120}, {"CET", Last, Sun, Oct, 3, 60},
  {"CEST", Last, Sun, Mar, 2, 120}, {"CET", Last, Sun, Oct, 3, 60},
  {"WAT", First, Sun, Nov, 2, 60},  {"WAT", First, Sun, Nov, 2, 60},
  {"EEST", Last, Sun, Mar, 3, 180}, {"EET", Last, Sun, Mar, 4, 120},
  {"EET", Last, Sun, Mar, 2, 120},  {"EET", Last, Sun, Mar, 2, 120},
  {"EEST", Last, Sun, Mar, 3, 180}, {"EET", Last, Sun, Mar, 4, 120},
  {"EEST", Last, Sun, Mar, 3, 180}, {"EET", Last, Sun, Mar, 4, 120},
  {"EEST", Last, Sun, Mar, 3, 180}, {"EET", Last, Sun, Mar, 4, 120},
  {"EEST", Last, Sun, Mar, 3, 180}, {"EET", Last, Sun, Mar, 4, 120},
  {"MSK", First, Sun, Nov, 2, 120}, {"MSK", First, Sun, Nov, 2, 120},
  {"IDT", Fourth, Fri, Mar, 2, 180},  {"IST", Last, Sun, Oct, 2, 120},
  {"CAT", First, Sun, Nov, 2, 120}, {"CAT", First, Sun, Nov, 2, 120},
  {"MSK", First, Sun, Nov, 2, 180}, {"MSK", First, Sun, Nov, 2, 180},
  {"AST", First, Sun, Nov, 2, 180}, {"AST", First, Sun, Nov, 2, 180},
  {"EAT", First, Sun, Nov, 2, 180}, {"EAT", First, Sun, Nov, 2, 180},
  {"AST", First, Sun, Nov, 2, 180}, {"AST", First, Sun, Nov, 2, 180},
  {"GST", First, Sun, Nov, 2, 240}, {"GST", First, Sun, Nov, 2, 240},
  {"AZT", First, Sun, Nov, 2, 240}, {"AZT", First, Sun, Nov, 2, 240},
  {"AFT", First, Sun, Nov, 2, 270}, {"AFT", First, Sun, Nov, 2, 270},
  {"YEKT", First, Sun, Nov, 2, 300},  {"YEKT", First, Sun, Nov, 2, 300},
  {"PKT", First, Sun, Nov, 2, 300}, {"PKT", First, Sun, Nov, 2, 300},
  {"IST", First, Sun, Nov, 2, 330}, {"IST", First, Sun, Nov, 2, 330},
  {"NPT", First, Sun, Nov, 2, 345}, {"NPT", First, Sun, Nov, 2, 345},
  {"BST", First, Sun, Nov, 2, 360}, {"BST", First, Sun, Nov, 2, 360},
  {"IST", First, Sun, Nov, 2, 360}, {"IST", First, Sun, Nov, 2, 360},
  {"ALMT", First, Sun, Nov, 2, 360},  {"ALMT", First, Sun, Nov, 2, 360},
  {"MMT", First, Sun, Nov, 2, 390}, {"MMT", First, Sun, Nov, 2, 390},
  {"ICT", First, Sun, Nov, 2, 420}, {"ICT", First, Sun, Nov, 2, 420},
  {"KRAT", First, Sun, Nov, 2, 420},  {"KRAT", First, Sun, Nov, 2, 420},
  {"CST", First, Sun, Nov, 2, 480}, {"CST", First, Sun, Nov, 2, 480},
  {"SGT", First, Sun, Nov, 2, 480}, {"SGT", First, Sun, Nov, 2, 480},
  {"CST", First, Sun, Nov, 2, 480}, {"CST", First, Sun, Nov, 2, 480},
  {"AWST", First, Sun, Nov, 2, 480},  {"AWST", First, Sun, Nov, 2, 480},
  {"ULAST", Last, Sat, Mar, 2, 540},  {"ULAT", Last, Sat, Sep, 0, 480},
  {"KST", First, Sun, Nov, 2, 540}, {"KST", First, Sun, Nov, 2, 540},
  {"JST", First, Sun, Nov, 2, 540}, {"JST", First, Sun, Nov, 2, 540},
  {"YAKT", First, Sun, Nov, 2, 540},  {"YAKT", First, Sun, Nov, 2, 540},
  {"ACST", First, Sun, Nov, 2, 570},  {"ACST", First, Sun, Nov, 2, 570},
  {"ACDT", First, Sun, Oct, 2, 630},  {"ACST", First, Sun, Apr, 3, 570},
  {"AEDT", First, Sun, Oct, 2, 660},  {"AEST", First, Sun, Apr, 3, 600},
  {"AEST", First, Sun, Nov, 2, 600},  {"AEST", First, Sun, Nov, 2, 600},
  {"AEDT", First, Sun, Oct, 2, 660},  {"AEST", First, Sun, Apr, 3, 600},
  {"VLAT", First, Sun, Nov, 2, 600},  {"VLAT", First, Sun, Nov, 2, 600},
  {"CHST", First, Sun, Nov, 2, 600},  {"CHST", First, Sun, Nov, 2, 600},
  {"NCT", First, Sun, Nov, 2, 660}, {"NCT", First, Sun, Nov, 2, 660},
  {"MHT", First, Sun, Nov, 2, 720}, {"MHT", First, Sun, Nov, 2, 720},
  {"NZDT", Last, Sun, Sep, 2, 780}, {"NZST", First, Sun, Apr, 3, 720}
};

TimeSettings::TimeSettings(){}

time_t TimeSettings::GetLocalTime(int timeZoneIndex) {
  TimeSettings ts;

  if ( timeZoneIndex<0 || timeZoneIndex>=MAX_TIMEZONE_TABLE ) return 0;
  ts.getTimeZoneRule(timeZoneIndex);

  Timezone tz(ts.zr[0],ts.zr[1]);
  time_t t = tz.toLocal(now());

  return t;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool TimeSettings::getTimeZoneRule(int idx) {
  long *p = (long*)zr;
  long *q = (long*)&g_TimeZoneRule[idx*2];
  int i;
  int n = sizeof(TimeChangeRule)/2;
  
  if ( idx>=MAX_TIMEZONE_TABLE ) return false;
  for (i=0; i<n; i++) {
  	delay(0);
    *p = *q;
    p++;
    q++;
  }
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// return JSON {server: "timeserver.com",time:12345648,zone:4}
// time is UNIX epoch and zone is the zone index
//////////////////////////////////////////////////////////////////////////////////////////////////
void TimeSettings::GetTimeSettingsData(String& result) {
  result.reserve(48);
  result = "{\"server\":\"";  
  TRACE2("TimeServer: ",g_ModuleSettings.data.TimeServer);
  result += g_ModuleSettings.data.TimeServer;
  result += "\",\"time\":";
  result += (unsigned long)(TimeSettings::GetLocalTime(g_ModuleSettings.data.TimeZone));
  result += ",\"zone\":";
  result += g_ModuleSettings.data.TimeZone;
  result += "}";
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// client call this to set the time zone index {zone: 4, server: "pool.ntp.org"}
// return status OK if success or failed
//////////////////////////////////////////////////////////////////////////////////////////////////
bool TimeSettings::ParseData(ESP8266WebServerEx* server) {
  if ( !server ) return false;
  int idx = atoi(server->arg("zone").c_str());

  if ( idx<0 || idx>=MAX_TIMEZONE_TABLE ) return false;
  g_ModuleSettings.SetTimeZone(idx);

  if ( g_ModuleSettings.SetTimeServer(server->arg("server").c_str()) ) {
    TRACE2("Set NTP server: ",server->arg("server"));
    return true;
  }
  
  return false;  
}

