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
    EEPROMStart();
    DEBUG=EEPROMReadBoolean(_DEBUG_);
    FIRSTSTART=EEPROMReadBoolean(_FIRST_START_);
    IPSTATIC=EEPROMReadBoolean(_IP_STATIC_);
    if(FIRSTSTART)  
        Restore();  // Set defaults on first startup
 
}
// Save defaults setup to EEPROM (Restore)
void ESPHB::Restore(void){
    EEPROMSave(_DEBUG_,true);
    if(DEBUG)   Serial.println(FPSTR(lb_RESTORE_START));
    EEPROM.write(_SERIAL_+1,_SERIAL_MAX_);
    EEPROM.write(_KEY_+1,_KEY_MAX_);
    EEPROM.write(_SSID_+1,_SSID_MAX_);
    EEPROM.write(_PASSWORD_+1,_PASSWORD_MAX_);
    EEPROM.write(_ADMIN_+1,_ADMIN_MAX_);
    EEPROM.write(_SERVER_+1,_SERVER_MAX_);
    if(DEBUG)   Serial.println(FPSTR(lb_EMPTY_IP_STATIC));
    EEPROMSave(_IP_STATIC_,false);
    EEPROMSave(_IP_,_nil);
    EEPROMSave(_GATEWAY_,_nil);
    EEPROMSave(_MASK_,_nil);
    if(DEBUG)   Serial.println(FPSTR(lb_EMPTY_SSID));
    EEPROMSave(_SSID_,_null);
    if(DEBUG)   Serial.println(FPSTR(lb_EMPTY_PASSWORD));
	EEPROMSave(_PASSWORD_,_null);
    if(DEBUG)   Serial.println(FPSTR(lb_EMPTY_PASSWORD));
    EEPROMSave(_SERVER_,_null);
    if(DEBUG)   Serial.println(FPSTR(lb_EMPTY_PASSWORD));
    EEPROMSave(_KEY_,DEFAULT_KEY);
    if(DEBUG)   Serial.println(FPSTR(lb_EMPTY_PASSWORD));
    EEPROMSave(_ADMIN_,DEFAULT_KEY);
    
    EEPROMSave(_FIRST_START_,false);
}

// read config on startup
void ESPHB::read_configs(void){
	if (!ReadSBoolean(_FIRST_START_)) { // write serial
		Serial.print(F("EARSE ROM"));
		for(int i=0;i<_EEPROM_SIZE_;i++){
			EEPROM.write(i,0);
		}
		EEPROM.commit();
		StoreKey("1234567890");
		StoreAdmin("1234567890");
//		EEPROM.write(_WIFI_CONN_TIMEOUT_, 60);	//x*0.5s timeout
		Serial.println("\nUpdated: key: 1234567890");
		EEPROM.write(_FIRST_START_, 1);
		EEPROM.write(_DEBUG_, 1);
		EEPROM.commit();
	}
	DEBUG	= ReadSBoolean(_DEBUG_);
	ipstatic	=	ReadSBoolean(_IP_STATIC_);
//	wifi_connect_timeout =	ReadSBoolean(_WIFI_CONN_TIMEOUT_);
	ReadSString(_SERIAL_,&serial);
	ReadSString(_SSID_,&ssid);
	ReadSString(_PASSWORD_,&password);
	ReadSString(_KEY_,&key);
	ReadSString(_ADMIN_,&admin);
	ReadSString(_SERVER_,&server);
	ReadSIP(_IP_,&ip);
	ReadSIP(_GATEWAY_,&gateway);
	ReadSIP(_MASK_,&mask);
};
////////////////////////////////////////
///  Group function Write to EEPROM  ///
////////////////////////////////////////
void ESPHB::EEPROMStart(void){
	EEPROM.begin(_EEPROM_SIZE_);
};
void ESPHB::EEPROMStop(void){
	EEPROM.end();
};
template <class T> void ESPHB::EEPROMRead(int StartAddress,T *storageVar){
	uint8_t * bytesToReadEEPROM = (uint8_t *)storageVar;
	const int STORAGE_SIZE = sizeof(*storageVar);
	for (int i = 0; i < STORAGE_SIZE; i++) {
		bytesToReadEEPROM[i] = EEPROM.read(StartAddress + i);
	}
};

//Stage
template <class T> void ESPHB::EEPROMWrite(int StartAddress,T *storageVar){
	uint8_t * bytesToWriteEEPROM = (uint8_t *)storageVar;
	const int STORAGE_SIZE = sizeof(*storageVar);
	for (int i = 0; i < STORAGE_SIZE; i++) {
		EEPROM.write(StartAddress + i,bytesToWriteEEPROM[i]);
	}
	EEPROM.commit();
};
//Stage

