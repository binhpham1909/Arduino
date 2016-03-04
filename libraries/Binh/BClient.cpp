#include "BClient.h"

void BClient::config(uint8_t _request_timeout, boolean _debug){
    delayTimeWaitRespone = _request_timeout;
    DEBUG = _debug;
}
// Send GET Request and receive respone from server
// Ex html GET: "GET /hostlink/local/file.php?key1=value1&&key2=value2 HTTP/1.1\r\nHost: 192.168.0.3\r\nConnection: close\r\nCache-Control: max-age=0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nUser-Agent: wifi-switch\r\nAccept-Encoding: gzip, deflate, sdch\r\nAccept-Language: vi-VN,vi;q=0.8,fr-FR;q=0.6,fr;q=0.4,en-US;q=0.2,en;q=0.2\r\n\r\n"
boolean BClient::sendRequest(char* _server,uint16_t port, String *_tosend, String *_respone){
    switch(positionFuncSendRequest){
        case 0:
            respone="";
            if (!client.connect(_server, port)) {
                if(DEBUG)  Serial.println(FPSTR(lb_FAILED_CONNECT));
                return false;
            };
            positionFuncSendRequest++;
            sendSuccess = false;
            break;
        case 1:
            client.print(*_tosend);
            lastCheckTime = millis();
            if(DEBUG){  Serial.print(F("\n"));   Serial.println(*_tosend);   };
            positionFuncSendRequest++;
            sendSuccess = false;
            break;
        case 2:
            if((millis()-lastCheckTime)>delayTimeWaitRespone){
                while (client.available()) { 
                    respone = client.readString();
                    positionFuncSendRequest = 0;
                    sendSuccess = true; 
                }
                *_respone = respone;
                client.stop();
                if(DEBUG&&sendSuccess) Serial.println(respone);          
            }
            break;
    }                                                                                                                      
	return sendSuccess;
}
