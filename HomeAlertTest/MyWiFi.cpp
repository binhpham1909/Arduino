#include "MyWiFi.h"

extern ModuleSettings g_ModuleSettings;

//////////////////////////////////////////////////////////////////////////////////////////////////
MyWiFi::MyWiFi() {
  apCount = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
MyWiFi::~MyWiFi() {
  InitData();
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void MyWiFi::InitData() {
  for (int i=0; i<apCount; i++) {
    delay(0);
    if ( ap[i] ) delete ap[i];
    ap[i] = NULL;
  }
  apCount = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
int MyWiFi::findAp(int rssi) {

  if ( apCount==0 ) {
    ap[0] = NULL;
    apCount++;
    return 0; 
  }

  for (int i=0; i<apCount; i++) {
    delay(0);
    if ( ap[i] && ap[i]->rssi<rssi ) {

      // full, so remove the last so apCount < MAX_MY_WIFI_AP
      if (apCount>=MAX_MY_WIFI_AP) {
        apCount--;
        if ( ap[apCount] ) delete ap[apCount];
      }
      ap[apCount]=NULL;
     
      // now, apCount < MAX_MY_WIFI_AP
      for (int j=apCount; j>i; j--) {
        delay(0);
        ap[j] = ap[j-1];
      }
      ap[i] = NULL;
      apCount++;
      return i;
    }    
  }
  
  if ( apCount>=MAX_MY_WIFI_AP ) return -1;

  ap[apCount] = NULL;
  apCount++;
  return apCount-1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool MyWiFi::AddAp(const char* ssid, int rssi) {
  if ( !ssid ) return false;
  
  int idx = findAp(rssi);
  
  if ( idx<0 ) return false;
  ap[idx] = new MyWifiAP;
  if ( ap[idx] ) {
    strncpy(ap[idx]->ssid, ssid, 31);
    ap[idx]->rssi = rssi;
    return true;
  }
  return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool MyWiFi::Scan() {
  InitData();
  
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(500);
	
	TRACE("Start WiFi scanning...");
	int n = WiFi.scanNetworks();
	TRACE("Scan done.");
	
	if (n <= 0) {
		TRACE("No WiFi network found.");
		return false;
	}
 
	for (int i = 0; i<n; i++) {
      delay(0);
      if ( WiFi.SSID(i).indexOf('"')<0 ) {
      	int quality = RelativeRssi(WiFi.RSSI(i));
      	if ( quality ) AddAp(WiFi.SSID(i).c_str(),quality);
		  //TRACE((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*" );
      }
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool MyWiFi::GetWifiList(String& buffer) {
  bool bNext = false;
  buffer = "{";
  for (int i=0; i<apCount; i++) {
    delay(0);
    if ( ap[i] ) {
      if ( bNext ) buffer += ",\""; else buffer += "\"";
      buffer += ap[i]->ssid;
      buffer += " (";
      buffer += ap[i]->rssi;
      buffer += "%)\":\"";
      buffer += ap[i]->ssid;
      buffer += "\"";
      bNext = true;
    }
  }
  buffer +="}";
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// {header:true,ssid:"MyWIFI",password:"123456",dhcp:true,ip:"192.168.1.100",mask:"255.255.255.0",gw:"192.168.1.1",port:8080};
bool MyWiFi::GetWifiData(String& buffer, bool bHeader) {
  buffer = "{\"header\":";
  buffer += bHeader ? "true" : "false";
  buffer += ",\"ssid\":\"";
  buffer += g_ModuleSettings.data.ssid;
  buffer += "\",\"password\":\"";
  buffer += g_ModuleSettings.data.pw;
  buffer += "\",\"dhcp\":";
  buffer += g_ModuleSettings.data.dhcp ? "true" : "false";
  buffer += ",\"ip\":\"";
  buffer += g_ModuleSettings.data.ip;
  buffer += "\",\"mask\":\"";
  buffer += g_ModuleSettings.data.mask;
  buffer += "\",\"gw\":\"";
  buffer += g_ModuleSettings.data.gw;
  buffer += "\",\"port\":";
  buffer += g_ModuleSettings.data.port;
  buffer += ",\"mac\":\"";
  buffer += WiFi.macAddress();  
  buffer += "\"}";
  return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool MyWiFi::ParseData(ESP8266WebServerEx* server) {

  TRACE("MyWiFi-ssid");
  if ( !server->hasArg("ssid") || !g_ModuleSettings.SetWifiSsid( server->arg("ssid") ) ) {
    TRACE("invalid ssid");
    return false;
  }

  TRACE("MyWiFi-pw");
  if ( !server->hasArg("pw") || !g_ModuleSettings.SetWifiPassword( server->arg("pw") ) ) {
    TRACE("invalid pw");
    return false;
  }

  TRACE("MyWiFi-ip");
  if ( !server->hasArg("ip") || !g_ModuleSettings.SetWifiIp( server->arg("ip") ) ) {
    TRACE("invalid ip");
    return false;
  }

  TRACE("MyWiFi-mask");
  if ( !server->hasArg("mask") || !g_ModuleSettings.SetWifiMask( server->arg("mask") ) ) {
    TRACE("invalid mask");
    return false;
  }

  TRACE("MyWiFi-gw");
  if ( !server->hasArg("gw") || !g_ModuleSettings.SetWifiGw( server->arg("gw") ) ) {
    TRACE("invalid gw");
    return false;
  }

  TRACE("MyWiFi-port");
  if ( !server->hasArg("port") || !g_ModuleSettings.SetHttpPort( atoi(server->arg("port").c_str()) ) ) {
    TRACE("invalid port");
    return false;
  }

  TRACE("MyWiFi-dhcp");
  if ( !server->hasArg("dhcp") || !g_ModuleSettings.SetWifiDhcp( atoi(server->arg("dhcp").c_str()) ) ) {
    TRACE("invalid dhcp");
    return false;
  }
  TRACE("MyWiFi-return");
  return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// convert rssi to relative (%) value
//////////////////////////////////////////////////////////////////////////////////////////////////
int MyWiFi::RelativeRssi(int rssi) {
  int quality = 0;

  if (rssi <= -100) {
    quality = 0;
  } else if (rssi >= -50) {
    quality = 100;
  } else {
    quality = 2 * (rssi + 100);
  }
  return quality;
}