// save boolean: ()
void ESPHB::EEPROMSave(int address, boolean _value){
	if(_value)
        EEPROM.write(address, 1);
    else
        EEPROM.write(address, 0);
	EEPROM.commit();
};
// save byte: ()
boolean ESPHB::EEPROMSave(int address,byte _byte){
	EEPROM.write(address, _byte);
	EEPROM.commit();
};
// save int
void ESPHB::EEPROMSave(int address,int value){
	union{
		float i;
		unsigned char bytes[2];
	} tosave;
	tosave.i=value;
    EEPROM.write(address, tosave.bytes[0]);
    EEPROM.write(1 + address, tosave.bytes[1]);
	EEPROM.commit();
};
// save float
void ESPHB::EEPROMSave(int address,float value){
	union{
		float f;
		unsigned char bytes[4];
	} tosave;
	tosave.f=value;
	EEPROM.write(address, tosave.bytes[0]);
    EEPROM.write(1 + address, tosave.bytes[1]);
    EEPROM.write(2 + address, tosave.bytes[2]);
    EEPROM.write(3 + address, tosave.bytes[3]);
	EEPROM.commit();
};
// save IPAddress IP ex: (192,168,1,2)
void ESPHB::EEPROMSave(int address,IPAddress ip_tosave){
    EEPROM.write(address, ip_tosave[0]);
    EEPROM.write(address+1, ip_tosave[1]);
    EEPROM.write(address+2, ip_tosave[2]);
    EEPROM.write(address+3, ip_tosave[3]);
	EEPROM.commit();
};
// save string
boolean ESPHB::EEPROMSave(int address,String value){
	int len=value.length();
	int max_len=EEPROM.read(address+1);
	if(max_len>=len)	{
		char arr[len];  // Init an array
		value.toCharArray(arr, len);  // Convert string to array
		EEPROM.write(address, len);
		for(int i=0;i<len;i++){		// write new data
			EEPROM.write(address + 2 + i, arr[i]);
		}
		EEPROM.commit();
		return true;
	}
	else return false;
};
// save String IP ex: 192.168.1.2
void ESPHB::EEPROMSaveStringIP(int address,String IPvalue){
	IPAddress _ip_tosave;	// khởi tạo struct IPAddress để lưu
	int _start=0;	// vị trí xuất phát String IP
    int _dot=IPvalue.indexOf('.');	// vị trí dấu . đầu tiên
    int _ends=IPvalue.length();	// tổng chiều dài String IP
    for(char k=0;k<4;k++){
        String ipk=IPvalue.substring(_start,_dot);	// octet 1: từ vị trí xuất phát đến dấu . đầu tiên
        _ip_tosave[k]=ipk.toInt();	// convert String to Int
        _start=_dot+1;	// dấu . đầu thành điểm xuất phát
        if(k<3){	
			_dot=IPvalue.indexOf('.',_start);}
            else{	// tại octet 4(octet cuối) điểm kết thúc là chiều dài String IP(ends)
              _dot=_ends;};
    }
    EEPROMSave(address,_ip_tosave)
	EEPROM.commit();
};
//////////////////////////////////////
/// Group function Read to EEPROM  ///
//////////////////////////////////////

// read boolean
boolean ESPHB::EEPROMReadBoolean(int address){
	if(EEPROM.read(address)==1){
		return true;
	}else{
		return false;
	};
};

