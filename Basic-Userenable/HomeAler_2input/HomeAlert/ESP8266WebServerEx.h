#ifndef _ESP8266_WEB_SERVER_EX_H_
#define _ESP8266_WEB_SERVER_EX_H_

#include <Arduino.h>
#include <libb64/cencode.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "HttpSession.h"

#define MAX_PIN_NUMBER    17

typedef void (*TDataProcessingHandler)(char* data, int len);

class ESP8266WebServerEx : public ESP8266WebServer {
  
protected:

  bool hasLogin;
  HttpSession g_Session;
  static String challengeString;
  static const char m_jsonMime[];
  String m_sessionId;

  void sendDateHeader(char* buf);
  void redirect(const char* homePath);
  bool checkPassword(const char* userId, const char *pw, const String& challenge, const String& hashPw);
  String createChallengeString(long challenge);

  static void loginForm(char* data, int len);
  static void loginFormWithError(char* data, int len);

  bool VerifyCookie(const char* mime);
  void sendEx(int code, const char* content_type, const char* data, size_t contentLength, TDataProcessingHandler handler);  

public:
  void ForceLogin(bool bLogin) { hasLogin=bLogin; }

  String GetSessionId();
  void SetSessionId(const String& id);

  String GetHost();

  ESP8266WebServerEx(int port = 80);

  long GetTag();

  // send data to client. Data is read from ROM  
  void sendEx(int code, const char* content_type, const char* data, size_t contentLength, bool bGzip = false, unsigned long expire=0);
  void sendEx(int code, const String& content_type, const String& content);

  // Read data from ROM and send to web client
  void sendWebData(const char* data, size_t dsize, TDataProcessingHandler handler=NULL);

  void handleGetGpio();
  void handleSetGpio();

  void handleLogin(const char* login, size_t loginLength, const char* homePath);
  void handleLogout();
};

#endif

