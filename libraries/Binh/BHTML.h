#ifndef BHTML_h
#define BHTML_h

#include <ESP8266WiFi.h>
#include <PGMSPACE.h>
// Json encode
#define	BKVONE	1
#define	BKVFIRST	2
#define	BKVNEXT	3
#define	BKVLAST	4

class BHTML{
    public:
        String createPOSTRequest();
        String createGETRequest();
    private:
        void createRequest();
};
#endif