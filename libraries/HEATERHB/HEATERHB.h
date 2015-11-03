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

#ifndef HEATERHB_h
#define HEATERHB_h
#include "Arduino.h"
//#include <EEPROM.h>

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
#define _CONFIGS_ 0		// Lan khoi dong dau tien

struct configs{
	boolean debug;
	char	serial[10];
	float	DispTemp;
};
class HEATERHB	// class chua cac ham xu ly cua thu vien
{
	public:
		void read_configs(void);
		void save_configs(void);
		void set_debug(boolean _debug);	// Set debug mode on or off
		boolean set_serial(String _serial);
		void set_temp(float _temp);
		void DeCommand(String *_request, String _separate,String *_key,String *_val);

		
		void SerialEvent(void);
	private:
		configs	cfg;
};
#endif