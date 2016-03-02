#ifndef BHTML_h
#define BHTML_h

#include <ESP8266WiFi.h>
#include <PGMSPACE.h>
// Json encode
#define	BKVONE	1
#define	BKVFIRST	2
#define	BKVNEXT	3
#define	BKVLAST	4

const char lb_NEWLINE[] PROGMEM = "\r\n";
const char lb_HTTP_200[] PROGMEM= "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json\r\nUser-Agent: Wifi-switch\r\nConnection: close\r\n\r\n";
const char lb_HTTP_GET_PREFIX[] PROGMEM = "GET ";
const char lb_SERIAL_KEY_GET[] PROGMEM = "&&serial=";
const char lb_HTTP_HEADER_VERSION[] PROGMEM = " HTTP/1.1\r\n";
const char lb_HTTP_HEADER_HOST[] PROGMEM = "Host: ";
const char lb_HTTP_HEADER_CONNECTION[] PROGMEM = "Connection: close\r\nCache-Control: max-age=0\r\nAccept: text/html,text/plain\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) coc_coc_browser/50.2.175 Chrome/44.2.2403.175 Safari/537.36\r\nAccept-Encoding: gzip, deflate, sdch\r\nAccept-Language: en-US,en\r\n\r\n";
class BHTML{
    public:
        String createPOSTRequest(void);
        String createGETRequest(void);
    private:
        void createRequest(void);
};
#endif