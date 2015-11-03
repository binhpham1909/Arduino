/*
Libraries of esp8266 connect to wifi, IOT of Pham Huuu Binh
functions include:
public:
boolean wifi_Init(char * STAssid,char * STApassword,char * APssid,char * APpassword);
	- Connect to wifi network with STAssid, STApassword. If time for connect > _timeout_STA then auto switch to 
	AP mode. In AP mode generate wifi network with APssid, APpassword and IP is 192.168.4.1. Return true if in 
	STA mode and false in AP mode.
	
void start_EEPROM(int size);
	- Init EEPROM with size in byte
	
void read_configs(configs *cf,String stSerial,String stKey);
	- Read configs from EEPROM and return in pointer "cf" with: serial, key, ssid, password, ip. "stSerial" is 
	factory serial of device you want set, "stKey" is default factory key connect of device.
	
boolean save_configs(unsigned char content,String value);
	- Save configs to EEPROM

void save_ipstr(String _ip);
	- save IP format String _ip (eg: 192.168.1.8) to EEPROM
	
void save_ipaddr(IPAddress _ip);
	- save IP format IPAddress _ip (eg: _ip(192,168,1,8) to EEPROM
	
void toggle_pin(String pinname, String value);
	- Turn ON, OFF GPIO pin with format: toggle_pin(pinxx,"ON"/"OFF");

void getDecode(request *s, String http_rq);
		void jsonEncode(int pos, String * s, String key, String val);
*/

#ifndef Binh_h
#define Binh_h

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// EEPROM byte:
// 0: bien khoi dong - 0: khoi dong lan dau; 1: khoi dong lan sau
// x0: so ky tu trong chuoi
// 10-21: num byte of SERIAL  (max 11)
// 22-43: num byte of KEY  (max 11)
// 44-77: num byte of ssid  (max 33)
// 78-111: num byte of password (max 33)
// 112-115: num byte of static ip (max 4)
// 116-119: num byte of static gateway (max 4)
// 120-123: num byte of static mask (max 4)
// 124-135: num byte of admin key  (max 11)
// 136-180: num byte of server address  (max 44)

// arrtostr: String var; ga'n: var=arr

// EEPROM
#define _EEPROM_SIZE_ 512		// Lan khoi dong dau tien
#define _FIRST_START_ 0		// Lan khoi dong dau tien
#define _IP_STATIC_ 1		// Chon setup static IP cho thiet bi
#define _DEBUG_ 2		// Chon setup static IP cho thiet bi
#define	_WIFI_CONN_TIMEOUT_	3	// timeout connect to wifi

#define _SERIAL_ 10	// Block dia chi Serial thiet bi 12byte, 1 byte chieu dai, 10 byte noi dung, 1 byte \0
#define _SERIAL_LEN_ 12	// max 11 = 10 byte noi dung + 1 byte \0
#define _KEY_	22
#define _KEY_LEN_ 12	// Block dia chi key thiet bi 12byte, 1 byte chieu dai, 10 byte noi dung, 1 byte \0
#define _SSID_	44
#define _SSID_LEN_ 34	// Block dia chi luu ssid 34byte, 1 byte chieu dai, 32 byte noi dung, 1 byte \0
#define _PASSWORD_	78
#define _PASSWORD_LEN_ 34	// Block dia chi luu password wifi 34byte, 1 byte chieu dai, 32 byte noi dung, 1 byte \0
#define _IP_	112
#define _IP_LEN_ 4	// Block dia chi luu static IP 5 byte, 1 byte chieu dai, 4 byte noi dung
#define _GATEWAY_	116
#define _GATEWAY_LEN_ 4	// Block dia chi luu static gateway 5 byte, 1 byte chieu dai, 4 byte noi dung
#define _MASK_	120
#define _MASK_LEN_ 4	// Block dia chi luu static gateway 5 byte, 1 byte chieu dai, 4 byte noi dung
#define _ADMIN_	124		// Block dia chi luu password admin service 12byte, 1 byte chieu dai, 10 byte noi dung, 1 byte \0
#define _ADMIN_LEN_ 12	
#define _SERVER_	136		// Block dia chi luu server address service 44byte, 1 byte chieu dai, 42 byte noi dung, 1 byte \0
#define _SERVER_LEN_ 44	

