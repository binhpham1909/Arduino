#include "BWIFI.h"
//////////////////////////////////////
///     Group function for WiFi	   ///
//////////////////////////////////////

// Connect to wifi
void ESPHB::connect(char* _ssid, char* _password){
    if(String(WF_INF.WF_SSID)!=""){
		WiFi.mode(WIFI_STA);
		CONNECTED=false;
		unsigned char timeout=0;	// khởi tạo biến timeout

		if(DEBUG) Serial.println(FPSTR(lb_CONNECTING));
		if(DEBUG) {   Serial.print(FPSTR(lb_SSID));   Serial.println(WF_INF.WF_SSID);}
		if(DEBUG) {   Serial.print(FPSTR(lb_PASSWORD));Serial.println(WF_INF.WF_PASSWORD);}	
		WiFi.begin(WF_INF.WF_SSID, WF_INF.WF_PASSWORD);	// kết nối tới mạng wifi
		while (WiFi.status() != WL_CONNECTED) {	// nếu chưa kết nối được
			delay(500); if(DEBUG)    Serial.print(FPSTR(lb_DOT));	// xuất ký tự . mỗi 0.5s
			timeout++;	// tăng timeout
			APMODE=false;
			if(timeout>WF_INF.WF_CONN_TIMEOUT){	// nếu timeout > thời gian cho phép timeout_STA
				APMODE=true;	// bật cờ apmode
				CONNECTED=false;
                if(DEBUG)   Serial.println(FPSTR(lb_FAILED_CONNECT));
                if(DEBUG)   Serial.println(FPSTR(lb_APMODE_START));
                wifi_apmode();
                if(DEBUG)   Serial.println(FPSTR(lb_APMODE_STARTED));
				break;
			}
            CONNECTED=true;
		}
		if(DEBUG&&CONNECTED)  Serial.print(FPSTR(lb_CONNECTED));
        if(DEBUG&&CONNECTED){
            Serial.print(FPSTR(lb_STA));
            Serial.print(FPSTR(lb_MAC_ADDRESS));
            Serial.println(WiFi.macAddress());
        }  
        if(DEBUG&&CONNECTED){
            Serial.print(FPSTR(lb_STA));
            Serial.print(FPSTR(lb_IP));
            Serial.println(WiFi.localIP());
        }         
        
    }else{
        CONNECTED=false;
        APMODE=true;
        wifi_apmode();
        if(DEBUG)   Serial.println(FPSTR(lb_APMODE_STARTED));       
    }
 	
};
void ESPHB::checkConnect(void){
	if(!APMODE&&(WiFi.status() != WL_CONNECTED)){
		wifi_connect();
	}
}
void ESPHB::apmode(void){
	if(APMODE){
		WiFi.mode(WIFI_AP_STA);
		WiFi.softAP(DV_INF.DV_SERIAL, WF_INF.AP_PASSWORD);
        if(DEBUG&&CONNECTED){
            Serial.print(FPSTR(lb_AP));
            Serial.print(FPSTR(lb_MAC_ADDRESS));
            Serial.println(WiFi.softAPmacAddress());
        }
        if(DEBUG&&CONNECTED){
            Serial.print(FPSTR(lb_AP));
            Serial.print(FPSTR(lb_IP));
            Serial.println(WiFi.softAPIP());
        }
	}
}


#include "BHTML.h"

BHTML::BHTML(uint8_t _request_timeout, boolean _debug){
    delayTimeWaitRespone = _request_timeout;
    DEBUG = _debug;
}
// Send GET Request and receive respone from server
// Ex html GET: "GET /hostlink/local/file.php?key1=value1&&key2=value2 HTTP/1.1\r\nHost: 192.168.0.3\r\nConnection: close\r\nCache-Control: max-age=0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nUser-Agent: wifi-switch\r\nAccept-Encoding: gzip, deflate, sdch\r\nAccept-Language: vi-VN,vi;q=0.8,fr-FR;q=0.6,fr;q=0.4,en-US;q=0.2,en;q=0.2\r\n\r\n"
boolean BHTML::sendRequest(char* _server,uint16_t port, String *_tosend, String *_respone){
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
            lastCheckTime = milis();
            if(DEBUG){  Serial.print(F("\n");   Serial.println(*_tosend);   };
            positionFuncSendRequest++;
            sendSuccess = false;
            break;
        case 2:
            if((milis()-lastCheckTime)>delayTimeWaitRespone){
                while (client.available()) { 
                    respone = client.readString();
                }
                *_respone = respone;
                client.stop();
                if(DEBUG&&sendSuccess) Serial.println(respone);
                positionFuncSendRequest = 0;           
            }
            sendSuccess = true;
            break;
    }
    respone = "";                                                                                                                         
	return sendSuccess;
}
// Add key, value to GETRequest String
void BHTML::addGETKeyValue(String *_s, String _key,String _val) {
    *_s += "&&" + _key + "=" + _val;
}