// read float
float ESPHB::EEPROMReadFloat(int address){
	union{
		float f;
		unsigned char bytes[4];
	} toread;
	bytes[0] = EEPROM.read(address);
    bytes[1] = EEPROM.read(address + 1);
    bytes[2] = EEPROM.read(address + 2);
    bytes[3] = EEPROM.read(address + 3);
	return toread.f;
};
// read IP
void ESPHB::EEPROMReadIP(int address,IPAddress *read){
	read[0] = EEPROM.read(address);
    read[1] = EEPROM.read(address + 1);
    read[2] = EEPROM.read(address + 2);
    read[3] = EEPROM.read(address + 3);
};
// read string
void ESPHB::EEPROMReadString(int address,String *value){
	*value="";
	int len=EEPROM.read(address);
	for(int i=0;i<len;i++){	// Ingrone \0 at end Char array
		*value+=char(EEPROM.read(i + address + 2));
	}
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
				if(value!=(ADMIN)){
					Serial.println(FPSTR(lb_LOGIN_FAILED));
				}else{
					Serial.println(FPSTR(lb_LOGIN_SUCCESS));
					LOGINED=true;					
				}
			}	
			else if((command=="status")&&LOGINED){
                Serial.println(FPSTR(lb_KEY) + KEY);
				Serial.println(FPSTR(lb_SERIAL) + SERIAL);
				Serial.println(FPSTR(lb_SSID) + SSID);
                Serial.println(FPSTR(lb_PASSWORD) + PASSWORD);
                Serial.print(FPSTR(lb_CONNECT_STATUS)); if(CONNECTED)   Serial.println(FPSTR(lb_CONNECTED)); else   Serial.println(FPSTR(lb_FAILED_CONNECT));
                Serial.print(FPSTR(lb_STA) + FPSTR(lb_MAC_ADDRESS));    Serial.println(WiFi.macAddress(mac));
                Serial.print(FPSTR(lb_STA) + FPSTR(lb_IP));    Serial.println(WiFi.localIP());	
			}
			else if((command=="debug")&&LOGINED){
				Serial.println(FPSTR(lb_DEBUG_ENABLE));
				EEPROMSave(_DEBUG_, true);
				ESP.restart();
			}
			else if((command=="normal")&&LOGINED){
				Serial.println(FPSTR(lb_DEBUG_DISABLE));
                EEPROMSave(_DEBUG_, false);
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
				if(EEPROMSave(_SERIAL_,value)){
					Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_SERIAL) + FPSTR(lb_TO));
					Serial.println(value);	
				}else{
                    Serial.print(FPSTR(lb_CHANGE_FAILED) + FPSTR(lb_SERIAL) + FPSTR(lb_TO));
					Serial.println(value);
                };
			}
			else if((command=="ssid")&&LOGINED){
				if(EEPROMSave(_SSID_,value)){
					Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_SSID) + FPSTR(lb_TO));
					Serial.println(value);	
				}else{
                    Serial.print(FPSTR(lb_CHANGE_FAILED) + FPSTR(lb_SSID) + FPSTR(lb_TO));
					Serial.println(value);
                };
			}
			else if((command=="password")&&LOGINED){
				if(EEPROMSave(_PASSWORD_,value)){
					Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_PASSWORD) + FPSTR(lb_TO));
					Serial.println(value);	
				}else{
                    Serial.print(FPSTR(lb_CHANGE_FAILED) + FPSTR(lb_PASSWORD) + FPSTR(lb_TO));
					Serial.println(value);
                };
			}
			else if((command=="key")&&LOGINED){
				if(EEPROMSave(_KEY_,value)){
					Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_KEY) + FPSTR(lb_TO));
					Serial.println(value);	
				}else{
                    Serial.print(FPSTR(lb_CHANGE_FAILED) + FPSTR(lb_KEY) + FPSTR(lb_TO));
					Serial.println(value);
                };
			}
			else if((command=="admin")&&LOGINED){
				if(EEPROMSave(_ADMIN_,value)){
					Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_ADMIN_PASSWORD) + FPSTR(lb_TO));
					Serial.println(value);	
				}else{
                    Serial.print(FPSTR(lb_CHANGE_FAILED) + FPSTR(lb_ADMIN_PASSWORD) + FPSTR(lb_TO));
					Serial.println(value);
                };
			}
			else if((command=="server")&&LOGINED){
				if(EEPROMSave(_SERVER_,value)){
					Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_SERVER) + FPSTR(lb_TO));
					Serial.println(value);	
				}else{
                    Serial.print(FPSTR(lb_CHANGE_FAILED) + FPSTR(lb_SERVER) + FPSTR(lb_TO));
					Serial.println(value);
                };
			}
			else if((command=="ip")&&LOGINED){
				EEPROMSaveStringIP(_IP_,value);
				Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_STATIC) + FPSTR(lb_IP) + FPSTR(lb_TO));
				Serial.println(value);	
			}
			else if((command=="mask")&&LOGINED){
                EEPROMSaveStringIP(_MASK_,value);
				Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_STATIC) + FPSTR(lb_MASK) + FPSTR(lb_TO));
				Serial.println(value);	
			}
			else if((command=="gateway")&&LOGINED){
                EEPROMSaveStringIP(_GATEWAY_,value);
				Serial.print(FPSTR(lb_CHANGE_SUCCESS) + FPSTR(lb_STATIC) + FPSTR(lb_GATEWAY) + FPSTR(lb_TO));
				Serial.println(value);
			}
			else if((command=="DHCP")&&LOGINED){
                EEPROMSave(_IP_STATIC_, !IPSTATIC);
				IPSTATIC=!IPSTATIC;
				if(IPSTATIC){
					Serial.println(FPSTR(lb_DHCP_ENABLE));
				}else{
					Serial.println(FPSTR(lb_DHCP_DISABLE));				
				}
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
		char sta_ssid[SSID.length()+1];	// chuyển đổi String ssid, password sang char array
		char sta_password[PASSWORD.length()+1];
		SSID.toCharArray(sta_ssid, SSID.length()+1);
		PASSWORD.toCharArray(sta_password, PASSWORD.length()+1);
		if(DEBUG) Serial.println(FPSTR(lb_CONNECTING));
		if(DEBUG) Serial.print(FPSTR(lb_SSID));Serial.println(sta_ssid);
		if(DEBUG) Serial.print(FPSTR(lb_PASSWORD));Serial.println(sta_password);	
		WiFi.begin(sta_ssid, sta_password);	// kết nối tới mạng wifi
		while (WiFi.status() != WL_CONNECTED) {	// nếu chưa kết nối được
			delay(500); if(DEBUG)    Serial.print(FPSTR(lb_DOT));	// xuất ký tự . mỗi 0.5s
			timeout++;	// tăng timeout
			APMODE=false;
			if(timeout>WIFI_CONNECT_TIMEOUT){	// nếu timeout > thời gian cho phép timeout_STA
				APMODE=true;	// bật cờ apmode
				CONNECTED=false;
                if(DEBUG)   Serial.println(FPSTR(lb_FAILED_CONNECT) + ssid);
                if(DEBUG)   Serial.println(FPSTR(lb_APMODE_START));
                wifi_apmode();
                if(DEBUG)   Serial.println(FPSTR(lb_APMODE_STARTED));
				break;
			}
            CONNECTED=true;
		}
		if(DEBUG&&CONNECTED)  Serial.print(FPSTR(lb_CONNECTED) + ssid);
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
		char len=SERIAL.length()+1;
		char apssid[len];	// chuyển đổi String ssid, password sang char array
		SERIAL.toCharArray(apssid,len);
		WiFi.mode(WIFI_AP_STA);
		WiFi.softAP(apssid, APPASSWORD);
        if(DEBUG&&CONNECTED)  Serial.print(FPSTR(lb_AP) + FPSTR(lb_MAC_ADDRESS));    Serial.println(WiFi.softAPmacAddress(mac));
        if(DEBUG&&CONNECTED)  Serial.print(FPSTR(lb_AP) + FPSTR(lb_IP));    Serial.println(WiFi.softAPIP());
	}
}

