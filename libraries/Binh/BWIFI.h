#ifndef BWIFI_h
#define BWIFI_h

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <EEPROM.h>

class BWIFI{
    public:
        void connect(char* _ssid, char* _password);
        void checkConnect(void);
        void apmode(void);        
    private:  
        boolean CONNECTED=false;
        boolean	APMODE=false;  
};
#endif