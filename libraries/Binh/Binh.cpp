#include "Binh.h"
////////////////////////////////////////
///  Group function Write to EEPROM  ///
////////////////////////////////////////

void ESPHB::StoreStart(void){
	EEPROM.begin(_EEPROM_SIZE_);
};
// Save debug setup
void ESPHB::set_debug(boolean _debug){
	if(_debug){EEPROM.write(_DEBUG_, 1);}
	else{EEPROM.write(_DEBUG_, 0);};
	EEPROM.commit();
}
// save string
boolean ESPHB::StoreString(int address,int max_len,String value){
	int len=value.length()+1;
	if(max_len>=len)	{
		char arr[len];  // Init an array
		value.toCharArray(arr, len);  // Convert string to array
		EEPROM.write(address, len);
		for(int i=0;i<len;i++){		// write new data
			EEPROM.write(address + 1 + i, arr[i]);
		}
		EEPROM.commit();
		return true;
	}
	else return false;
};
// save String IP ex: 192.168.1.2
void ESPHB::StoreStringIP(int address,String IPvalue){
	IPAddress _ip_tosave;	// khởi tạo struct IPAddress để lưu
	int _start=0;	// vị trí xuất phát String IP
    int _dot=IPvalue.indexOf('.');	// vị trí dấu . đầu tiên
    int _ends=IPvalue.length();	// tổng chiều dài String IP
    for(char k=0;k<4;k++){
        String ipk=IPvalue.substring(_start,_dot);	// octet 1: từ vị trí xuất phát đến dấu . đầu tiên
        _ip_tosave[k]=ipk.toInt();	// convert String to Int
		EEPROM.write(address+k, _ip_tosave[k]);
        _start=_dot+1;	// dấu . đầu thành điểm xuất phát
        if(k<3){	
			_dot=IPvalue.indexOf('.',_start);}
            else{	// tại octet 4(octet cuối) điểm kết thúc là chiều dài String IP(ends)
              _dot=_ends;};
    }
	EEPROM.commit();
};
// save IPAddress IP ex: (192,168,1,2)
void ESPHB::StoreIP(int address,IPAddress ip_tosave){
    for(char k=0;k<4;k++){
		EEPROM.write(address+k, ip_tosave[k]);
    }
	EEPROM.commit();
};
// save float
void ESPHB::StoreFloat(int address,float value){
	union{
		float f;
		unsigned char bytes[4];
	} tosave;
	tosave.f=value;
	EEPROM.write(address, 4);
	for(int i=0;i<4;i++){
		EEPROM.write(i + address, tosave.bytes[i]);
	}
	EEPROM.commit();
};

// save serial
boolean ESPHB::StoreSerial(String tosave){
	return StoreString(_SERIAL_,_SERIAL_LEN_,tosave);
}
// save KEY
boolean ESPHB::StoreKey(String tosave){
	return StoreString(_KEY_,_KEY_LEN_,tosave);
}
// save ssid
boolean ESPHB::StoreSsid(String tosave){
	return StoreString(_SSID_,_SSID_LEN_,tosave);
}
// save password
boolean ESPHB::StorePassword(String tosave){
	return StoreString(_PASSWORD_,_PASSWORD_LEN_,tosave);
}
// save admin password
boolean ESPHB::StoreAdmin(String tosave){
	return StoreString(_ADMIN_,_ADMIN_LEN_,tosave);
}
// save server address
boolean ESPHB::StoreServer(String tosave){
	return StoreString(_SERVER_,_SERVER_LEN_,tosave);
}
// Save defaults setup to EEPROM (Restore)
void ESPHB::Restore(void){
	String _null="";
	IPAddress _nil=(0,0,0,0);
	StoreIP(_IP_,_nil);
	StoreIP(_GATEWAY_,_nil);
	StoreIP(_MASK_,_nil);
	EEPROM.write(_FIRST_START_, 0);
	EEPROM.write(_IP_STATIC_, 0);
	EEPROM.commit();
	StoreSsid(_null);
	StorePassword(_null);
}
//////////////////////////////////////
/// Group function Read to EEPROM  ///
//////////////////////////////////////

// read boolean
boolean ESPHB::ReadSBoolean(int address){
	if(EEPROM.read(address)==1){
		return 1;
	}else{
		return 0;
	};
};
// read float
float ESPHB::ReadSFloat(int address){
	union{
		float f;
		unsigned char bytes[4];
	} toread;
	for(int i=0;i<4;i++){
		EEPROM.read(i + address);
	}
	return toread.f;
};
// read string
void ESPHB::ReadSString(int address,String *value){
	*value="";
	int len=EEPROM.read(address);
	for(int i=0;i<len-1;i++){	// Ingrone \0 at end Char array
		*value+=char(EEPROM.read(i + address + 1));
	}
};
// read IP
void ESPHB::ReadSIP(int address,IPAddress *read){
	for(int i=0;i<4;i++){
		read[i]=EEPROM.read(i + address);
	}
};

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
	isdebug	= ReadSBoolean(_DEBUG_);
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

