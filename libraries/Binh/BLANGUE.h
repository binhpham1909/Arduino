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

#ifndef BLANGUE_h
#define BLANGUE_h


#include "DHT.h"



const char lb_CONNECTING[] PROGMEM = "Connecting to:";
const char lb_CONNECTED[] PROGMEM = "Connected ";
const char lb_FAILED_CONNECT[] PROGMEM = "Failed connect ";
const char lb_TRY_RECONNECT[] PROGMEM = "Try reconnect ";
const char lb_ERROR_CONNECT[] PROGMEM = "Connect ERROR!";
const char lb_TIMEOUT_CONNECT[] PROGMEM = "Connect timeout!";
const char lb_APMODE_START[] PROGMEM = "APmode start";
const char lb_APMODE_STARTED[] PROGMEM = "APmode started";
const char lb_SSID[] PROGMEM = "SSID ";
const char lb_PASSWORD[] PROGMEM = "PASSWORD ";
const char lb_MAC_ADDRESS[] PROGMEM = "MAC ADDRESS ";
const char lb_IP[] PROGMEM = "IP ADDRESS ";
const char lb_STA[] PROGMEM = "STA ";
const char lb_AP[] PROGMEM = "AP ";
const char lb_DOT[] PROGMEM = ".";
const char lb_LOGIN_FAILED[] PROGMEM = "Enter password for login.";
const char lb_LOGIN_SUCCESS[] PROGMEM = "You had login lo system.";
const char lb_SERIAL[] PROGMEM = "Serial: ";
const char lb_KEY[] PROGMEM = "KEY ";
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
const char lb_SERVER_PORT[] PROGMEM = "Server PORT ";
const char lb_STATIC[] PROGMEM = "Static ";
const char lb_MASK[] PROGMEM = "NetMask ";
const char lb_GATEWAY[] PROGMEM = "Gateway ";
const char lb_DHCP_ENABLE[] PROGMEM = "Enable DHCP";
const char lb_DHCP_DISABLE[] PROGMEM = "Disable DHCP";
const char lb_COMMAND_ERROR[] PROGMEM = "Command ERROR.";
        
const char lb_NEWLINE[] PROGMEM = "\r\n";
const char lb_HTTP_200[] PROGMEM= "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json\r\nUser-Agent: Wifi-switch\r\nConnection: close\r\n\r\n";
const char lb_HTTP_GET_PREFIX[] PROGMEM = "GET ";
const char lb_SERIAL_KEY_GET[] PROGMEM = "&&serial=";
const char lb_HTTP_HEADER_VERSION[] PROGMEM = " HTTP/1.1\r\n";
const char lb_HTTP_HEADER_HOST[] PROGMEM = "Host: ";
const char lb_HTTP_HEADER_CONNECTION[] PROGMEM = "Connection: close\r\nCache-Control: max-age=0\r\nAccept: text/html,text/plain\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) coc_coc_browser/50.2.175 Chrome/44.2.2403.175 Safari/537.36\r\nAccept-Encoding: gzip, deflate, sdch\r\nAccept-Language: en-US,en\r\n\r\n";
        
const char lb_RESTORE_START[] PROGMEM = "Starting Restore...";
const char lb_EMPTY_IP_STATIC[] PROGMEM = "Empty static IP, disable DHCP.";
const char lb_EMPTY_SSID[] PROGMEM = "Clear Wifi SSID.";
const char lb_EMPTY_PASSWORD[] PROGMEM = "Clear Wifi PASSWORD.";
 
const char lb_SETUP_REQUEST_TIMEOUT[] PROGMEM = "Set MAX request timeout to: ";       
const char lb_SETUP_DEBUG[] PROGMEM = "Set DEBUG to: ";
const char lb_YES[] PROGMEM = "YES";
const char lb_NO[] PROGMEM = "NO";
const char lb_EEPROM_READING[] PROGMEM = "Reading EEPROM..";
const char lb_EEPROM_WRITING[] PROGMEM = "Writing EEPROM..";

class ESPHB	// class chua cac ham xu ly cua thu vien
{
	public:
		ESPHB(unsigned char _ledpin);
        
		void Startup(void);
        void Restore(void);
        void SerialEvent(void);

        boolean CheckArlert(String *request);     
        String httpHandlerEvent(String *request);

	private:
        EEPROM_DEVICE DV_INF;
        EEPROM_WIFI WF_INF;
        DHT dht;
        boolean DEBUG = true;
		boolean LOGINED = false;

		boolean SERIAL_COMPLETE = false;


        String	SERIAL_RECEIVER = "";
        String  DEFAULT_KEY = "1234567890";
        String  DEFAULT_APPASSWORD = "hbinvent";
        String  null = "";
               

};
#endif