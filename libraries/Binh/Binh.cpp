#include "Binh.h"

//////////////////////////////////////
///             SYSTEM	           ///
//////////////////////////////////////

ESPHB::ESPHB(unsigned char _ledpin){
	LEDSTATUS=_ledpin;
	pinMode(LEDSTATUS, OUTPUT);
};
// Start up
void ESPHB::Startup(void){
    EEPROMRead(0,&DV_INF);
    EEPROMRead(50,&WF_INF);
    if(DV_INF.DEBUG==1){
        DEBUG = true;
    }else{
        DEBUG = false;
    }
    if(DV_INF.FIRSTSTART==0)  
        Restore();  // Set defaults on first startup
 
}
// Save defaults setup to EEPROM (Restore)
void ESPHB::Restore(void){
    EEPROM.begin(_EEPROM_SIZE_);
    for(uint16_t i=MAX_EEPROM_SIZE; i>0; i--){
        EEPROM.write(i-1,0);
    }
    EEPROM.end();
    DV_INF.DEBUG=1;
    DV_INF.FIRST_START=1;    // 1
    StringToArray(&DEFAULT_KEY, DV_INF.DV_ADMIN, MAX_ADMIN_LEN);
    WF_INF.IS_STATICIP = 0;
    WF_INF.WF_CONN_TIMEOUT = 60;
    WF_INF.WF_STATICIP = 0;
    WF_INF.MAX_REQUEST_TIMEOUT = 6;
    WF_INF.MASTER_SERVER_PORT = 80;
    
    StringToArray(&null, WF_INF.WF_SSID, MAX_SSID_LEN);
    StringToArray(&null, WF_INF.WF_PASSWORD, MAX_PASSWORD_LEN);
    StringToArray(&DEFAULT_KEY, WF_INF.WF_KEY, MAX_KEY_LEN);
    StringToArray(&DEFAULT_APPASSWORD, WF_INF.AP_PASSWORD, MAX_PASSWORD_LEN);
    StringToArray(&null, WF_INF.MASTER_SERVER, MAX_SERVER_LEN);
    EEPROMSave(0,&DV_INF);
    EEPROMSave(50,&WF_INF);
    ESP.restart();
}
////////////////////////////////////////
///  Group function Write to EEPROM  ///
////////////////////////////////////////
template <class T> void ESPHB::EEPROMSave(int StartAddress,T *storageVar){
    EEPROM.begin(_EEPROM_SIZE_);
	uint8_t * bytesToWriteEEPROM = (uint8_t *)storageVar;
	const int STORAGE_SIZE = sizeof(*storageVar);
	for (int i = 0; i < STORAGE_SIZE; i++) {
		EEPROM.write(StartAddress + i,bytesToWriteEEPROM[i]);
	}
	EEPROM.commit();
    EEPROM.end();
};
//

//////////////////////////////////////
/// Group function Read to EEPROM  ///
//////////////////////////////////////
template <class T> void ESPHB::EEPROMRead(int StartAddress,T *storageVar){
    EEPROM.begin(_EEPROM_SIZE_);
	uint8_t * bytesToReadEEPROM = (uint8_t *)storageVar;
	const int STORAGE_SIZE = sizeof(*storageVar);
	for (int i = 0; i < STORAGE_SIZE; i++) {
		bytesToReadEEPROM[i] = EEPROM.read(StartAddress + i);
	}
    EEPROM.end();
};

