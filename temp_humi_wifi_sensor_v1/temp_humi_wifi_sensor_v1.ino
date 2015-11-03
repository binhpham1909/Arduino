#include <Binh.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <DHT.h>

#define ALERT_LED 14
#define DHTPIN 0
#define DHTTYPE DHT22

String local_link = "/temphumi.php";

ESPHB esp;
DHT dht(DHTPIN, DHTTYPE, 11);
// Create wifi server
WiFiServer server(80);

float humidity, temp_c;
// Init global variables
String process_link="";
String respone ="";
boolean isalert=false;

unsigned long last_send_temp=0;
unsigned long last_read_dht=0;

void setup() {
  Serial.begin(115200); // Open serial communications and wait for port to open:
  dht.begin();
  esp.StoreStart();
  esp.read_configs();
  WiFi.mode(WIFI_STA);
  esp.wifi_connect();
    
  pinMode(ALERT_LED, OUTPUT);
  esp.LedOn(ALERT_LED);

  server.begin();    // Start the server
  Serial.println("Server started");
}

void loop() {
    esp.SerialEvent();
    if(WiFi.status() != WL_CONNECTED){esp.wifi_connect();}
    // Read dht every 5s
    if(esp.Timer(&last_read_dht,5000)){  read_dht(); }
    // send temp, humility to server
    if(esp.Timer(&last_send_temp,10000)){
      esp.GETRequest(&process_link,&respone);
      isalert = esp.CheckArlert(&respone);  
    }
    // Blink led alert
    if(isalert)  esp.LedBlink(ALERT_LED,300); else esp.LedOff(ALERT_LED); 

    WiFiClient client = server.available(); // Check if a client has connected
    if (!client) {
      return; // return at here is code bellow not run and re loop() when if condition true
    }
    
// Wait until the client sends some data
    Serial.println("New client connecting..");
    while (!client.available()) { 
    }
    String req = client.readStringUntil('\r');  // Read the request
    client.flush();
    Serial.println("Incomming requests:");
    Serial.println(req);
    respone="";
    esp.HttpServerEvent(&req,&respone);
    Serial.println(respone);
    client.print(respone); 
    client.stop();
    req="";
}
void read_dht(void){
  humidity= dht.readHumidity();
  temp_c  = dht.readTemperature();
  process_link = local_link+"?temp="+String(temp_c,2)+"&&humi="+String(humidity,2);
}



