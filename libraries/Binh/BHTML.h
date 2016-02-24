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
        String sendRequest(char* _server,uint16_t port, String *_tosend);
    private:
        void addGETKeyValue(String *_s, String _key,String _val);
        uint8_t MAX_REQUEST_TIMEOUT = 3;
        boolean DEBUG = true;
};
#endif