//////////////////////////////////////
///     Group function for HTML	   ///
//////////////////////////////////////

// Event when ESP8266 server receive request
void ESPHB::HttpServerEvent(String *request,String *respone){
	String cssid,cpassword,ckey,cserver;
	GETValue(request,"key",&ckey);
	GETValue(request,"ssid",&cssid);
	GETValue(request,"password",&cpassword);
	GETValue(request,"server",&cserver);
	if((ckey==key)&&(cssid.length()>4)&&(cpassword.length()>4)&&(cserver.length()>4)){
			StoreSsid(cssid);
			StorePassword(cpassword);
			StoreServer(cserver);
			jsonEncode(ONEJSON,respone,"result","succes change ssid to: "+cssid+"\n password to: "+cpassword+"\n server to: "+cserver);
			if(DEBUG){Serial.println("succes change ssid to: "+cssid+"\n password to: "+cpassword+"\n server to: "+cserver);};
	}
}
// Event when ESP8266 server receive request 
boolean ESPHB::CheckArlert(String *request){
	String alert,key="alert";
	GETValue(request,"alert",&alert);
	AnalysisValue(request,"==","&&","&&",&key,&alert);
	if(DEBUG){Serial.print(alert);};
	if(alert=="ON")	{return true;} else{return false;}
}
// Send GET Request and receive respone from server
// Ex html GET: "GET /hostlink/local/file.php?key1=value1&&key2=value2 HTTP/1.1\r\nHost: 192.168.0.3\r\nConnection: close\r\nCache-Control: max-age=0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nUser-Agent: wifi-switch\r\nAccept-Encoding: gzip, deflate, sdch\r\nAccept-Language: vi-VN,vi;q=0.8,fr-FR;q=0.6,fr;q=0.4,en-US;q=0.2,en;q=0.2\r\n\r\n"
boolean ESPHB::GETRequest(String *_link,String *respone){
	char _len = server.length() + 1;
	char _host[_len];
	server.toCharArray(_host, _len);
	*respone="";
	WiFiClient client3;
	unsigned char _times_out=0;
	if (!client3.connect(_host, 80)) {
		if(DEBUG){Serial.println("Connect failed, try reconnect");};
		delay(500);
		_times_out++;
		if(_times_out>timesout_request){
			if(DEBUG){Serial.println("Connect Error");};
			return 0;
		}	
	};
	client3.print("GET ");	// prefix "GET "
	if(DEBUG){Serial.print("\nGET ");};
	client3.print(*_link+"&&serial="+serial);	// link ex: /hostlink/local/file.php?key1=value1&&key2=value2
	if(DEBUG){Serial.print(*_link+"&&serial="+serial);};
	client3.print(" HTTP/1.1\r\nHost: "+server);
	if(DEBUG){Serial.print(" HTTP/1.1\r\nHost: "+server);};
	client3.print(HTTP_header);
	if(DEBUG){Serial.print(HTTP_header);};
	while (!client3.available()){};
	while (client3.available()) { 
		*respone = client3.readString();
    }
	if(DEBUG){Serial.println(*respone);};
	client3.stop();                                                                                                                                
	return 1;
}

