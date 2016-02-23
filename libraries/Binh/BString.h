#ifndef BString_h
#define BString_h

#include <ESP8266WiFi.h>

class BString{
    public:
        boolean StringToArray(String *StringFrom, char* arrayTo, int maxlen);
        uint32_t StringToIPAdress(String IPvalue);    
        String htmlGETValue(String *_request,String _key);
    private:
        String decodeToKeyValue(String *_request, String _separate, String _end,String _enall, String *_key);    
};

#endif