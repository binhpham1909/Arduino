#ifndef BClient_h
#define BClient_h

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "BDef.h"
#include "BHTML.h"


class BClient{
    public:
        boolean sendRequest(uint32_t _server,uint16_t port, String *_tosend, uint8_t _request_timeout);
        boolean checkRespone(void);
        String getRespone(void);
    private:
        WiFiClient client;
        uint32_t lastCheckTime =0;  // check time to calculate delta time replace delay function
        uint32_t requestTimeOut = 30000;    // time delay wait for server respone
        boolean sendSuccess = false;
        String respone = "";
};
#endif