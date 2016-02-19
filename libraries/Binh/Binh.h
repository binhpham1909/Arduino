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

// EEPROM
// EEPROM save string need string size + 2 byte, fisrt byte store len of string, second byte store max len of string, after are bytes of string
// EEPROM save boolean, byte need 1 byte
// EEPROM save int need 2 byte
// EEPROM save float, IPAddress need 4 byte
#define MAX_EEPROM_SIZE 512		// Lan khoi dong dau tien

#define MAX_SERIAL_LEN   10	// Serial of device 12         Bytes 20 to 31
#define MAX_SSID_LEN     48
#define MAX_PASSWORD_LEN 32	// Key to control 12           Bytes 32 to 43
#define MAX_STATICIP_LEN 10
#define MAX_KEY_LEN	     10	// SSID Wifi 34 byte           Bytes 44 to 77
#define MAX_ADMIN_LEN    10
#define MAX_SERVER_LEN	 48	// PASSWORD Wifi 34 byte   Bytes 78 to 111

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

// set the EEPROM structure
struct EEPROM_storage {
  uint8_t DEBUG;
  uint8_t FIRST_START;
  uint8_t IS_STATICIP;
  uint8_t WIFI_CONN_TIMEOUT;
 
  char DEVICE_SERIAL[MAX_SERIAL_LEN + 1]; // WIFI ssid + null
  char WF_SSID[MAX_SSID_LEN + 1]; // WIFI ssid + null
  char WF_PASSWORD[MAX_PASSWORD_LEN + 1]; // WiFi password,  if empyt use OPEN, else use AUTO (WEP/WPA/WPA2) + null
  char DEVICE_STATICIP[MAX_STATICIP_LEN + 1]; // staticIP, if empty use DHCP + null
  char DEVICE_KEY[MAX_KEY_LEN + 1]; // WIFI ssid + null
  char DEVICE_ADMIN[MAX_ADMIN_LEN + 1]; // WIFI ssid + null
  char MASTER_SERVER[MAX_SERVER_LEN + 1]; // WIFI ssid + null
} STORAGE;


class ESPHB	// class chua cac ham xu ly cua thu vien
{
	public:
		ESPHB(unsigned char _ledpin);
		void StoreStart(void);	// Set debug mode on or off
		void set_debug(boolean _debug);	// Set debug mode on or off
		boolean StoreString(int address,String value);	// Save String to EEPROM, return 1 if success, 0 if failure
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
		void wifi_reconnect(void);
		void wifi_apmode(void);
		
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
		void LedBlink(unsigned long _interval);
		void LedOn();
		void LedOff();
		boolean Timer(unsigned long *_last_time, unsigned long _interval);
		
		void SerialEvent(void);
	private:
		
		
        String _null="";
        IPAddress _nil=(0,0,0,0);
		boolean DEBUG;
        boolean IPSTATIC;
        boolean FIRSTSTART;
		unsigned char LEDSTATUS;
		String	SERIAL;
		String	SSID;
		String	PASSWORD;
		String	KEY;
		String	ADMIN;
		String	SERVER;
		IPAddress	IP;	// static IP cua thiet bi
		IPAddress	GATEWAY;	// static IP cua thiet bi
		IPAddress	MASK;	// static IP cua thiet bi
        boolean CONNECTED=false;
        boolean	APMODE=false;
        unsigned char	WIFI_CONNECT_TIMEOUT=60;
        const char *APPASSWORD = "hbinvent";
        boolean SERIAL_COMPLETE = false;
        boolean LOGINED = false;
        String	SERIAL_RECEIVER = "";
        String  DEFAULT_KEY = "1234567890";

        const char lb_CONNECTING[] PROGMEM = "Connecting to:";
        const char lb_CONNECTED[] PROGMEM = "Connected ";
        const char lb_FAILED_CONNECT[] PROGMEM = "Failed connect ";
        const char lb_APMODE_START[] PROGMEM = "APmode start";
        const char lb_APMODE_STARTED[] PROGMEM = "APmode started";
        const char lb_SSID[] PROGMEM = "SSID: ";
        const char lb_PASSWORD[] PROGMEM = "PASSWORD: ";
        const char lb_MAC_ADDRESS[] PROGMEM = "MAC ADDRESS: ";
        const char lb_IP[] PROGMEM = "IP ADDRESS: ";
        const char lb_STA[] PROGMEM = "STA ";
        const char lb_AP[] PROGMEM = "AP ";
        const char lb_DOT[] PROGMEM = ".";
        const char lb_LOGIN_FAILED[] PROGMEM = "Enter password for login.";
        const char lb_LOGIN_SUCCESS[] PROGMEM = "You had login lo system.";
        const char lb_SERIAL[] PROGMEM = "Serial: ";
        const char lb_KEY[] PROGMEM = "KEY";
        const char lb_CONNECT_STATUS[] PROGMEM = "Connect status: ";
        const char lb_DEBUG_ENABLE[] PROGMEM = "Enable debug mode";
        const char lb_DEBUG_DISABLE[] PROGMEM = "Disable debug mode";
        const char lb_REBOOT[] PROGMEM = "Reboot";
        const char lb_RESTORE[] PROGMEM = "Restore";
        const char lb_CHANGE_SUCCESS[] PROGMEM = "Success change ";
        const char lb_CHANGE_FAILED[] PROGMEM = "Failed change ";
        const char lb_TO[] PROGMEM = "to ";
        const char lb_ADMIN_PASSWORD[] PROGMEM = "Admin password ";
        const char lb_SERVER[] PROGMEM = "Server ";
        const char lb_STATIC[] PROGMEM = "Static ";
        const char lb_MASK[] PROGMEM = "NetMask ";
        const char lb_GATEWAY[] PROGMEM = "Gateway ";
        const char lb_DHCP_ENABLE[] PROGMEM = "Enable DHCP";
        const char lb_DHCP_DISABLE[] PROGMEM = "Disable DHCP";
        const char lb_COMMAND_ERROR[] PROGMEM = "Command ERROR.";
        
        const char HTTP_header[] PROGMEM = "\r\nConnection: close\r\nCache-Control: max-age=0\r\nAccept: text/html,text/plain\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) coc_coc_browser/50.2.175 Chrome/44.2.2403.175 Safari/537.36\r\nAccept-Encoding: gzip, deflate, sdch\r\nAccept-Language: en-US,en\r\n\r\n";
        const char lb_RESTORE_START[] PROGMEM = "Starting Restore...";
        const char lb_EMPTY_IP_STATIC[] PROGMEM = "Empty static IP, disable DHCP.";
        const char lb_EMPTY_SSID[] PROGMEM = "Clear Wifi SSID.";
        const char lb_EMPTY_PASSWORD[] PROGMEM = "Clear Wifi PASSWORD.";
        
        
        const char lb_DOT[] PROGMEM = ".";
        const char lb_DOT[] PROGMEM = ".";
        
		
		boolean reconnect=false;
		
		
		
		
		unsigned long last_time_request=0;
		unsigned char timesout_request=3;	// times to try connect to host
		unsigned long last_blink=0;
		boolean LedState=LOW;
//		String	HTTP_header_ok="HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json\r\nUser-Agent: Wifi-switch\r\nConnection: close\r\n\r\n";
		String HTTP_notfound="";
		
};
#endif