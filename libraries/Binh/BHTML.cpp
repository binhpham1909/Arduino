#include "BHTML.h"

BHTML::BHTML(uint8_t _request_timeout, boolean _debug){
    MAX_REQUEST_TIMEOUT = _request_timeout;
    DEBUG = _debug;
    Serial.print(FPSTR(lb_SETUP_REQUEST_TIMEOUT));   Serial.print(_request_timeout * 1/2);  Serial.println(F("\t s"));
    Serial.print(FPSTR(lb_SETUP_DEBUG));    if(DEBUG) Serial.println(FPSTR(lb_YES)); else Serial.println(FPSTR(lb_NO));
}
// Send GET Request and receive respone from server
// Ex html GET: "GET /hostlink/local/file.php?key1=value1&&key2=value2 HTTP/1.1\r\nHost: 192.168.0.3\r\nConnection: close\r\nCache-Control: max-age=0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nUser-Agent: wifi-switch\r\nAccept-Encoding: gzip, deflate, sdch\r\nAccept-Language: vi-VN,vi;q=0.8,fr-FR;q=0.6,fr;q=0.4,en-US;q=0.2,en;q=0.2\r\n\r\n"
String BHTML::sendRequest(char* _server,uint16_t port, String *_tosend){
    String _respone="";
	WiFiClient client3;
	uint8_t _times_out=0;
	if (!client3.connect(_server, port)) {
		if(DEBUG){
            Serial.println(FPSTR(lb_FAILED_CONNECT));
            Serial.println(FPSTR(lb_TRY_RECONNECT));
        };
		delay(500);
		_times_out++;
		if(_times_out>MAX_REQUEST_TIMEOUT){
			if(DEBUG){
                Serial.println(FPSTR(lb_ERROR_CONNECT));
            };
			return FPSTR(lb_TIMEOUT_CONNECT);
		}	
	}; 
    client3.print(*_tosend);
	if(DEBUG){
        Serial.print(F("\n");
        Serial.println(*_tosend);
    };
    delay(200);
	while (client3.available()) { 
		_respone = client3.readString();
    }
	if(DEBUG){
        Serial.println(_respone);
    };
	client3.stop();                                                                                                                                
	return _respone;
}
// Add key, value to GETRequest String
void BHTML::addGETKeyValue(String *_s, String _key,String _val) {
    *_s += "&&" + _key + "=" + _val;
}