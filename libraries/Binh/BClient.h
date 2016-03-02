#ifndef BClient_h
#define BClient_h

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "BDef.h"
#include "BHTML.h"


class BClient:public BHTML {
    public:
        void config(uint8_t _request_timeout, boolean _debug);
        boolean sendRequest(char* _server,uint16_t port, String *_tosend, String *_respone);
    private:
        boolean DEBUG = true;
        WiFiClient client;
        uint32_t lastCheckTime =0;  // check time to calculate delta time replace delay function
        uint32_t delayTimeWaitRespone = 200;    // time delay wait for server respone
        boolean sendSuccess = false;
        boolean positionFuncSendRequest = 0;
        String respone = "";
};
#endif