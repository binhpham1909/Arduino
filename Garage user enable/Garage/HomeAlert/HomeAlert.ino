#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <TimeLib.h>
#include <Timezone.h>
//#include <FS.h>
#include "ModuleSettings.h"
#include "ESP8266WebServerEx.h"
#include "SendEmailSms.h"
#include "NTPClient.h"
#include "TimeSettings.h"
#include "SerialDebug.h"
#include "General.h"
#include "EmailSms.h"
#include "InputPinMgmt.h"
#include "HttpSession.h"
#include "InfoExchg.h"
#include "MyWiFi.h"
#include "ArmLogic.h"

//#include <ESP8266mDNS.h>

#include "admin.html.h"
#ifndef PHASE2_CODE
#include "email.html.h"
#else
//#include "email_p2.html.h"
#endif
#include "favicon.png.h"
#include "general.html.h"
#include "gpio.html.h"
#include "prog.html.h"
#include "login.html.h"
#include "time.html.h"
#include "device.html.h"
#include "network.html.h"
#include "skeleton.css.h"
#include "normalize.css.h"
#include "main.css.h"
#include "main.js.h"
#include "jquery-3.1.0.min.js.gz.h"
#include "md5.min.js.gz.h"

///////////////////////////////////////////////////////////////////////// 12 13
int g_inputPins[INPUT_PIN_COUNT]={4, 5, 14};
int g_outputPins[OUTPUT_PIN_COUNT]={15};
int g_buttonPins[BUTTON_PIN_COUNT]={12, 13, 2};
int g_ARM_INPUT_PIN = 5;
int g_ARM_OFF_PIN = 99; // virtual pin for toggle off ARM
int g_BELL_PIN = 16;
char* g_WifiApPassword = NULL;
IPAddress g_ApIp(192,168,1,1);
/////////////////////////////////////////////////////////////////////////

const int ANALOG_PIN = A0;  // Analog input. Need to seed the random generator

#define BROWSER_CACHE_DAYS   864000 // cache for 10 days

static const char MimeTypeJS[] = "application/javascript";
static const char MimeCss[] = "text/css";
static const char MimeHtml[] = "text/html";
static const char MimeJson[] = "application/json";
static const char MimePNG[] = "image/png";

static const char StatusJsonTrue[] = "{\"status\":true}";
static const char StatusJsonFalse[] = "{\"status\":false}";

InfoExchg udpServer;
CNTPClient NTP;
ModuleSettings g_ModuleSettings;
General g_General;
InputPinMagement g_InputPinMgmt;
ArmLogic g_ArmLogic(g_ARM_INPUT_PIN, g_BELL_PIN);
MyWiFi g_WifiList;
ESP8266WebServerEx *g_pServer = NULL;
DNSServer *g_pDnsServer = NULL;