//////////////////////////////////////
///     Group function for WiFi	   ///
//////////////////////////////////////

// Connect to wifi
void ESPHB::wifi_connect(void){
		connected=false;
		unsigned char timeout=0;	// khởi tạo biến timeout
		char sta_ssid[ssid.length()+1];	// chuyển đổi String ssid, password sang char array
		char sta_password[password.length()+1];
		ssid.toCharArray(sta_ssid, ssid.length()+1);
		password.toCharArray(sta_password, password.length()+1);
		sta_ssid[ssid.length()+1]=0;
		sta_password[password.length()+1]=0;
		if(isdebug){Serial.println(F("Connecting to "));};
		if(isdebug){Serial.print(F("SSID: "));Serial.println(sta_ssid);};
		if(isdebug){Serial.print(F("PASSWORD: "));Serial.println(sta_password);};	
		WiFi.begin(sta_ssid, sta_password);	// kết nối tới mạng wifi
		while (WiFi.status() != WL_CONNECTED) {	// nếu chưa kết nối được
			delay(500); if(isdebug){Serial.print(".");}	// xuất ký tự . mỗi 0.5s
			timeout++;	// tăng timeout
			connected=true;
			apmode=false;
			if(timeout>wifi_connect_timeout){	// nếu timeout > thời gian cho phép timeout_STA
				apmode=true;	// bật cờ apmode
				connected=false;
				break;
			}
		}
		if(isdebug&&connected){Serial.print("Connected to "+ssid);};
		if(isdebug&&!connected){Serial.print("Failed connect to "+ssid);};
};

//////////////////////////////////////
///     Group function for HTML	   ///
//////////////////////////////////////

