#ifndef BJSON_h
#define BJSON_h

#include <ESP8266WiFi.h>
#include <PGMSPACE.h>
// Json encode
#define	BJSONONE	1
#define	BJSONFIRST	2
#define	BJSONNEXT	3
#define	BJSONLAST	4

const char lb_JSON_OPEN_BRAKE[] PROGMEM = "{\r\n";
const char lb_JSON_CLOSE_BRAKE[] PROGMEM = "\r\n}";
const char lb_JSON_NEW_LINE[] PROGMEM = ",\r\n";

class BJSON{
    public:
        void JsonEncode(int _position, String * _s, String _key, String _val);
    private:
        void JsonAdd(String *_s, String _key,String _val);
};

#endif