#include "BData.h"

void BData::readData(void){
    DEBUGln_HBI("Open EEPROM");
    EEPROM.begin(MAX_EEPROM_SIZE);
    EEPROM.get(WF_SSID_ADD,wf_ssid);
    DEBUG_HBI(FPSTR(lb_SSID));  DEBUGln_HBI(wf_ssid);
    EEPROM.get(WF_PASSWORD_ADD,wf_password);
    DEBUG_HBI(FPSTR(lb_PASSWORD));  DEBUGln_HBI(wf_password);
    EEPROM.get(WF_KEY_ADD,wf_key);
    DEBUG_HBI(FPSTR(lb_KEY));   DEBUGln_HBI(wf_key);
    EEPROM.get(AP_SSID_ADD,ap_ssid);
    DEBUG_HBI(FPSTR(lb_AP));    DEBUGln_HBI(ap_ssid);
    EEPROM.get(AP_PASSWORD_ADD,ap_password);
    DEBUG_HBI(FPSTR(lb_AP));    DEBUGln_HBI(ap_password);
    EEPROM.get(DV_PASSWORD_ADD,dv_password);
    DEBUG_HBI(FPSTR(lb_ADMIN_PASSWORD));    DEBUGln_HBI(dv_password);
    EEPROM.get(SERVER_IP_ADD,server_ip);
    DEBUG_HBI(FPSTR(lb_SERVER));    DEBUGln_HBI(server_ip);
    EEPROM.get(SERVER_PORT_ADD,server_port);
    DEBUG_HBI(FPSTR(lb_SERVER_PORT));    DEBUGln_HBI(server_port);
    used_DHT =(EEPROM.read(USED_DHT_ADD)==1)?true:false;
    DEBUG_HBI("Used DHT\t");    DEBUGln_HBI(used_DHT);
    DHT_type = EEPROM.read(DHT_TYPE_ADD);
    DEBUG_HBI("DHT Type\t");    DEBUGln_HBI(DHT_type);
    DHT_pin = EEPROM.read(DHT_PIN_ADD);
    DEBUG_HBI("DHT PIN\t");     DEBUGln_HBI(DHT_pin);
    wf_connect_timeout  = EEPROM.read(WF_CONNECT_TIMEOUT_ADD);
    DEBUG_HBI("Time out connect wifi\t");   DEBUGln_HBI(wf_connect_timeout);
    wf_request_timeout  = EEPROM.read(WF_REQUEST_TIMEOUT_ADD);
    DEBUG_HBI("Timeout send request\t");    DEBUGln_HBI(wf_request_timeout);
    EEPROM.end();
    DEBUGln_HBI("Close EEPROM");
};