void SetupGPIO();
void handleRoot();
void handleIoPinMaps();
void handleNotFound();
void handleGetTimeSettings();
void handleSetTimeSettings();
void handleGetGeneral();
void handleSetGeneral();
void handleGetEmailSms();
void handleSetEmailSms();
void handleGetDeviceInfo();
void handleGetGpio();
void handleGetDeviceList();
void handleScanWiFi();
void handleGetNetworkSettings(bool bHead=true);
void handleSetNetworkSettings();
void handleRebootEsp();
bool StartAsWifiSTA();
bool StartAsWifiAP();
bool g_IsAPMode = false;
bool g_FirstAPMode = false;
unsigned long g_lastSave = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
// global setup
void setup ( void ) { 
  delay(1000);  
  INIT_SERIAL_DEBUG();

  g_IsAPMode = false;

  g_ModuleSettings.LoadSettings();
  g_ModuleSettings.data.ending=8;
   g_ModuleSettings.SaveSettings();
  // seed the random generator sources
  randomSeed(analogRead(ANALOG_PIN) ^ millis());

  SetupGPIO();
  
  g_WifiList.Scan();

  if ( StartAsWifiSTA() ) {
    g_pServer = new ESP8266WebServerEx(g_ModuleSettings.data.port);
    TRACE2("HTTP server port: ",g_ModuleSettings.data.port);
    TRACE2("IP address: ",WiFi.localIP());
    g_FirstAPMode = false;
  }
  else {
    if ( StartAsWifiAP() ) {
        g_FirstAPMode = true;
      g_IsAPMode = true;
      g_pServer = new ESP8266WebServerEx(80);
      TRACE2("IP address: ",WiFi.softAPIP());
      g_pServer->ForceLogin(false);    
    } else {
      TRACE("Starting as WIFI AP failed.\nRebooting...");
      delay(2000);
      ESP.restart();
      return;
    }
  }

  //here the list of headers to be recorded
  const char * headerkeys[] = {"Cookie","Host"};
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);

  //ask server to track these headers
  g_pServer->collectHeaders(headerkeys, headerkeyssize );

  // send jquery and related CSS files
  g_pServer->on ( "/js/jquery-3.1.0.min.js.gz", []() { g_pServer->sendEx(200, MimeTypeJS, jquery_3_1_0_min_js_gz, sizeof(jquery_3_1_0_min_js_gz),true,BROWSER_CACHE_DAYS); });
  g_pServer->on ( "/js/md5.min.js.gz", []() { g_pServer->sendEx(200, MimeTypeJS, md5_min_js_gz, sizeof(md5_min_js_gz),true,BROWSER_CACHE_DAYS); });
  g_pServer->on("/js/main.js", []() { g_pServer->sendEx(200, MimeTypeJS, main_js, sizeof(main_js),false,BROWSER_CACHE_DAYS); });
  g_pServer->on("/css/skeleton.css", []() { g_pServer->sendEx(200, MimeCss, skeleton_css, sizeof(skeleton_css),false,BROWSER_CACHE_DAYS); });
  g_pServer->on("/css/normalize.css", []() { g_pServer->sendEx(200, MimeCss, normalize_css, sizeof(normalize_css),false,BROWSER_CACHE_DAYS); });
  g_pServer->on("/css/main.css", []() { g_pServer->sendEx(200, MimeCss, main_css, sizeof(main_css),false,BROWSER_CACHE_DAYS); });
  g_pServer->onNotFound ( handleNotFound );
  g_pServer->on("/favicon.png", []() { g_pServer->sendEx(200, MimePNG, favicon_png, sizeof(favicon_png),false,BROWSER_CACHE_DAYS); });
  //g_pServer->on("/favicon.png", handleFavicon);
  g_pServer->on("/js/ioPinMaps.js", handleIoPinMaps);
  g_pServer->on("/getdeviceinfo", handleGetDeviceInfo);
  g_pServer->on("/getgpio", handleGetGpio);
  g_pServer->on("/setgpio", []() {  g_pServer->handleSetGpio(); });
  g_pServer->on("/gettimesettings", handleGetTimeSettings);
  g_pServer->on("/settimesettings", handleSetTimeSettings);
  g_pServer->on("/getgeneral", handleGetGeneral);
  g_pServer->on("/setgeneral", handleSetGeneral);
  g_pServer->on("/getemailsms", handleGetEmailSms);
  g_pServer->on("/setemailsms", handleSetEmailSms);
  g_pServer->on("/getdevicelist", handleGetDeviceList);
  g_pServer->on("/scanwifi", handleScanWiFi);
  g_pServer->on("/setnetworksettings", handleSetNetworkSettings); 
  g_pServer->on("/reboot", handleRebootEsp);
  if ( g_IsAPMode ) {    
    g_pServer->on("/getnetworksettings", []() { handleGetNetworkSettings(false); });
    g_pServer->on("/network.html", []() { g_pServer->sendEx(200, MimeHtml, network_html, sizeof(network_html)); });
    g_pServer->on("/", []() { g_pServer->sendEx(200, MimeHtml, network_html, sizeof(network_html)); });
    
    g_pServer->begin();
    return;
  }

  g_pServer->on("/getnetworksettings", []() { handleGetNetworkSettings(); });  
  
  g_pServer->on("/", handleRoot);
  g_pServer->on("/admin.html", []() { g_pServer->sendEx(200, MimeHtml, admin_html, sizeof(admin_html)); });
  g_pServer->on("/gpio.html", []() { g_pServer->sendEx(200, MimeHtml, gpio_html, sizeof(gpio_html)); });
  g_pServer->on("/prog.html", []() { g_pServer->sendEx(200, MimeHtml, prog_html, sizeof(prog_html)); });
  g_pServer->on("/general.html", []() { g_pServer->sendEx(200, MimeHtml, general_html, sizeof(general_html)); });
  g_pServer->on("/network.html", []() { g_pServer->sendEx(200, MimeHtml, network_html, sizeof(network_html)); }); 
  g_pServer->on("/time.html", []() { g_pServer->sendEx(200, MimeHtml, time_html, sizeof(time_html)); }); 
  #ifndef PHASE2_CODE
  g_pServer->on("/email.html", []() { g_pServer->sendEx(200, MimeHtml, email_html, sizeof(email_html)); });
  #else
  g_pServer->on("/email.html", []() { g_pServer->sendEx(200, MimeHtml, email_p2_html, sizeof(email_p2_html)); });
  #endif
  g_pServer->on("/device.html", []() { g_pServer->sendEx(200, MimeHtml, device_html, sizeof(device_html)); });
  g_pServer->on("/logout.html", []() { g_pServer->handleLogout(); });
  g_pServer->on("/login.html", handleRoot);

  g_pServer->begin();
  udpServer.begin(); 
}

