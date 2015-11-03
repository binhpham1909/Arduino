 /* AC POWER CONTROL WITH PWM AND ZERO CROSS DETECTION */
 /* AUTHOR: Dimitris El. Vassis - 2013 */
 
//#include <ACPWM.h>
//#include <dht.h>
#include <PID_v1.h>
#include <OneWire.h>
 
#define ZERO_PIN 2 //Input pin from zero cross detector
#define PWM_PIN 9 //Output pin to TRIAC / SSR
#define DHT22_PIN 4
#define PIN_INPUT 0
#define PIN_OUTPUT 3

//int SET_PIN = A0; //Analog pin for setting the dutyCycle value with a pontentiometer
/*dht DHT;
struct
{
    uint32_t total;
    uint32_t ok;
    uint32_t crc_error;
    uint32_t time_out;
    uint32_t connect;
    uint32_t ack_l;
    uint32_t ack_h;
    uint32_t unknown;
} stat = { 0,0,0,0,0,0,0,0};
*/
//Define Variables we'll be connecting to
double Setpoint, Input, Output, LastInput;

//Define the aggressive and conservative Tuning Parameters
double aggKp=4, aggKi=0.2, aggKd=1;
double consKp=1, consKi=0.05, consKd=0.25;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

void setup()
{
  Serial.begin(115200);
  Serial.println("Temp control");
  Serial.println("VERSION: v0.1");
  Serial.println("AUTHOR: PHAM HUU BINH"); 

  pinMode(ZERO_PIN, INPUT);
  pinMode(PWM_PIN, OUTPUT);
  //Initialize PWM operation.
  //Mains frequency: 50Hz.
  //Zero crossing point reached whenever pulse to PIN2 changes
  //Duty cycle = 0..255. 0:always off. 255: always on. 150: 59% on.
//  ACpwm.initialize(50,ZERO_PIN,CHANGE,PWM_PIN,255);
  //Latching when voltage is positive: 3 microseconds.
  //Latching when voltage is negative: 5 microseconds.
//  ACpwm.setLatch(3,5);

  Setpoint = 60;
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
}
 
void loop(){
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  Serial.print("Read Temp\t");
  uint32_t start = micros();
//  int chk = DHT.read22(DHT22_PIN);

  uint32_t stop = micros();
  stat.total++;
  switch (chk){
    case DHTLIB_OK:
        stat.ok++;
        LastInput=DHT.temperature;
        Serial.print("OK,\t");
        break;
    case DHTLIB_ERROR_CHECKSUM:
        stat.crc_error++;
        LastInput=DHT.temperature;
        Serial.print("Checksum error,\t");
        break;
    case DHTLIB_ERROR_TIMEOUT:
        stat.time_out++;
        LastInput=DHT.temperature;
        Serial.print("Time out error,\t");
        break;
    case DHTLIB_ERROR_CONNECT:
        stat.connect++;
        LastInput=100;
        Serial.print("Connect error,\t");
        break;
    case DHTLIB_ERROR_ACK_L:
        stat.ack_l++;
        LastInput=100;
        Serial.print("Ack Low error,\t");
        break;
    case DHTLIB_ERROR_ACK_H:
        stat.ack_h++;
        LastInput=100;
        Serial.print("Ack High error,\t");
        break;
    default:
        stat.unknown++;
        LastInput=100;
        Serial.print("Unknown error,\t");
        break;
  }
  Input=LastInput;
  double gap = abs(Setpoint-Input); //distance away from setpoint
  if (gap < 10)
  {  //we're close to setpoint, use conservative tuning parameters
    myPID.SetTunings(consKp, consKi, consKd);
  }
  else
  {
   //we're far from setpoint, use aggressive tuning parameters
   myPID.SetTunings(aggKp, aggKi, aggKd);
  }
  myPID.Compute();
  Serial.print(DHT.humidity, 1);
  Serial.print(" H,\t");
  Serial.print(DHT.temperature, 1);
  Serial.print(" C,\t");
  Serial.print(stop - start);
  Serial.print("us");
  Serial.print(",\tPID Input:");
  Serial.print(Input);
  Serial.print(",\t PID output:");
  Serial.print(Output);
//  Serial.print(",\t Kp:");
//  Serial.print(myPID.GetKp());
//  Serial.print(",\t Ki:");
//  Serial.print(myPID.GetKi());
//  Serial.print(",\t Kd:");
//  Serial.println(myPID.GetKd());
  //Adjust the power with the potentiometer
  //Set the duty cycle equal to the value of the potentiometer.
//  ACpwm.setDutyCycle(Output);
  if(Setpoint>Input){digitalWrite(PWM_PIN, HIGH);}else{digitalWrite(PWM_PIN, LOW);}
  if((Input-Setpoint)>10){digitalWrite(PWM_PIN, LOW);}
  Serial.print(",\t SSR :");
  if(digitalRead(PWM_PIN)){Serial.println("ON");}else{Serial.println("OFF");}
  
  delay(1500);
 }
