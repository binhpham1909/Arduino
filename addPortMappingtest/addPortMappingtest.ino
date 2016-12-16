/*
   Add Port Mapping to UPNP device port mapping table
 
 This example sketch uses the UPNP functionality in most modern commercial routers to add a port mapping.  
 The port mapping allows an external service to contact the LAN based device via the mapped port. 
 Logic Steps:
 1) Send an M-SEARCH broadcast request for a UPNP compliant Internet Gateway Device.
 2) Parse any device IGD request response for control URL and port number for the IGD
 3) Send an Add Port Mapping request to the IGD
 4) Optionally request the IGD for the external (public) IP address of the IGD
 
  
 created 08 Aug 2012
 by Deverick McIntyre
 version 1.0
 
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include <Ethernet.h>
#include "portMapping.h"

//declare the client object here so it is global in scope so we can call it in functions in order to print
EthernetClient client;

PortMapClass portmap;
//
const char* ssid = "HBInvent";
const char* password = "!()(!(*(";
ESP8266WebServer server(8080);
//
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network, only used if DHCP fails
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0, 177);


  /*
  core Arduino functions
  */
void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(115200);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
 WiFi.begin(ssid, password);
   // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  
    if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  //send first message after serial port is connected
  Serial.println(F("Initializing... "));

   if (portmap.addPortMap(WiFi.localIP(),80, 2900))  // add port mapping for user defined port values
   //if (portmap.addPortMap(Ethernet.localIP()))  // add port mapping for random port values
 {
   Serial.println(F("Port Mapped successfully!"));
   Serial.print(F("Internal Port: "));
   Serial.println(portmap.internalPort());
   Serial.print(F("External Port: "));
   Serial.println(portmap.externalPort());
 } else {
   Serial.println(F("unable to map port.  Check your router UPNP function is turned on"));
 }
  
    server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
Serial.println("HTTP server started");

}


void loop() {
  
 server.handleClient();
 
}
