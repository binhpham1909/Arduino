#ifndef BJSON_h
#define BJSON_h

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "BLANGUE.h"
// Request encode
#define	ONEGET	1
#define	FIRSTGET	2
#define	NEXTGET	3
#define	LASTGET	4

class BHTML{
    public:
        BHTML(uint8_t _request_timeout, boolean _debug);
        boolean sendRequest(char* _server,uint16_t port, String *_tosend, String *_respone);
    protected:
        void addGETKeyValue(String *_s, String _key,String _val);
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