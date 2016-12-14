#ifndef __MY_WIFI_H__
#define __MY_WIFI_H__

#include <ESP8266WiFi.h>
#include "ESP8266WebServerEx.h"
#include "SerialDebug.h"
#include "ModuleSettings.h"

typedef struct {
  char ssid[32];
  int rssi;
} MyWifiAP;

#define MAX_MY_WIFI_AP 20

class MyWiFi {
  MyWifiAP *ap[MAX_MY_WIFI_AP];
  int apCount;
  int findAp(int rssi);
  void InitData();
  int RelativeRssi(int rssi);
public:
	MyWiFi();
  ~MyWiFi();
  bool AddAp(const char* ssid, int rssi);
  bool Scan();
  bool GetWifiList(String& buffer);
  static bool GetWifiData(String& buffer, bool bHeader=true);
  static bool ParseData(ESP8266WebServerEx* server);
};
#endif
