
/*

 It connects to an MQTT server then:
  - on 0 switches off relay
  - on 1 switches on relay
  - on 2 switches the state of the relay

  - sends 0 on off relay
  - sends 1 on on relay

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 The current state is stored in EEPROM and restored on bootup

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>


const char* ssid = "HBInvent";
const char* password = "!()(!(*(";
const char* mqtt_server = "19.9.89.2";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const char* outTopic = "private/binhpham/relay1";
const char* inTopic = "private/binhpham/+";



void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    for(int i = 0; i<500; i++){
      delay(1);
    }
    Serial.print(".");
  }
  digitalWrite(13, LOW);
  delay(500);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
  digitalWrite(13, HIGH);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "Sonoff1 booted");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      for(int i = 0; i<5000; i++){
        delay(1);
      }
    }
  }
}


void setup() {
   
  Serial.begin(115200);
  setup_wifi();                   // Connect to wifi 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}