#ifdef _DEBUG_
unsigned long heapChk = 0;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
// main loop
//////////////////////////////////////////////////////////////////////////////////////////////////
void loop ( void ) {
    while(!g_FirstAPMode&&(WiFi.status()==WL_CONNECTION_LOST||WiFi.status()==WL_DISCONNECTED)){
        g_WifiList.Scan();
        delay(1000);
        StartAsWifiSTA();
    }
    
    if(g_FirstAPMode&&!g_ModuleSettings.data.ssid[0]){
        if ( g_ModuleSettings.data.pw[0] ) WiFi.begin(g_ModuleSettings.data.ssid, g_ModuleSettings.data.pw);
          else WiFi.begin(g_ModuleSettings.data.ssid);
          for (int i=0; i<30; i++) {
            if ( WiFi.isConnected() ) {
              TRACE("CONNECTED!");
              StartAsWifiSTA();
            }
            if (g_pServer) g_pServer->handleClient();
            delay(500);
          }
    }
    
  if (g_pServer) g_pServer->handleClient();

  if ( g_IsAPMode ) {
    if ( g_pDnsServer ) g_pDnsServer->processNextRequest();
    return;
  }
  g_InputPinMgmt.checkInputPinStatus();
  g_ArmLogic.runLogic();
  NTP.AutoSync();
    
  g_General.CheckPinSetOnOff();
  udpServer.Update();
  
  unsigned long curMillis = millis();
  if ( curMillis>g_lastSave ) {
    if ( curMillis-g_lastSave>3600000 ) {
      g_ModuleSettings.SaveSettings();
      g_lastSave = curMillis;
    }
  }
  else g_lastSave = curMillis;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
void handleRoot() {
  g_pServer->handleLogin(login_html,sizeof(login_html),"/gpio.html");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// return a JavaScript mapping of inputs and output pins
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleIoPinMaps() {
  int i;
  char buf[16];
  String data = "var outputPins = {";

  for (i=0; i<OUTPUT_PIN_COUNT; i++) {
  	delay(0);
    if ( i ) data += ",";
    sprintf(buf,"\"GPIO-%d\":%d",g_outputPins[i],g_outputPins[i]);
    data += buf;
  }
  // for virtual pin arm/disarm
  sprintf(buf,",\"GPIO-%d\":%d",g_ARM_OFF_PIN,g_ARM_OFF_PIN);
  data += buf;
  data += "};\nvar inputPins = {"; 

  for (i=0; i<INPUT_PIN_COUNT; i++) {
  	delay(0);
    if ( i ) data += ",";
    sprintf(buf,"\"GPIO-%d\":%d",g_inputPins[i],g_inputPins[i]);
    data += buf;
  }  
  data += "};\nvar buttonPins = {"; 

  for (i=0; i<BUTTON_PIN_COUNT; i++) {
      delay(0);
    if ( i ) data += ",";
    sprintf(buf,"\"GPIO-%d\":%d",g_buttonPins[i],g_buttonPins[i]);
    data += buf;
  }
  data += "};";

  g_pServer->send(200,MimeTypeJS,data);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// process not found case
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += g_pServer->uri();
  message += "\nMethod: ";
  message += ( g_pServer->method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += g_pServer->args();
  message += "\n";

  for ( uint8_t i = 0; i < g_pServer->args(); i++ ) {
  	delay(0);
    message += " " + g_pServer->argName ( i ) + ": " + g_pServer->arg ( i ) + "\n";
  }

  g_pServer->send ( 404, "text/plain", message );
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// process admintest
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//
void SetupGPIO() {
  int i;

  for (i=0; i<INPUT_PIN_COUNT; i++) {
  	delay(0);
    pinMode(g_inputPins[i], INPUT_PULLUP);
  }

  for (i=0; i<OUTPUT_PIN_COUNT; i++) {
    delay(0);
    pinMode(g_outputPins[i], OUTPUT);
  }

  for (i=0; i<BUTTON_PIN_COUNT; i++) {
    delay(0);
    pinMode(g_buttonPins[i], OUTPUT);
  }

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// return JSON {server: "timeg_pServer->com",time:12345648,zone:4}
// time is UNIX epoch and zone is the zone index
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleGetTimeSettings() {
  String result;
  TimeSettings::GetTimeSettingsData(result);
  g_pServer->send(200, MimeJson, result);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// client call this to set the time zone index {zone: 4, server: "pool.ntp.org"}
// return status OK if success or failed
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleSetTimeSettings() {
  if ( TimeSettings::ParseData(g_pServer) ) {
    NTP.SetNTPServer(g_ModuleSettings.data.TimeServer);
    g_pServer->send(200, MimeJson, StatusJsonTrue);
    g_ModuleSettings.SaveSettings();
  } else {
    g_pServer->send(200, MimeJson, StatusJsonFalse);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// return settings to client
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleGetGeneral() {
  String result; 
  General::GetGeneralData(g_pServer,result);
  g_pServer->send(200, MimeJson, result);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// Parse client data and save to the settings
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleSetGeneral() {
  if ( General::ParseData(g_pServer) ) {
    g_pServer->send(200, MimeJson, StatusJsonTrue);
    g_ModuleSettings.SaveSettings();
  } else {
    g_pServer->send(200, MimeJson, StatusJsonFalse);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// return email and sms settings to client
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleGetEmailSms() {
  String result;
  EmailSmsSettings::GetEmailSmsSettings(g_pServer,result);
  g_pServer->send(200, MimeJson, result);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// Parse client data and save to the settings for email and sms
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleSetEmailSms() {
  if ( EmailSmsSettings::ParseData(g_pServer) ) {
    g_pServer->send(200, MimeJson, StatusJsonTrue);
    g_ModuleSettings.SaveSettings();
  } else {
    g_pServer->send(200, MimeJson, StatusJsonFalse);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// return device information: {"name":"My Bedroom","cat:":"0","ip":"192.168.1.100","port":8080}
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleGetDeviceInfo() {
  String buffer;
  buffer.reserve(80);
  buffer = "{\"name\":\"";
  buffer += g_ModuleSettings.data.ModuleName;
  buffer += "\",\"cat\":\"";
  buffer += g_ModuleSettings.data.Category;
  buffer += "\",\"ip\":\"";
  buffer += WiFi.localIP().toString();
  buffer += "\",\"port\":";
  buffer += (int)g_ModuleSettings.data.port;
  buffer +="}";
  g_pServer->send(200, MimeJson, buffer);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// return a json of all inputs and output pin status
// {"gpi12":1,"gpo13":0}
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleGetGpio() {
  String buffer;
  int i;
  buffer.reserve(64);

  buffer = "{";
  for (i=0; i<INPUT_PIN_COUNT; i++) {
  	delay(0);
    if ( i>0 ) buffer += ",";
    buffer += "\"gpi";
    buffer += g_inputPins[i];
    buffer += "\":";
    buffer += g_InputPinMgmt.GetInputPinStatus(i);    
  }

  for (i=0; i<OUTPUT_PIN_COUNT; i++) {
  	delay(0);
    buffer += ",\"gpo";
    buffer += g_outputPins[i];
    buffer += "\":";
    buffer += (int)(digitalRead(g_outputPins[i])==HIGH);
  }
  // for virtual pin arm/disarm
    buffer += ",\"gpo";
    buffer += g_ARM_OFF_PIN;
    buffer += "\":";
    buffer += (int)g_ArmLogic.getOutputActive();
    
  for (i=0; i<BUTTON_PIN_COUNT; i++) {
      delay(0);
    buffer += ",\"gpo";
    buffer += g_buttonPins[i];
    buffer += "\":";
    buffer += (int)(digitalRead(g_buttonPins[i])==HIGH);
  }
  buffer += "}";
  g_pServer->send(200, MimeJson, buffer);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// receive: {cat: 1}
// return: [{"name":"Living Room","ip":"192.168.1.1","port":8080},{name:"Bed Room",ip:"192.168.1.2",port:8080}]
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleGetDeviceList() {
  String buffer;
  buffer.reserve(256);
  udpServer.ParseData(g_pServer,buffer);
  g_pServer->send(200, MimeJson, buffer);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleScanWiFi() {
  String buffer;
  buffer.reserve(256);
  
  g_WifiList.GetWifiList(buffer);
  g_pServer->send(200, MimeJson, buffer);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleGetNetworkSettings(bool bHead) {
  String buffer;
  buffer.reserve(256);
  MyWiFi::GetWifiData(buffer,bHead);
  g_pServer->send(200, MimeJson, buffer);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleSetNetworkSettings() {
  if ( MyWiFi::ParseData(g_pServer) ) {
    g_pServer->send(200, MimeJson, StatusJsonTrue);
    g_ModuleSettings.SaveSettings();
  } else {
    g_pServer->send(200, MimeJson, StatusJsonFalse);
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool StartAsWifiSTA() {
  WiFi.softAPdisconnect();
  WiFi.disconnect();
  WiFi.mode(WIFI_AP_STA);

  TRACE("Trying to connect to WIFI");

  // ip, gw, subnet, dns1, dns2
  if ( !g_ModuleSettings.data.dhcp ) {
    IPAddress ip;
    IPAddress gw;
    IPAddress mask;

    TRACE("Using static IP");

    if ( ip.fromString(g_ModuleSettings.data.ip) 
      && gw.fromString(g_ModuleSettings.data.gw)
      && mask.fromString(g_ModuleSettings.data.mask) ) {
      WiFi.config(ip, gw, mask, IPAddress(8,8,8,8), IPAddress(208,67,222,222));
    }
    else {
      TRACE("Error: Invalid IP format");
    }
  }
  else {
    TRACE("Using DHCP");
  }

  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);

  TRACE("Connecting to WIFI.");
  TRACE2("SSID:",g_ModuleSettings.data.ssid);
  TRACE2("PW:",g_ModuleSettings.data.pw);

  if ( g_ModuleSettings.data.pw[0] ) WiFi.begin(g_ModuleSettings.data.ssid, g_ModuleSettings.data.pw);
  else WiFi.begin(g_ModuleSettings.data.ssid);

  for (int i=0; i<30; i++) {
    if ( WiFi.isConnected() ) {
      TRACE("CONNECTED!");
      return true;
    }
    delay(500);
  }

  TRACE("Failed to connect to WIFI.\nStarting in AP Mode");
  return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool StartAsWifiAP() {
  WiFi.disconnect(); 
  WiFi.mode(WIFI_AP);

  WiFi.softAPConfig(g_ApIp,g_ApIp,IPAddress(255,255,255,0));

  g_pDnsServer = new DNSServer();

  String ssid = "ESP" + String(ESP.getChipId());
  bool bRvl = WiFi.softAP(ssid.c_str(),g_WifiApPassword);

  if ( bRvl && g_pDnsServer ) {
    // Setup the DNS server redirecting all the domains to the apIP
    g_pDnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    g_pDnsServer->start(53, "*", g_ApIp);
  }

  return bRvl;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void handleRebootEsp() {
  g_pServer->send(200, MimeJson, StatusJsonTrue);
  delay(100);
  g_pServer->close();
  ESP.restart();
}