//////////////////////////////////////////
///     Group function for Serial	   ///
//////////////////////////////////////////
void ESPHB::SerialEvent(void) {
	while (Serial.available()){
		char inChar = (char)Serial.read();
		SERIAL_RECEIVER += inChar;
		if (inChar == '\n') {SERIAL_COMPLETE = true;}   
		if(SERIAL_COMPLETE){
			String command=SERIAL_RECEIVER.substring(0,SERIAL_RECEIVER.indexOf("=="));
			String value=SERIAL_RECEIVER.substring(SERIAL_RECEIVER.indexOf("==")+2,SERIAL_RECEIVER.length()-1);
			if((command=="login")&&(!LOGINED)){
				if(value!=String(DV_INF.DV_ADMIN)){
					Serial.println(FPSTR(lb_LOGIN_FAILED));
				}else{
					Serial.println(FPSTR(lb_LOGIN_SUCCESS));
					LOGINED=true;					
				}
			}	
			else if((command=="status")&&LOGINED){
                Serial.print(FPSTR(lb_KEY));    Serial.println(WF_INF.WF_KEY);
				Serial.print(FPSTR(lb_SERIAL)); Serial.println(DV_INF.DV_SERIAL);
				Serial.print(FPSTR(lb_SSID));   Serial.println(WF_INF.WF_SSID);
                Serial.print(FPSTR(lb_PASSWORD));   Serial.println(WF_INF.WF_PASSWORD);
                Serial.print(FPSTR(lb_CONNECT_STATUS)); if(CONNECTED)   Serial.println(FPSTR(lb_CONNECTED)); else   Serial.println(FPSTR(lb_FAILED_CONNECT));
                Serial.print(FPSTR(lb_STA) + FPSTR(lb_MAC_ADDRESS));    Serial.println(WiFi.macAddress(mac));
                Serial.print(FPSTR(lb_STA) + FPSTR(lb_IP));    Serial.println(WiFi.localIP());	
			}
			else if((command=="debug")&&LOGINED){
                DV_INF.DEBUG=true;
				EEPROMSave(0,&DV_INF);
                Serial.println(FPSTR(lb_DEBUG_ENABLE));
				ESP.restart();
			}
			else if((command=="normal")&&LOGINED){
                DV_INF.DEBUG=false;
				EEPROMSave(0,&DV_INF);
                Serial.println(FPSTR(lb_DEBUG_ENABLE));
				ESP.restart();
			}
			else if((command=="reboot")&&LOGINED){
				Serial.println(FPSTR(lb_REBOOT));
				ESP.restart();
			}
			else if((command=="restore")&&LOGINED){
				Serial.println(FPSTR(lb_RESTORE));
				Restore();
				ESP.restart();
			}
			else if((command=="serial")&&LOGINED){
				if(StringToArray(&value, DV_INF.DV_SERIAL, MAX_SERIAL_LEN)){
                    EEPROMSave(0,&DV_INF);
					Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_SERIAL) + FPSTR(lb_TO));
					Serial.println(value);	
				}else{
                    Serial.print(FPSTR(lb_CHANGE_FAILED) + FPSTR(lb_SERIAL) + FPSTR(lb_TO));
					Serial.println(value);
                };
			}
			else if((command=="ssid")&&LOGINED){
				if(StringToArray(&value, WF_INF.WF_SSID, MAX_SSID_LEN)){
                    EEPROMSave(50,&WF_INF);
					Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_SSID) + FPSTR(lb_TO));
					Serial.println(value);	
				}else{
                    Serial.print(FPSTR(lb_CHANGE_FAILED) + FPSTR(lb_SSID) + FPSTR(lb_TO));
					Serial.println(value);
                };
			}
			else if((command=="password")&&LOGINED){
				if(StringToArray(&value, WF_INF.WF_PASSWORD, MAX_PASSWORD_LEN)){
                    EEPROMSave(50,&WF_INF);
					Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_PASSWORD) + FPSTR(lb_TO));
					Serial.println(value);	
				}else{
                    Serial.print(FPSTR(lb_CHANGE_FAILED) + FPSTR(lb_PASSWORD) + FPSTR(lb_TO));
					Serial.println(value);
                };
			}
			else if((command=="key")&&LOGINED){
				if(StringToArray(&value, WF_INF.WF_KEY, MAX_KEY_LEN)){
                    EEPROMSave(50,&WF_INF);
					Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_KEY) + FPSTR(lb_TO));
					Serial.println(value);	
				}else{
                    Serial.print(FPSTR(lb_CHANGE_FAILED) + FPSTR(lb_KEY) + FPSTR(lb_TO));
					Serial.println(value);
                };
			}
			else if((command=="admin")&&LOGINED){
				if(StringToArray(&value, DV_INF.DV_ADMIN, MAX_ADMIN_LEN)){
                    EEPROMSave(0,&DV_INF);
					Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_ADMIN_PASSWORD) + FPSTR(lb_TO));
					Serial.println(value);	
				}else{
                    Serial.print(FPSTR(lb_CHANGE_FAILED) + FPSTR(lb_ADMIN_PASSWORD) + FPSTR(lb_TO));
					Serial.println(value);
                };
			}
			else if((command=="server")&&LOGINED){
				if(StringToArray(&value, WF_INF.MASTER_SERVER, MAX_SERVER_LEN)){
                    EEPROMSave(50,&WF_INF);
					Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_SERVER) + FPSTR(lb_TO));
					Serial.println(value);	
				}else{
                    Serial.print(FPSTR(lb_CHANGE_FAILED) + FPSTR(lb_SERVER) + FPSTR(lb_TO));
					Serial.println(value);
                };
			}
			else if((command=="ip")&&LOGINED){
				WF_INF.WF_STATICIP = StringToIPAdress(String IPvalue);
                EEPROMSave(50,&WF_INF);
				Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_STATIC) + FPSTR(lb_IP) + FPSTR(lb_TO));
				Serial.println(value);	
			}
			else if((command=="DHCP")&&LOGINED){
                if(WF_INF.IS_STATICIP!=1){
                    WF_INF.IS_STATICIP = 1;
                    Serial.println(FPSTR(lb_DHCP_ENABLE));
                }else{
                    WF_INF.IS_STATICIP = 0;
                    Serial.println(FPSTR(lb_DHCP_DISABLE));	
                }
                EEPROMSave(50,&WF_INF);
			}else{
				Serial.println(FPSTR(lb_COMMAND_ERROR));
			}
			SERIAL_RECEIVER="";
			SERIAL_COMPLETE = false;
		}
	}
}

