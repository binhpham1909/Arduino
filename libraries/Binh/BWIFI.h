#ifndef BWIFI_h
#define BWIFI_h

#include <ESP8266WiFi.h>
#include "BDef.h"


class BWIFI{
    public:
        void config(boolean _debug);  
        boolean connect(char* _ssid, char* _password);
        boolean checkConnected(void);
        void reConnect(char* _ssid, char* _password);
        uint8_t getConnectTimeOut(void);
        void setSTAMode(void);
        void setAPMode(char* _apssid, char* _appassword);  
    private:  
        boolean DEBUG = false;
        boolean CONNECTED=false;
        boolean	APMODE=false;  
        uint32_t lastConnect = 0;
        uint32_t lastCheckUnConnect = 0;
        uint8_t maxTimeOut = 30;    // second
};
#endif