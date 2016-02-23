#ifndef EEPROMData_h
#define EEPROMData_h

#include <EEPROM.h>
// EEPROM
// EEPROM save string need string size + 2 byte, fisrt byte store len of string, second byte store max len of string, after are bytes of string
// EEPROM save boolean, byte need 1 byte
// EEPROM save int need 2 byte
// EEPROM save float, IPAddress need 4 byte
#define MAX_EEPROM_SIZE 512		// Lan khoi dong dau tien

#define MAX_ADMIN_LEN    10
#define MAX_SERIAL_LEN   10	

#define MAX_SSID_LEN     32
#define MAX_PASSWORD_LEN 32	// Key to control 12           Bytes 32 to 43
#define MAX_STATICIP_LEN 10
#define MAX_KEY_LEN	     10	// SSID Wifi 34 byte           Bytes 44 to 77
#define MAX_SERVER_LEN	 48	// PASSWORD Wifi 34 byte   Bytes 78 to 111

// set the EEPROM structure
// first 50 bytes from 0 - 49 for device infomation
struct EEPROM_DEVICE {
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
} ;

// after from 50 - 2 for wifi
struct EEPROM_WIFI {
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
} ;

class EEPROMData{
    public:
        template <class T> void EEPROMSave(int StartAddress,T *storageVar);
        template <class T> void EEPROMRead(int StartAddress,T *storageVar);    
};
#endif