// Json encode
void ESPHB::jsonEncode(int _position, String * _s, String _key, String _val){	// header http + content
	switch (_position) {
        case ONEJSON:
        case FIRSTJSON:
		//	*_s += HTTP_header_ok;
			*_s += String(F("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json\r\nUser-Agent: Wifi-switch\r\nConnection: close\r\n\r\n"));
            *_s += "{\r\n";
            jsonAdd(_s,_key,_val);
            *_s+= (_position==ONEJSON) ? "\r\n}" : ",\r\n";
            break;
        case NEXTJSON:
            jsonAdd(_s,_key,_val);
            *_s+= ",\r\n";
            break;
        case LASTJSON:
            jsonAdd(_s,_key,_val);
            *_s+= "\r\n}";
            break;
    };
}
// Add key, value to Json String
void ESPHB::jsonAdd(String *_s, String _key,String _val) {
    *_s += '"' + _key + '"' + ":" + '"' + _val + '"';
}
// Get value at key in GET Request
void ESPHB::GETValue(String *_request,String _key,String *_val){
	AnalysisValue(_request,"=","&&"," ",&_key,_val);
}
// Analysis command : ?key:value&&key2=value2 HTTP/1
// ?	start
// :	separate
// $$	end
// " "	enall (ex: space char in and of request in html)
void ESPHB::AnalysisValue(String *_request, String _separate, String _end,String _enall, String *_key,String *_val){
    int g_start,g_compare,g_end,i;
    g_start = _request->indexOf(*_key);
    g_compare = _request->indexOf(_separate,g_start);
    g_end = _request->indexOf(_end,g_compare);
    if(g_end<0){
      g_end = _request->indexOf(_enall,g_compare);
    }
    if(g_start<0){
      *_val="";
    }else{
      *_val=_request->substring(g_compare+_separate.length(), g_end);
    }
}
// Add key, value to GETRequest String
void ESPHB::AddGetRequest(String *_s, String _key,String _val) {
    *_s += "&&" + _key + "=" + _val;
}

//////////////////////////////////////////
///     Group function for Utility	   ///
//////////////////////////////////////////

// Convert IP to String
void ESPHB::IPtoString(IPAddress _ip,String * _s){	// chuyển đổi IP dạng struct IPAddress sang String
	*_s="";
	*_s=String(_ip[0],DEC)+"."+String(_ip[1],DEC)+"."+String(_ip[2],DEC)+"."+String(_ip[3],DEC); // build String IP from array 4 bytes c_ip[].
}
// Toggle PIN OUT
// Ex: PIN2=ON/OFF
boolean ESPHB::toggle_pin(String _pinname, String _value){	// toggle pinout
  digitalWrite(get_pin(_pinname), pin_value(_value));
  return digitalRead(get_pin(_pinname));
}
// Convert String PINxx to (int)xx (xx: 0-16)
unsigned char ESPHB::get_pin(String _pin){	// Get pin name from string to int
	return _pin.substring(3,_pin.length()).toInt();
}
// Convert String ON/OFF to (boolean)HIGH/LOW
boolean ESPHB::pin_value(String _value){	// Get pin value from string to boolean
	if(_value=="ON"){return HIGH;}else if(_value=="OFF"){return LOW;};
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


