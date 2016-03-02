#include "BWIFI.h"
//////////////////////////////////////
///     Group function for WiFi	   ///
//////////////////////////////////////
void BWIFI::config(boolean _debug){
    DEBUG = _debug;
};
// Connect to wifi
boolean BWIFI::connect(char* _ssid, char* _password){
	CONNECTED=false;
	lastConnect=millis();	// khởi tạo biến timeout
	if(DEBUG) Serial.println(FPSTR(lb_CONNECTING));
	if(DEBUG) {   Serial.print(FPSTR(lb_SSID));   Serial.println(_ssid);}
	if(DEBUG) {   Serial.print(FPSTR(lb_PASSWORD));Serial.println(_password);}	
	WiFi.begin(_ssid, _password);	// kết nối tới mạng wifi
};
boolean BWIFI::checkConnected(void){
	if(WiFi.status() != WL_CONNECTED){
        if(DEBUG) Serial.print(FPSTR(lb_DOT));
        if(DEBUG&&(getConnectTimeOut()>maxTimeOut)) Serial.println(FPSTR(lb_FAILED_CONNECT));
		CONNECTED=false;
	}else{
        CONNECTED=true;
        if(DEBUG) Serial.println(FPSTR(lb_CONNECTED));
    }
    return CONNECTED;
}
void BWIFI::reConnect(char* _ssid, char* _password){
	if(!CONNECTED&&(getConnectTimeOut()>maxTimeOut)){
		connect(_ssid,_password);
	}
}
uint8_t BWIFI::getConnectTimeOut(void){
    return (uint8_t)1000*(millis()-lastConnect);
}
void BWIFI::setSTAMode(void){
    APMODE = false;
	WiFi.mode(WIFI_STA);
    if(DEBUG)   Serial.println(FPSTR(lb_STAMODE));
}
void BWIFI::setAPMode(char* _apssid, char* _appassword){
    APMODE = true;
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(_apssid, _appassword);
    Serial.print(FPSTR(lb_AP));
    Serial.print(FPSTR(lb_MAC_ADDRESS));
    Serial.println(WiFi.softAPmacAddress());
    Serial.print(FPSTR(lb_AP));
    Serial.print(FPSTR(lb_IP));
    Serial.println(WiFi.softAPIP());    
}


