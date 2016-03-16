#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include "DHT.h"

#include "BWIFI.h"  // used: BWifi.func
#include "BUTILS.h" // used: Butils.func
#include "BClient.h"  // used: BClient def new client

#define ALERT_LED 14
#define DHTPIN 0
#define DHTTYPE DHT22


BClient client;
BWIFI wifi;
WiFiServer server(80);
//ESPHB esp(ALERT_LED);
DHT dht;

String toSend ="";
String respone ="";
boolean isSendSuccess = false;
boolean isWifiConnected = false;
boolean isalert=false;

float humidity, temp_c;

void setup() {
  Serial.begin(115200); // Open serial communications and wait for port to open:
  dht.init(DHTPIN,DHTTYPE,11,true);
  wifi.init();
  wifi.setSTAMode();
  wifi.connect();
  isWifiConnected = wifi.checkConnected();
  if(!isWifiConnected){
    wifi.setAPMode();
  }
  server.begin();    // Start the server
  DEBUGln_HBI(F("Server started"));
}

void loop() {
  if((dht.readHumidity())&&(dht.readTemperature())){
    humidity= dht.readHumidity();
    temp_c  = dht.readTemperature();    
  }
  toSend = String(humidity,2) + "\t" + String(temp_c,2);
  Serial.println(toSend);
  
    wifi.reConnect();
    isWifiConnected = wifi.checkConnected();
    if(isWifiConnected){
      client.sendRequest(wifi.getServer(),wifi.getServerPort(), &toSend, wifi.getRequestTimeout());
      isSendSuccess = client.checkRespone();
      if(isSendSuccess){
        respone = client.getRespone();
      }    
    }

    WiFiClient client2 = server.available(); // Check if a client has connected
    if (!client2) {
      return; // return at here is code bellow not run and re loop() when if condition true
    }
    
// Wait until the client sends some data
    while (!client2.available()) { 
    }
    String req = client2.readStringUntil('\r');  // Read the request
    client2.flush();
    DEBUGln_HBI(F("New client connecting.."));
    DEBUGln_HBI(F("Incomming requests:"));
    DEBUGln_HBI(req);
    respone="";
    //respone==esp.httpHandlerEvent(&req);
    DEBUGln_HBI(respone);
    client2.print(respone); 
    client2.stop();
    req="";
}