//////////////////////////////////////
///     Group function for WiFi	   ///
//////////////////////////////////////

// Connect to wifi
void ESPHB::wifi_connect(void){
		WiFi.mode(WIFI_STA);
		CONNECTED=false;
		unsigned char timeout=0;	// khởi tạo biến timeout

		if(DEBUG) Serial.println(FPSTR(lb_CONNECTING));
		if(DEBUG) Serial.print(FPSTR(lb_SSID));Serial.println(WF_INF.WF_SSID);
		if(DEBUG) Serial.print(FPSTR(lb_PASSWORD));Serial.println(WF_INF.WF_PASSWORD);	
		WiFi.begin(WF_INF.WF_SSID, WF_INF.WF_PASSWORD);	// kết nối tới mạng wifi
		while (WiFi.status() != WL_CONNECTED) {	// nếu chưa kết nối được
			delay(500); if(DEBUG)    Serial.print(FPSTR(lb_DOT));	// xuất ký tự . mỗi 0.5s
			timeout++;	// tăng timeout
			APMODE=false;
			if(timeout>WF_INF.WIFI_CONNECT_TIMEOUT){	// nếu timeout > thời gian cho phép timeout_STA
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
        if(DEBUG&&CONNECTED)  Serial.print(FPSTR(lb_STA) + FPSTR(lb_MAC_ADDRESS));    Serial.println(WiFi.macAddress(mac));
        if(DEBUG&&CONNECTED)  Serial.print(FPSTR(lb_STA) + FPSTR(lb_IP));    Serial.println(WiFi.localIP());	
};
void ESPHB::wifi_reconnect(void){
	if(!APMODE&&(WiFi.status() != WL_CONNECTED)){
		wifi_connect();
	}
}
void ESPHB::wifi_apmode(void){
	if(APMODE){
		WiFi.mode(WIFI_AP_STA);
		WiFi.softAP(DV_INF.DV_SERIAL, WF_INF.AP_PASSWORD);
        if(DEBUG&&CONNECTED)  Serial.print(FPSTR(lb_AP) + FPSTR(lb_MAC_ADDRESS));    Serial.println(WiFi.softAPmacAddress(mac));
        if(DEBUG&&CONNECTED)  Serial.print(FPSTR(lb_AP) + FPSTR(lb_IP));    Serial.println(WiFi.softAPIP());
	}
}
//////////////////////////////////////////
///     Group function for Utility	   ///
//////////////////////////////////////////
//use: ex: StringToArray(&StringFrom, STORAGE.DEVICE_SERIAL, MAX_SERIAL_LEN);
boolean ESPHB::StringToArray(String *StringFrom, char* arrayTo, int maxlen){
    int _len=StringFrom->length();
    if(maxlen>=_len){
        StringFrom->toCharArray(arrayTo,_len+1);
        return true;
    }else{
        return false;
    }
};

// function to convert IP string to IPAdress 32bit
uint32_t ESPHB::StringToIPAdress(String IPvalue){
	union bytes32{
        uint32_t b32;
        uint8_t b8[4];
    } mybytes32;
	int _start=0;	// vị trí xuất phát String IP
    int _dot=IPvalue.indexOf('.');	// vị trí dấu . đầu tiên
    int _ends=IPvalue.length();	// tổng chiều dài String IP
    for(char k=0;k<4;k++){
        String ipk=IPvalue.substring(_start,_dot);	// octet 1: từ vị trí xuất phát đến dấu . đầu tiên
        b8[k]=(uint8_t)ipk.toInt();	// convert String to Int
        _start=_dot+1;	// dấu . đầu thành điểm xuất phát
        if(k<3){	
			_dot=IPvalue.indexOf('.',_start);}
            else{	// tại octet 4(octet cuối) điểm kết thúc là chiều dài String IP(ends)
              _dot=_ends;};
    }
    return mybytes32.b32;
};
// Analysis command : ?key:value&&key2=value2 HTTP/1
// ?	start
// :	separate
// $$	end
// " "	enall (ex: space char in and of request in html)
void ESPHB::decodeToKeyValue(String *_request, String _separate, String _end,String _enall, String *_key,String *_val){
    int g_start,g_compare,g_end,i;
    g_start = _request->indexOf(*_key); // position start of key
    g_compare = _request->indexOf(_separate,g_start);   // position of charactor separate
    g_end = _request->indexOf(_end,g_compare);  // position end of value
    if(g_end<0){    
      g_end = _request->indexOf(_enall,g_compare);
    }
    if(g_start<0){
      *_val="";
    }else{
      *_val=_request->substring(g_compare+_separate.length(), g_end);
    }
}
//////////////////////////////////////
///     Group function for HTML	   ///
//////////////////////////////////////
// Event when ESP8266 server receive request 
boolean ESPHB::CheckArlert(String *request){
    if((request->indexOf('ALERT_ON'))>0){
        if(DEBUG){Serial.print(alert);};
        return true;       
    }else{
        return false;
    }
}
// Json encode
void ESPHB::jsonEncode(int _position, String * _s, String _key, String _val){	// header http + content
	switch (_position) {
        case ONEJSON:
        case FIRSTJSON:
		//	*_s += HTTP_header_ok;
			*_s += FPSTR(lb_HTTP_200)+FPSTR(lb_JSON_OPEN_BRAKE);
            jsonAdd(_s,_key,_val);
            *_s+= (_position==ONEJSON) ? FPSTR(lb_JSON_CLOSE_BRAKE) : FPSTR(lb_JSON_NEW_LINE);
            break;
        case NEXTJSON:
            jsonAdd(_s,_key,_val);
            *_s+= FPSTR(lb_JSON_NEW_LINE);
            break;
        case LASTJSON:
            jsonAdd(_s,_key,_val);
            *_s+= FPSTR(lb_JSON_CLOSE_BRAKE);
            break;
    };
}
// Add key, value to Json String
void ESPHB::jsonAdd(String *_s, String _key,String _val) {
    *_s += '"' + _key + '"' + ":" + '"' + _val + '"';
}
// Send GET Request and receive respone from server
// Ex html GET: "GET /hostlink/local/file.php?key1=value1&&key2=value2 HTTP/1.1\r\nHost: 192.168.0.3\r\nConnection: close\r\nCache-Control: max-age=0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nUser-Agent: wifi-switch\r\nAccept-Encoding: gzip, deflate, sdch\r\nAccept-Language: vi-VN,vi;q=0.8,fr-FR;q=0.6,fr;q=0.4,en-US;q=0.2,en;q=0.2\r\n\r\n"
boolean ESPHB::sendGETRequest(String *_link,String *respone){
	*respone="";
	WiFiClient client3;
	unsigned char _times_out=0;
	if (!client3.connect(WF_INF.MASTER_SERVER, WF_INF.MASTER_SERVER_PORT)) {
		if(DEBUG){Serial.println(FPSTR(lb_FAILED_CONNECT) + FPSTR(lb_TRY_RECONNECT));};
		delay(500);
		_times_out++;
		if(_times_out>WF_INF.MAX_REQUEST_TIMEOUT){
			if(DEBUG){Serial.println(FPSTR(lb_ERROR_CONNECT));};
			return 0;
		}	
	};
	client3.print(FPSTR(lb_HTTP_GET_PREFIX));	// prefix "GET "
	if(DEBUG){
        Serial.print("\n" + FPSTR(lb_HTTP_GET_PREFIX));
    };
	client3.print(*_link);	// link ex: /hostlink/local/file.php?key1=value1&&key2=value2
    client3.print(FPSTR(lb_SERIAL_KEY_GET));
    client3.print(DV_INF.DV_SERIAL);
	if(DEBUG){
        Serial.print(*_link);
        Serial.print(FPSTR(lb_SERIAL_KEY_GET));
        Serial.print(DV_INF.DV_SERIAL);
    };
	client3.print(FPSTR(lb_HTTP_HEADER_VERSION));
    client3.print(FPSTR(lb_HTTP_HEADER_HOST));  client3.print(WF_INF.MASTER_SERVER);    client3.print(FPSTR(lb_NEWLINE));
    client3.print(FPSTR(lb_HTTP_HEADER_CONNECTION));
    client3.print(FPSTR(lb_HTTP_HEADER_CACHE));
    client3.print(FPSTR(lb_HTTP_HEADER_ACCEPT));
    client3.print(FPSTR(lb_HTTP_HEADER_USER_AGENT));
    client3.print(FPSTR(lb_HTTP_HEADER_ENCODING));
    client3.print(FPSTR(lb_HTTP_HEADER_LANGUE));
    client3.print(FPSTR(lb_NEWLINE));
	if(DEBUG){
        Serial.print(FPSTR(lb_HTTP_HEADER_VERSION));
        Serial.print(FPSTR(lb_HTTP_HEADER_HOST));  Serial.print(WF_INF.MASTER_SERVER);    Serial.print(FPSTR(lb_NEWLINE));
        Serial.print(FPSTR(lb_HTTP_HEADER_CONNECTION));
        Serial.print(FPSTR(lb_HTTP_HEADER_CACHE));
        Serial.print(FPSTR(lb_HTTP_HEADER_ACCEPT));
        Serial.print(FPSTR(lb_HTTP_HEADER_USER_AGENT));
        Serial.print(FPSTR(lb_HTTP_HEADER_ENCODING));
        Serial.print(FPSTR(lb_HTTP_HEADER_LANGUE));
        Serial.print(FPSTR(lb_NEWLINE));
    };
	while (!client3.available()){};
	while (client3.available()) { 
		*respone = client3.readString();
    }
	if(DEBUG){
        Serial.println(*respone);
    };
	client3.stop();                                                                                                                                
	return 1;
}
// Add key, value to GETRequest String
void ESPHB::AddGetRequest(String *_s, String _key,String _val) {
    *_s += "&&" + _key + "=" + _val;
}

// Get value at key in GET Request
void ESPHB::GETValue(String *_request,String _key,String *_val){
	decodeToKeyValue(_request,"=","&&"," ",&_key,_val);
}
// Handler Event when ESP8266 server receive request
void ESPHB::HttpHandlerEvent(String *request,String *respone){
	String cssid,cpassword,ckey,cserver;
	GETValue(request,"key",&ckey);
	GETValue(request,"ssid",&cssid);
	GETValue(request,"password",&cpassword);
	GETValue(request,"server",&cserver);
	if((ckey==String(WF_INF.WF_KEY))&&(cssid.length()<=MAX_SSID_LEN)&&(cpassword.length()<=MAX_PASSWORD_LEN)&&(cserver.length()<=MAX_SERVER_LEN)){
		StringToArray(&value, WF_INF.WF_SSID, MAX_SSID_LEN);
        StringToArray(&value, WF_INF.WF_SSID, MAX_SSID_LEN);
        StringToArray(&value, WF_INF.WF_SSID, MAX_SSID_LEN);
        EEPROMSave(50,&WF_INF);        
		jsonEncode(ONEJSON,respone,"result",FPSTR(lb_CHANGE_SUCCESS)+FPSTR(lb_SSID)+FPSTR(lb_TO)+cssid+FPSTR(lb_NEWLINE)+FPSTR(lb_PASSWORD)+FPSTR(lb_TO)+cpassword+FPSTR(lb_NEWLINE)+FPSTR(lb_SERVER)+FPSTR(lb_TO)+cserver);
		if(DEBUG){
            Serial.println(FPSTR(lb_CHANGE_SUCCESS)+FPSTR(lb_SSID)+FPSTR(lb_TO)+cssid+FPSTR(lb_NEWLINE)+FPSTR(lb_PASSWORD)+FPSTR(lb_TO)+cpassword+FPSTR(lb_NEWLINE)+FPSTR(lb_SERVER)+FPSTR(lb_TO)+cserver);};
	}
}
//////////////////////////////////////
///   Group function for Hardware  ///
//////////////////////////////////////

// Toggle PIN OUT
// Ex: PIN2=ON/OFF
boolean ESPHB::togglePin(String _pinname, String _value){	// toggle pinout
    uint8_t _pinControl = getPin(_pinname);
    digitalWrite(_pinControl, pinValue(_value));
    return digitalRead(_pinControl);
}
// Convert String PINxx to (int)xx (xx: 0-16)
uint8_t ESPHB::getPin(String _pin){	// Get pin name from string to int
	return _pin.substring(3,_pin.length()).toInt();
}
// Convert String ON/OFF to (boolean)HIGH/LOW
boolean ESPHB::pinValue(String _value){	// Get pin value from string to boolean
	if((_value=="ON")||(_value=="on")){
        return HIGH;}
    else{
        if((_value=="OFF")||(_value=="off")){return LOW;};
    } 
}
// Blink led
void ESPHB::LedBlink(unsigned long _interval){
	if(Timer(&last_blink, _interval)){
		if (LedState == LOW)
			LedState = HIGH;  // Note that this switches the LED *off*
		else if(LedState == HIGH)
			LedState = LOW;   // Note that this switches the LED *on*
		digitalWrite(ledpin, LedState);         
    }
}
void ESPHB::LedOn(void){
	digitalWrite(ledpin, HIGH);         
}
void ESPHB::LedOff(void){
	digitalWrite(ledpin, LOW);         
}
boolean ESPHB::Timer(unsigned long *_last_time, unsigned long _interval){
	unsigned long time_now=millis();
	if((time_now-*_last_time)>=_interval){
		*_last_time=time_now;
		return 1;        
    }else{
		return 0;
	}
}