// Event when ESP8266 server receive request
void ESPHB::HttpServerEvent(String *request,String *respone){
	String command,ckey;
	GETValue(request,"key",&ckey);
	GETValue(request,"command",&command);
	if(ckey==key){
		if(command=="status"){	// Add status content at here
//			jsonEncode(ONEJSON,respone,"status","online");
		}
	}
}
// Event when ESP8266 server receive request 
boolean ESPHB::CheckArlert(String *request){
	String alert,key="alert";
	GETValue(request,"alert",&alert);
	AnalysisValue(request,"==","&&","&&",&key,&alert);
	if(isdebug){Serial.print(alert);};
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
		if(isdebug){Serial.println("Connect failed, try reconnect");};
		delay(500);
		_times_out++;
		if(_times_out>timesout_request){
			if(isdebug){Serial.println("Connect Error");};
			return 0;
		}	
	};
	client3.print("GET ");	// prefix "GET "
	if(isdebug){Serial.print("\nGET ");};
	client3.print(*_link+"&&serial="+serial);	// link ex: /hostlink/local/file.php?key1=value1&&key2=value2
	if(isdebug){Serial.print(*_link+"&&serial="+serial);};
	client3.print(" HTTP/1.1\r\nHost: "+server);
	if(isdebug){Serial.print(" HTTP/1.1\r\nHost: "+server);};
	client3.print(HTTP_header);
	if(isdebug){Serial.print(HTTP_header);};
	while (!client3.available()){};
	while (client3.available()) { 
		*respone = client3.readString();
    }
	if(isdebug){Serial.println(*respone);};
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
void ESPHB::LedBlink(unsigned char _pin, unsigned long _interval){
	if(Timer(&last_blink, _interval)){
		if (LedState == LOW)
			LedState = HIGH;  // Note that this switches the LED *off*
		else if(LedState == HIGH)
			LedState = LOW;   // Note that this switches the LED *on*
		digitalWrite(_pin, LedState);         
    }
}
void ESPHB::LedOn(unsigned char _pin){
	digitalWrite(_pin, HIGH);         
}
void ESPHB::LedOff(unsigned char _pin){
	digitalWrite(_pin, LOW);         
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

//////////////////////////////////////////
///     Group function for Serial	   ///
//////////////////////////////////////////

void ESPHB::SerialEvent(void) {
	while (Serial.available()){
		char inChar = (char)Serial.read();
		SerialString += inChar;
		if (inChar == '\n') {SerialComplete = true;}   
		if(SerialComplete){
			String command=SerialString.substring(0,SerialString.indexOf("=="));
			String value=SerialString.substring(SerialString.indexOf("==")+2,SerialString.length()-1);
			if((command=="login")&&(!AdminLogin)){
				if(value!=(admin)){
					Serial.println(F("Enter password for login."));
				}else{
					Serial.println(F("You had login lo system"));
					AdminLogin=true;					
				}
			}	
			else if((command=="status")&&AdminLogin){
				Serial.print(F("Serial "));Serial.println(serial);
				Serial.print(F("SSID "));Serial.println(ssid);
				Serial.print(F("Password "));Serial.println(password);
				Serial.print(F("KEY "));Serial.println(key);
				Serial.print(F("IP "));Serial.println(ip);
				Serial.print(F("Connect status "));Serial.println(connected);
				Serial.print("Now ");Serial.print(F("IP "));Serial.println(WiFi.localIP());
				Serial.print(F("Gateway "));Serial.println(gateway);
				Serial.print("Now ");Serial.print(F("Gateway "));Serial.println(WiFi.gatewayIP());
				Serial.print(F("Mask "));Serial.println(mask);
				Serial.print("Now ");Serial.print(F("Mask "));Serial.println(WiFi.subnetMask());
			}
			else if((command=="debug")&&AdminLogin){
				Serial.println(F("Enable debug mode"));
				set_debug(true);
				ESP.restart();
			}
			else if((command=="normal")&&AdminLogin){
				Serial.println(F("Disable debug mode"));
				set_debug(false);
				ESP.restart();
			}
			else if((command=="reboot")&&AdminLogin){
				Serial.println(F("Reboot"));
				ESP.restart();
			}
			else if((command=="restore")&&AdminLogin){
				AdminLogin=false;
				Serial.println(F("Restoring..."));
				Restore();
				ESP.restart();
			}
			else if((command=="serial")&&AdminLogin){
				if(StoreSerial(value)){
					Serial.print(F("Success change "));
					Serial.print(F("Serial "));
					Serial.print(F("to "));
					Serial.println(value);	
				}else{Serial.println(F("Error"));};
			}
			else if((command=="ssid")&&AdminLogin){
				if(StoreSsid(value)){
					Serial.print(F("Success change "));
					Serial.print(F("SSID "));
					Serial.print(F("to "));
					Serial.println(value);	
				}else{Serial.println(F("Error"));};
			}
			else if((command=="password")&&AdminLogin){
				if(StorePassword(value)){
					Serial.print(F("Success change "));
					Serial.print(F("Password "));
					Serial.print(F("to "));
					Serial.println(value);	
				}else{Serial.println(F("Error"));};
			}
			else if((command=="key")&&AdminLogin){
				if(StoreKey(value)){
					Serial.print(F("Success change "));
					Serial.print(F("KEY "));
					Serial.print(F("to "));
					Serial.println(value);	
				}else{Serial.println(F("Error"));};
			}
			else if((command=="admin")&&AdminLogin){
				if(StoreAdmin(value)){
					Serial.print(F("Success change "));
					Serial.print(F("Admin password "));
					Serial.print(F("to "));
					Serial.println(value);	
				}else{Serial.println(F("Error"));};
			}
			else if((command=="server")&&AdminLogin){
				if(StoreServer(value)){
					Serial.print(F("Success change "));
					Serial.print(F("Server "));
					Serial.print(F("to "));
					Serial.println(value);	
				}else{Serial.println(F("Error"));};
			}
			else if((command=="ip")&&AdminLogin){
				StoreStringIP(_IP_,value);
				Serial.print(F("Success change "));
				Serial.print(F("IP "));
				Serial.print(F("to "));
				Serial.println(value);	
			}
			else if((command=="mask")&&AdminLogin){
				StoreStringIP(_MASK_,value);
				Serial.print(F("Success change "));
				Serial.print(F("Mask "));
				Serial.print(F("to "));
				Serial.println(value);	
			}
			else if((command=="gateway")&&AdminLogin){
				StoreStringIP(_GATEWAY_,value);
				Serial.print(F("Success change "));
				Serial.print(F("Gateway "));
				Serial.print(F("to "));
				Serial.println(value);	
			}
			else if((command=="DHCP")&&AdminLogin){
				EEPROM.write(_IP_STATIC_, !ipstatic);
				EEPROM.commit();
				ipstatic=!ipstatic;
				if(ipstatic){
					Serial.print(F("Enable"));
					Serial.println(F("DHCP "));
				}else{
					Serial.print(F("Disable"));
					Serial.println(F("DHCP "));					
				}
			}else{
				Serial.print(F("Command ERROR"));
			}
			SerialString="";
			SerialComplete = false;
		}
	}
}
