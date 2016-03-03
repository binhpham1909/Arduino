#ifndef BDictionary_h
#define BDictionary_h

#include <ESP8266WiFi.h>
#include "BDef.h"

#define NULL_KEY    65535

class BDict{
    public:
        void set(String _key, String _value);
        String get(String _key);
        uint16_t checkKey(String _key);
        uint16_t insertDict(String _key);
        void updateDict(uint16_t _index, String _value);
    protected:
        Dict *m_DICT;
    private:
        uint16_t d_size = 0;;
}



#endif