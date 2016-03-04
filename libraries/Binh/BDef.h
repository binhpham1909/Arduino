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

#ifndef BDef_h
#define BDef_h

#include <PGMSPACE.h>
#include "DHT.h"

#define MAX_ADMIN_LEN    10
#define MAX_SERIAL_LEN   10	

#define MAX_SSID_LEN     32
#define MAX_PASSWORD_LEN 32	// Key to control 12           Bytes 32 to 43
#define MAX_STATICIP_LEN 10
#define MAX_KEY_LEN	     10	// SSID Wifi 34 byte           Bytes 44 to 77
#define MAX_SERVER_LEN	 48	// PASSWORD Wifi 34 byte   Bytes 78 to 111


const char lb_CONNECTING[] PROGMEM = "Connecting to:";
const char lb_CONNECTED[] PROGMEM = "Connected ";
const char lb_SSID[] PROGMEM = "SSID ";
const char lb_PASSWORD[] PROGMEM = "PASSWORD ";
const char lb_AP[] PROGMEM = "AP ";
const char lb_APMODE_START[] PROGMEM = "APmode start";
const char lb_APMODE_STARTED[] PROGMEM = "APmode started";
const char lb_FAILED_CONNECT[] PROGMEM = "Failed connect ";
const char lb_DOT[] PROGMEM = ".";
const char lb_STA[] PROGMEM = "STA ";
const char lb_STAMODE[] PROGMEM = "STA Mode";
const char lb_MAC_ADDRESS[] PROGMEM = "MAC ADDRESS ";
const char lb_IP[] PROGMEM = "IP ADDRESS ";

const char lb_TRY_RECONNECT[] PROGMEM = "Try reconnect ";
const char lb_ERROR_CONNECT[] PROGMEM = "Connect ERROR!";
const char lb_TIMEOUT_CONNECT[] PROGMEM = "Connect timeout!";

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
        
const char lb_RESTORE_START[] PROGMEM = "Starting Restore...";
const char lb_EMPTY_IP_STATIC[] PROGMEM = "Empty static IP, disable DHCP.";
const char lb_EMPTY_SSID[] PROGMEM = "Clear Wifi SSID.";
const char lb_EMPTY_PASSWORD[] PROGMEM = "Clear Wifi PASSWORD.";
 
const char lb_SETUP_REQUEST_TIMEOUT[] PROGMEM = "Set MAX request timeout to: ";       
const char lb_SETUP_DEBUG[] PROGMEM = "Set DEBUG to: ";
const char lb_YES[] PROGMEM = "YES";
const char lb_NO[] PROGMEM = "NO";

const char lb_NEWLINE[] PROGMEM = "\r\n";
const char lb_HTTP_200[] PROGMEM= "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json\r\nUser-Agent: Wifi-switch\r\nConnection: close\r\n\r\n";
const char lb_HTTP_GET_PREFIX[] PROGMEM = "GET ";
const char lb_SERIAL_KEY_GET[] PROGMEM = "&&serial=";
const char lb_HTTP_HEADER_VERSION[] PROGMEM = " HTTP/1.1\r\n";
const char lb_HTTP_HEADER_HOST[] PROGMEM = "Host: ";
const char lb_HTTP_HEADER_CONNECTION[] PROGMEM = "Connection: close\r\nCache-Control: max-age=0\r\nAccept: text/html,text/plain\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) coc_coc_browser/50.2.175 Chrome/44.2.2403.175 Safari/537.36\r\nAccept-Encoding: gzip, deflate, sdch\r\nAccept-Language: en-US,en\r\n\r\n";


// first 50 bytes from 0 - 49 for device infomation
typedef struct EEPROM_DEVICE {
    uint8_t DEBUG;  //  0
    uint8_t FIRST_START;    // 1
    char DV_ADMIN[MAX_ADMIN_LEN + 1];   // 2  - 12 Admin password to setup by serial + null
    char DV_SERIAL[MAX_SERIAL_LEN + 1]; // 13 - 23 IMEI/Serial of device + null
    uint8_t USE_DHT;    // 24
    uint8_t DHT_PIN;    // 25
    uint8_t DHT_TYPE;   // 26
    uint8_t USE_DS18B20;// 27
    uint8_t DS18B20_PIN; // 28
    uint8_t DS18B20_TYPE; // 29
} BDEVICE;

// after from 50 - 2 for wifi
typedef struct EEPROM_WIFI {
    uint8_t IS_STATICIP;    // 50 use for enable/disable static ip
    uint8_t WF_CONN_TIMEOUT;    // 51 use for time of wifi timeout connect by WF_CONN_TIMEOUT * 0.5s
    char WF_SSID[MAX_SSID_LEN + 1]; // 52 - 84 ssid + null
    char WF_PASSWORD[MAX_PASSWORD_LEN + 1]; // 85 - 117 WiFi password,  if empyt use OPEN, else use AUTO (WEP/WPA/WPA2) + null
    uint32_t WF_STATICIP; // 118 - 121 IP of staticIP, if empty use DHCP
    char WF_KEY[MAX_KEY_LEN + 1]; // 122 - 132 key to control by network + null
    char MASTER_SERVER[MAX_SERVER_LEN + 1]; // 133 - 181 server process sensor (ex Pi2) + null
    uint16_t MASTER_SERVER_PORT; // 182-183  port for listen of server process sensor
    char AP_SSID[MAX_SSID_LEN + 1]; // 184 - 216 AP ssid + null
    char AP_PASSWORD[MAX_PASSWORD_LEN + 1]; // 217 - 249 AP WiFi password,  if empyt use OPEN, else use AUTO (WEP/WPA/WPA2) + null
    uint8_t MAX_REQUEST_TIMEOUT;    // 250 use for time of wifi timeout connect by MAX_REQUEST_TIMEOUT * 0.5s   
} BNETINFO;


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