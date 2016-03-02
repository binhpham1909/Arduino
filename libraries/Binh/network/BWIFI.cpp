#include "BWIFI.h"
//////////////////////////////////////
///     Group function for WiFi	   ///
//////////////////////////////////////

// Connect to wifi
boolean BWIFI::connect(char* _ssid, char* _password){
    if(String(WF_INF.WF_SSID)!=""){
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
 	return CONNECTED;
};
boolean BWIFI::checkConnect(void){
	if(!APMODE&&(WiFi.status() != WL_CONNECTED)){
		CONNECTED=false;
	}else{
        CONNECTED=true;
    }
    return CONNECTED;
}
void BWIFI::setSTAMode(void){
	WiFi.mode(WIFI_STA);
}
void BWIFI::setAPMode(void){
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