// Json encode
#define	ONEJSON	1
#define	FIRSTJSON	2
#define	NEXTJSON	3
#define	LASTJSON	4
// Request encode
#define	ONEGET	1
#define	FIRSTGET	2
#define	NEXTGET	3
#define	LASTGET	4

class ESPHB	// class chua cac ham xu ly cua thu vien
{
	public:
		void StoreStart(void);	// Set debug mode on or off
		void set_debug(boolean _debug);	// Set debug mode on or off
		boolean StoreString(int address,int max_len,String value);	// Save String to EEPROM, return 1 if success, 0 if failure
		void StoreStringIP(int address,String IPvalue);	// Save String IP eg: 192.168.1.10 to EEPROM
		void StoreIP(int address,IPAddress ip_tosave);	// Save IPAddress IP eg: (192,168,1,10) to EEPROM
		void StoreFloat(int address,float value);	// Save float to EEPROM
		boolean StoreSerial(String tosave);
		boolean StoreKey(String tosave);
		boolean StoreSsid(String tosave);
		boolean StorePassword(String tosave);
		boolean StoreAdmin(String tosave);
		boolean StoreServer(String tosave);
		void Restore(void);
		
		boolean ReadSBoolean(int address);
		float ReadSFloat(int address);	// Read float from EEPROM
		void ReadSString(int address,String *value);	// Read String from EEPROM
		void ReadSIP(int address,IPAddress *read);	//	Read IP from EEPROM to String	
		void read_configs(void);
		
		void wifi_connect(void);	// Connect to wifi, return 1 if connected, 0 if fail
		
		void HttpServerEvent(String *request,String *respone);
		boolean CheckArlert(String *request);
		boolean GETRequest(String *_link,String *respone);	// Send GET Request to server
		void jsonEncode(int _position, String * _s, String _key, String _val);		
		void jsonAdd(String *_s, String _key,String _val);
		void GETValue(String *_request,String _key,String *_val);
		void AnalysisValue(String *_request, String _separate, String _end,String _enall, String *_key,String *_val);
		void AddGetRequest(String *_s, String _key,String _val);
		
		void IPtoString(IPAddress _ip,String * _s);
		boolean toggle_pin(String _pinname, String _value);
		unsigned char get_pin(String _pin);
		boolean pin_value(String _value);
		void LedBlink(unsigned char _pin, unsigned long _interval);
		void LedOn(unsigned char _pin);
		void LedOff(unsigned char _pin);
		boolean Timer(unsigned long *_last_time, unsigned long _interval);
		
		void SerialEvent(void);
	private:
		boolean isdebug;
		boolean firststart;
		boolean ipstatic;
		boolean	apmode=false;
		unsigned char	wifi_connect_timeout=60;
		String	serial;
		String	ssid;
		String	password;
		String	key;
		String	admin;
		String	server;
		IPAddress	ip;	// static IP cua thiet bi
		IPAddress	gateway;	// static IP cua thiet bi
		IPAddress	mask;	// static IP cua thiet bi
		String	SerialString = "";
		boolean SerialComplete = false;
		boolean AdminLogin = false;
		boolean connected=false;
		
		unsigned long last_time_request=0;
		unsigned char timesout_request=3;	// times to try connect to host
		unsigned long last_blink=0;
		boolean LedState=LOW;
		String	HTTP_header="\r\nConnection: close\r\nCache-Control: max-age=0\r\nAccept: text/html,text/plain\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) coc_coc_browser/50.2.175 Chrome/44.2.2403.175 Safari/537.36\r\nAccept-Encoding: gzip, deflate, sdch\r\nAccept-Language: en-US,en\r\n\r\n";
//		String	HTTP_header_ok="HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json\r\nUser-Agent: Wifi-switch\r\nConnection: close\r\n\r\n";
		String HTTP_notfound="";
};
#endif