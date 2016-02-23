#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

#include <BLANGUE.h>
#include <BJSON.h>
#include <BEEPROM.h>
#include <BString.h>
#include <BHTML.h>

#define ALERT_LED 14
#define DHTPIN 0
#define DHTTYPE DHT22

ESPHB esp(ALERT_LED);
//DHT dht(DHTPIN, DHTTYPE, 11);
// Create wifi server
WiFiServer server(80);

String respone ="";
boolean isalert=false;

unsigned long last_send_temp=0;

void setup() {
  Serial.begin(115200); // Open serial communications and wait for port to open:
  esp.Startup();
  esp.wifi_connect();
  esp.LedOn();
  server.begin();    // Start the server
  Serial.println("Server started");
}

void loop() {
    esp.SerialEvent();
    esp.wifi_reconnect();
    // send temp, humility to server
    if(esp.Timer(&last_send_temp,10000)){
      esp.sendRequest();
      isalert = esp.CheckArlert(&respone);  
    }
    // Blink led alert
    if(isalert)  esp.LedBlink(300); else esp.LedOff(); 

    WiFiClient client = server.available(); // Check if a client has connected
    if (!client) {
      return; // return at here is code bellow not run and re loop() when if condition true
    }
    
// Wait until the client sends some data
    while (!client.available()) { 
    }
    String req = client.readStringUntil('\r');  // Read the request
    client.flush();
    Serial.println("New client connecting..");
    Serial.println("Incomming requests:");
    Serial.println(req);
    respone="";
    respone==esp.httpHandlerEvent(&req);
    Serial.println(respone);
    client.print(respone); 
    client.stop();
    req="";
}



