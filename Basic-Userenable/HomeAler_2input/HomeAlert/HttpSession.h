#ifndef ___HTTP_SESSION_H___
#define ___HTTP_SESSION_H___

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include "SerialDebug.h"

#define MAX_HTTP_SESSION  20
#define SESSION_ID_LENGTH 33

#define SESSION_EXPIRE_TIME   (60*10) //  minutes

class HttpSession {
  
protected:
  String sessionId[MAX_HTTP_SESSION];
  time_t sessionTime[MAX_HTTP_SESSION];
  long sessionTag[MAX_HTTP_SESSION];
  void Delete(int idx);

  bool isExpired(int idx, time_t cur);
  int Find(const String& session);
public:
  static String GetRandomString();
  
  HttpSession(); 
  bool isValid(const String& session, long* tag=NULL);
  long GetTag(const String& session);
  bool SetTag(const String& session, long tag);
  String Create(long tag = 0);
  void Delete(const String& session);
};

#endif

