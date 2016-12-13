#include <ESP8266WebServer.h>
#include <TimeLib.h>
#include <Arduino.h>
#include "ESP8266WebServerEx.h"
#include "ModuleSettings.h"
#include "SerialDebug.h"
#include "ArmLogic.h"


extern ModuleSettings g_ModuleSettings;
extern int g_ARM_OFF_PIN;
extern int g_ProgButton;
extern ArmLogic g_ArmLogic;

const char ESP8266WebServerEx::m_jsonMime[] = "application/json";
static const char adminUser[]="admin";
static const char normalUser[]="user";

String ESP8266WebServerEx::challengeString = "";

ESP8266WebServerEx::ESP8266WebServerEx(int port) : ESP8266WebServer(port) {
  hasLogin = true;
  
  m_sessionId = "esp8266";
  m_sessionId += ESP.getChipId();
  m_sessionId += "=";
}


long ESP8266WebServerEx::GetTag() {
  String session = GetSessionId();
  return g_Session.GetTag(session);
}

bool ESP8266WebServerEx::VerifyCookie(const char* mime) {
  if ( !hasLogin || strcmp(mime,"text/html")!=0 ) return true;

  String session = GetSessionId();
  long tag = -1;
  if ( session.length()==32 && g_Session.isValid(session,&tag) ) {
    if ( tag>0 && tag<10 ) {
      SetSessionId(session);
      return true;
    }
  }
  redirect("/"); 
  return false;    
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// check if system date is valid and send the date header.
//////////////////////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServerEx::sendDateHeader(char* buf) {
  time_t t = now();
  // if time is accurate, then try to send a Date header Wed, 15 Nov 1995 06:25:24 GMT
  if ( year(t)>2015 ) {
    char dow[5];  
    strncpy(dow,dayShortStr(weekday(t)),4);
    dow[3] = 0;
    sprintf(buf,"%s, %02u %s %04u %02u:%02u:%02u GMT",dow,day(t),monthShortStr(month(t)),year(t),hour(t),minute(t),second(t));
    sendHeader("Date",buf);
  } 
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// send data to client. Data is read from ROM  
// expire is # of seconds the cache will be expired.
//////////////////////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServerEx::sendEx(int code, const char* content_type, const char* data, size_t contentLength, bool bGzip, unsigned long expire) {

  if ( !VerifyCookie(content_type) ) return;
  
  String header;
  char buf[34];
    
  if ( bGzip ) sendHeader("Content-Encoding", "gzip");

  sendDateHeader(buf);
  if ( expire ) {
    sprintf(buf,"max-age=%lu",expire);
    sendHeader("Cache-Control",buf);
    sprintf(buf,"%lu",(unsigned long)contentLength);
    sendHeader("ETag",buf);
  }
  _prepareHeader(header, code, content_type, contentLength);
  _currentClient.write(header.c_str(), header.length());
  sendWebData(data,contentLength);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// send web content with handler function to process the data
//////////////////////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServerEx::sendEx(int code, const char* content_type, const char* data, size_t contentLength, TDataProcessingHandler handler) {
  String header;
  char buf[34];

  //if ( !VerifyCookie(content_type) ) return;
  sendDateHeader(buf);
  _prepareHeader(header, code, content_type, contentLength);
  _currentClient.write(header.c_str(), header.length());
  sendWebData(data,contentLength,handler);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// just call the base class
//////////////////////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServerEx::sendEx(int code, const String& content_type, const String& content) {
  if ( !VerifyCookie(content_type.c_str()) ) return;  
  send(code,content_type,content);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Read data from ROM and send to web client
//////////////////////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServerEx::sendWebData(const char* data, size_t dsize, TDataProcessingHandler handler) {
  #define SEND_DATA_BUFFER_SIZE 1024
  char *buf = (char*)malloc(SEND_DATA_BUFFER_SIZE*4+1);
  long n;
  size_t len = dsize;
  uint32 *p;
  uint32 *q;

  if ( !buf ) return;
  
  p = (uint32*)data;

  if ( ((unsigned long)dsize)&0x3 ) n = (dsize/4)+1;
  else n = (dsize/4);

  while (n>=SEND_DATA_BUFFER_SIZE) {
    q = (uint32*)buf;
    for (int i=0; i<SEND_DATA_BUFFER_SIZE; i++) {
      delay(0);
      *q = *p;
      p++;
      q++;
    }
    n -= SEND_DATA_BUFFER_SIZE;
    if ( handler ) (*handler)(buf,SEND_DATA_BUFFER_SIZE*4);
    sendContent_P(buf,SEND_DATA_BUFFER_SIZE*4);
    len -= SEND_DATA_BUFFER_SIZE*4;
  }
  
  if ( n ) {
    q = (uint32*)buf;
    while ( n-- ) {
      delay(0);
      *q = *p;
      p++;
      q++;
    }
    if ( handler ) (*handler)(buf,len);
    sendContent_P(buf,len);

  }
  free(buf);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// handle get GPIO pin from client
// receive a json such as {"0":0, "1":0, "5":1}
// return json such as {"0":1,"2":0,"4":1} which reflect current status of gpio
//////////////////////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServerEx::handleGetGpio() {
  char *buf = (char*)malloc(16);
  String result = "{";
  int i, n;

  if ( !buf ) {
    send(200, m_jsonMime, "{}");
    return;
  }

  for (i=0; i<MAX_PIN_NUMBER; i++) {
    delay(0);
    sprintf(buf,"%d",i);
    if ( hasArg(buf) ) {
      if  ( digitalRead(i)==HIGH ) n=1; else n=0;
      sprintf(buf,"\"%d\":%d",i,n);
      if ( result.length()>1 ) result += ",";
      result += buf;
    }
  }
  // virtual pin for ARM/DISARM
  sprintf(buf,",\"%d\":%d",g_ARM_OFF_PIN,g_ArmLogic.getOutputActive());
  result += "}";
  
  send(200, m_jsonMime, result);
  free(buf);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// handle set GPIO pin from client
// receive a json such as {"0":0, "1":0, "5":1}
// return true if success or false if fail
//////////////////////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServerEx::handleSetGpio() {
  char *buf = (char*)malloc(8);
  int i;

  if ( !buf ) {
    send(200, m_jsonMime, "{\"status\":false}");
    return;
  }

  for (i=0; i<MAX_PIN_NUMBER; i++) {
    delay(0);
    sprintf(buf,"%d",i);
    if ( hasArg(buf)&&(i!=g_ProgButton)) {
      if ( atoi(arg(buf).c_str()) ) {
        digitalWrite(i,HIGH);
      } else{
        digitalWrite(i,LOW);  
      }
    }else if ( hasArg(buf)&&(i==g_ProgButton)&&digitalRead(i)){// prog pin dang o trang thai cao
        digitalWrite(i,LOW);
        lastTimeProgButtonLow = millis();
    }
  }
  // Check virtual pin for ARM/DISARM
  sprintf(buf,"%d",g_ARM_OFF_PIN);
  if ( hasArg(buf) ) {
    if ( atoi(arg(buf).c_str()) ) {
      g_ArmLogic.resetLogic();
    } else{
      g_ArmLogic.disableBell();
    }
  }  
  send(200, m_jsonMime, "{\"status\":true}");
  free(buf);
}
void ESP8266WebServerEx::checkProgButton(){
    if(!digitalRead(g_ProgButton)&&((millis()-lastTimeProgButtonLow)>=30000)){
        digitalWrite(g_ProgButton,HIGH);
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////
String ESP8266WebServerEx::GetSessionId() { 
  String id = "";
  String cookie = header("Cookie"); 
  
  int idx = cookie.indexOf(m_sessionId);
  if ( idx<0 ) return id;
  idx = idx+m_sessionId.length();
  if ( idx+32>cookie.length() ) return id;
  id = cookie.substring(idx,idx+32);
  return id;  
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServerEx::SetSessionId(const String& id) {
  String cookie;

  cookie.reserve(50);
  cookie = m_sessionId;
  cookie += id;
  cookie += "; Path=/; Max-Age=";
  cookie += SESSION_EXPIRE_TIME;
  sendHeader("Set-Cookie",cookie);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
String ESP8266WebServerEx::createChallengeString(long challenge) {
  MD5Builder md5;  
  md5.begin();
  md5.add(String(challenge));
  md5.calculate();
  return md5.toString();  
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool ESP8266WebServerEx::checkPassword(const char* userId, const char *pw, const String& challenge, const String& hashPw) {
  if ( userId==NULL || pw==NULL ) return false;

  MD5Builder md5;  
  md5.begin();
  md5.add(userId);
  md5.add(challenge);
  md5.add(pw);
  md5.calculate();
  return hashPw==md5.toString();
}
//////////////////////////////////////////////////////////////////////////////////////////////////
String ESP8266WebServerEx::GetHost() {
  String host;
  
  host.reserve(24);
  host = header("Host");

  if ( host.length()<1 ) {
    host=WiFi.localIP().toString();
    if ( g_ModuleSettings.data.port!=80 ) {
      host += ":";
      host += g_ModuleSettings.data.port;
    }
  }
  TRACE2("Host: ",host);
  return host;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServerEx::redirect(const char* homePath) {
  uint16_t port = g_ModuleSettings.data.port;
  String url;

  if ( !homePath ) return;
  url.reserve(50);
  url = "http://";
  url += GetHost();
  url += homePath;
  
  String content;
  content.reserve(250);

  content = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta http-equiv=\"refresh\" content=\"0; URL='";
  content += url;
  content += "'\" />\n<title>Login</title>\n</head>\n<body onload=\"window.location='";
  content += url;
  content += "';\">\n</body>\n</html>";

  sendHeader("Location",url);
  sendHeader("Cache-Control","no-cache");
  sendHeader("Pragma","no-cache");

  TRACE("Redirect...");
    
  send(302,"text/html",content);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServerEx::handleLogout() {
  String session = GetSessionId();

  g_Session.Delete(session);
  redirect("/");
}


//////////////////////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServerEx::handleLogin(const char* login, size_t loginLength, const char* homePath) {
  
  hasLogin = true;
  
  String session = GetSessionId();
  long tag = -1;
  bool validSession = false;

  if ( session.length()==32 ) validSession = g_Session.isValid(session,&tag);

  // if everything is OK and user already logged in
  if ( validSession && tag>=0 && tag<10 ) {
    redirect(homePath);
    return;
  }

  // if user never login before, just display a form with challenge string
  long challenge = random(11,0x7fffffff);
  if ( !hasArg("userid") || !hasArg("hashpw") ) {
    challengeString = createChallengeString(challenge);
    SetSessionId(g_Session.Create(challenge));
    sendEx(200, "text/html", login, loginLength, &loginForm);
    return;
  }  

  // user already provided userId and password
  String userId = arg("userid");
  String hashPw = arg("hashpw");

  challengeString = createChallengeString(tag);
  if ( userId==adminUser ) {
    if ( checkPassword(adminUser,g_ModuleSettings.GetLoginPassword(true).c_str(),challengeString,hashPw) ) {
      g_Session.SetTag(session,ADMIN_USER_ID);
      redirect(homePath);
      return;  
    }
  } else if ( userId==normalUser) {
    if ( checkPassword(normalUser,g_ModuleSettings.GetLoginPassword(false).c_str(),challengeString,hashPw) ) {
      g_Session.SetTag(session,USER_USER_ID);
      redirect(homePath);
      return;
    }
  }

  // show the login form with invalid userid/password here
  challengeString = createChallengeString(challenge);
  SetSessionId(g_Session.Create(challenge));
  sendEx(200, "text/html", login, loginLength, &loginFormWithError);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServerEx::loginForm(char* data, int len) {
  if ( data==NULL ) return;
  data[len] = 0;
  char *p = strstr(data,"01234567890123456789012345678901");
  if ( p!=NULL ) memcpy(p,challengeString.c_str(),challengeString.length());
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServerEx::loginFormWithError(char* data, int len) {
  if ( data==NULL ) return;
  data[len] = 0;
  static const char err[] = "Error: User name or Password is incorrect!";  
  char *p = strstr(data,"                                          ");
  if ( p!=NULL ) memcpy(p,err,strlen(err)); 
  loginForm(data,len);
}
//////////////////////////////////////////////////////////////////////////////////////////////////


