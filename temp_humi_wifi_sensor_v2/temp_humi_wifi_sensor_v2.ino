#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <EEPROM.h>  // used: EEPROM.func

#include "BWIFI.h"  // used: BWifi.func
#include "BUTILS.h" // used: Butils.func
#include "BClient.h"  // used: BClient def new client
#include "BDictionary.h"

#define ALERT_LED 14
#define DHTPIN 0
#define DHTTYPE DHT22

EEPROM_DEVICE DV_INF;
EEPROM_WIFI WF_INF;


BClient client;
BWIFI wifi;
BDict data;
//ESPHB esp(ALERT_LED);
//DHT dht(DHTPIN, DHTTYPE, 11);
// Create wifi server
WiFiServer server(80);

String respone ="";
boolean isalert=false;

unsigned long last_send_temp=0;

void setup() {
  Serial.begin(115200); // Open serial communications and wait for port to open:
  EEPROM.get(0,DV_INF);
  EEPROM.get(50,WF_INF);

  data.set("serial",String(DV_INF.DV_SERIAL));
  data.set("ssid",String(WF_INF.WF_SSID));
  data.set("password",String(WF_INF.WF_PASSWORD));
  data.set("apssid",String(WF_INF.AP_SSID));
  data.set("appass",String(WF_INF.AP_PASSWORD));
  
  client.config(WF_INF.MAX_REQUEST_TIMEOUT,DV_INF.DEBUG);
  wifi.config(DV_INF.DEBUG);
  
  wifi.connect(WF_INF.WF_SSID, WF_INF.WF_PASSWORD);
  server.begin();    // Start the server
  Serial.println("Server started");
}

void loop() {


    WiFiClient client2 = server.available(); // Check if a client has connected
    if (!client2) {
      return; // return at here is code bellow not run and re loop() when if condition true
    }
    
// Wait until the client sends some data
    while (!client2.available()) { 
    }
    String req = client2.readStringUntil('\r');  // Read the request
    client2.flush();
    Serial.println("New client connecting..");
    Serial.println("Incomming requests:");
    Serial.println(req);
    respone="";
    //respone==esp.httpHandlerEvent(&req);
    Serial.println(respone);
    client2.print(respone); 
    client2.stop();
    req="";
}



