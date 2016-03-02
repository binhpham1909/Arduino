#ifndef BWIFI_h
#define BWIFI_h

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 

const char lb_CONNECTING[] PROGMEM = "Connecting to:";
const char lb_CONNECTED[] PROGMEM = "Connected ";
const char lb_SSID[] PROGMEM = "SSID ";
const char lb_PASSWORD[] PROGMEM = "PASSWORD ";
const char lb_AP[] PROGMEM = "AP ";
const char lb_APMODE_START[] PROGMEM = "APmode start";
const char lb_APMODE_STARTED[] PROGMEM = "APmode started";
const char lb_FAILED_CONNECT[] PROGMEM = "Failed connect ";
const char lb_DOT[] PROGMEM = ".";
const char lb_STA[] PROGMEM = "STA ";
const char lb_MAC_ADDRESS[] PROGMEM = "MAC ADDRESS ";
const char lb_IP[] PROGMEM = "IP ADDRESS ";
class BWIFI{
    public:
        boolean connect(char* _ssid, char* _password);
        boolean checkConnect(void);
        void setSTAMode(void)
        void setAPMode(void);       
    private:  
        boolean CONNECTED=false;
        boolean	APMODE=false;  
};
#endif