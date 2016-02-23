#ifndef JSON_h
#define JSON_h

#include <ESP8266WiFi.h>
#include <PGMSPACE.h>
// Json encode
#define	ONEJSON	1
#define	FIRSTJSON	2
#define	NEXTJSON	3
#define	LASTJSON	4

const char lb_JSON_OPEN_BRAKE[] PROGMEM = "{\r\n";
const char lb_JSON_CLOSE_BRAKE[] PROGMEM = "\r\n}";
const char lb_JSON_NEW_LINE[] PROGMEM = ",\r\n";

class JSON{
    public:
        void Encode(int _position, String * _s, String _key, String _val);
    private:
        void Add(String *_s, String _key,String _val);    
};

#endif