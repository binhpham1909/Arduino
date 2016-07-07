#include <Arduino_FreeRTOS.h>
#include <avr/pgmspace.h>
#include <RotatyEncoderMenu.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
// Relay pin
#define BAT(X) digitalWrite(X, LOW)  
#define TAT(X) digitalWrite(X, HIGH)  
#define HEATER_PIN 5
#define COOLER_PIN 6
#define COOLER_VAL_PIN 7
#define FAN_PIN 7
#define BELL_PIN 7
// Sensor pin
#define INSIDE_PIN A0
#define OUTSIDE_PIN A1
// Rotaty Encoder PIN
#define A_PIN 2
#define B_PIN 3
#define BTN_PIN 4

// EEPROM save
#define R_SETTEMP 10


int ErrorCode;
int ledval = 0;
// define two tasks for Blink & AnalogRead
// Sensor

float insideT, outsideT;
RotatyEncoderMenu *enMenu;
int menuIndex;
float setTemp, nowTemp, newSetTemp;
int deltaCalib[10];

void TaskReadSensor( void *pvParameters );
void TaskSerial( void *pvParameters );
void TaskDisplay( void *pvParameters );
void TaskRunControl( void *pvParameters );
void TaskInput( void *pvParameters );


// the setup function runs once when you press reset or power the board
void setup() {
  // Now set up two tasks to run independently.
  /* Example
  xTaskCreate(
    TaskBlink
    ,  (const portCHAR *)"Blink"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
    */
    xTaskCreate( TaskReadSensor,  (const portCHAR *) "1",  128,  NULL,  1,  NULL );
    xTaskCreate( TaskSerial,  (const portCHAR *) "2",  128,  NULL,  1,  NULL );
    xTaskCreate( TaskDisplay,  (const portCHAR *)"3",  128,  NULL,  2,  NULL );
    xTaskCreate( TaskRunControl,  (const portCHAR *)"4",  128,  NULL,  2,  NULL );
    xTaskCreate( TaskInput,  (const portCHAR *)"5",  128,  NULL,  2,  NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop(){}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void TaskReadSensor(void *pvParameters)  // This is a task.
{
    OneWire ds1(INSIDE_PIN);   // inside
    OneWire ds2(OUTSIDE_PIN);   // outside
    byte addr1[8];
    byte addr2[8];
    boolean err=0;
    (void) pvParameters;
    if ( !ds1.search(addr1)) {
        ErrorCode = 11;
        Serial.println(F("Inside SS Err"));
        err=1;
    }else{
        Serial.print(F("SS1 addr= "));
        for(byte i = 0; i < 8; i++) {
            Serial.print(F(" "));
            Serial.print(addr1[i], HEX);
        }
        Serial.println(F("."));
    }
    if ( !ds2.search(addr2)) {
        ErrorCode = 12;
        Serial.println(F("Outside SS Err"));
        err=1;
    }else{
        Serial.print(F("SS2 addr= "));
        for(byte i = 0; i < 8; i++) {
            Serial.print(F(" "));
            Serial.print(addr2[i], HEX);
        }
        Serial.println(F("."));
    }
    for (;;)
    {
        byte data1[2], data2[2];
        ds1.reset();
        ds1.select(addr1);
        ds1.write(0x44); // start conversion, read temperature and store it in the scratchpad
        ds2.reset();
        ds2.select(addr2);
        ds2.write(0x44); // start conversion, read temperature and store it in the scratchpad
        vTaskDelay(15);  // one tick delay (15x30 ms) in between reads for stability 
        byte present = ds1.reset();  //now we can read the temp sensor data
        ds1.select(addr1);
        ds1.write(0xBE); // Read Scratchpad
        for (int i = 0; i < 2; i++) { // Only read the bytes you need? there is more there
            data1[i] = ds1.read();
        }
        present = ds2.reset();  //now we can read the temp sensor data
        ds2.select(addr2);
        ds2.write(0xBE); // Read Scratchpad
        for (int i = 0; i < 2; i++) { // Only read the bytes you need? there is more there
            data2[i] = ds2.read();
        }
        byte MSB = data1[1];
        byte LSB = data1[0];
        float tempRead = ((MSB << 8) | LSB); //using two’s compliment
        insideT = tempRead / 16; //this converts to C
        MSB = data2[1];
        LSB = data2[0];
        tempRead = ((MSB << 8) | LSB); //using two’s compliment
        outsideT = tempRead / 16; //this converts to C
    }
}
void TaskSerial(void *pvParameters)  // This is a task.
{
    (void) pvParameters;
    // initialize digital pin 13 as an output.
    Serial.begin(115200);
    String serialInput="";
    boolean serialComplete = false;
    for(;;) // A Task shall never return or exit.
    {
        while (!serialComplete&&Serial.available()) {
            char inChar = (char)Serial.read();
            serialInput += inChar;
            if (inChar == '\n') serialComplete = true;
            if(!serialComplete) vTaskDelay(1);  // one tick delay (30ms) in between reads for stability
        }
        if(!serialComplete){
            vTaskDelay(1);  // one tick delay (30ms) in between reads for stability
            continue;
        }
        // String giao tiep: {key}={val}
        String key;
        String val;
        int startS = serialInput.indexOf('{',0);
        int endS = serialInput.indexOf('}',startS+1);
        key = serialInput.substring(startS+1,endS);
        startS = serialInput.indexOf('{', endS+1);
        endS = serialInput.indexOf('}', startS+1);
        val = serialInput.substring(startS + 1, endS);
        int comm = key.toInt();
        // process key-command at here
            Serial.println(comm);
            Serial.println(val);
        serialInput = "";
        serialComplete = false;
        vTaskDelay(1);  // one tick delay (30ms) in between reads for stability};
  }
}
void TaskDisplay(void *pvParameters)  // This is a task.
{
    (void) pvParameters;
    LiquidCrystal_I2C LCD(0x20,16,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display
    LCD.clear();  // Reset the display  
    LCD.home();
    LCD.backlight();  //Backlight ON if under program control  
    LCD.setCursor(8,0); //Start at character 0 on line 0
    LCD.print(F("HBInvent"));
    delay(1000);
    LCD.clear();
    for (;;) // A Task shall never return or exit.
    {
        LCD.clear();
        if((int)(menuIndex/100) == 0){  // Home
          LCD.setCursor(0,0); LCD.print(F("Set temp:"));   LCD.setCursor(9,0);    LCD.print(setTemp,1);
          LCD.setCursor(0,1); LCD.print(F("Now temp:"));   LCD.setCursor(9,1);    LCD.print(nowTemp,1);
        }else if((int)(menuIndex/100) == 1){
          LCD.setCursor(0,0); LCD.print(F("Set temp:"));   LCD.setCursor(9,0);    LCD.print(newSetTemp,1);          
        }else if((int)(menuIndex/100) == 2){
          LCD.setCursor(0,0); LCD.print(F("Now sensor temp:"));   LCD.setCursor(9,1);    LCD.print(insideT,1);          
        }else if((int)(menuIndex/100) == 3){
          byte indexCal = (byte)(menuIndex%100);
          LCD.setCursor(0,0); LCD.print(F("Calib at: "));   LCD.setCursor(9,0);   LCD.print(indexCal);    LCD.print(F(" C"));
          LCD.setCursor(0,0); LCD.print(F("Delta: "));   LCD.setCursor(9,1);    LCD.print(deltaCalib[indexCal]/100,2);          
        }
        vTaskDelay(1);  // one tick delay (30ms) in between reads for stability
    }
}
void TaskRunControl(void *pvParameters)  // This is a task.
{
    (void) pvParameters;
//    pinMode(HEATER_PIN, OUTPUT);  TAT(LIGHT_PIN);
//    pinMode(COOLER_PIN, OUTPUT); TAT(PUMP_PIN); 
//    pinMode(COOLER_VAL_PIN, OUTPUT); TAT(PUMP_PIN);  
//    pinMode(FAN_PIN, OUTPUT); BAT(PUMP_PIN);    
//   pinMode(BELL_PIN, OUTPUT); BAT(BELL_PIN); 
    
    vTaskDelay(1);  // one tick delay (30ms) in between reads for stability
}
void TaskInput(void *pvParameters)  // This is a task.
{
    (void) pvParameters;
    enMenu = new RotatyEncoderMenu(A_PIN,B_PIN,BTN_PIN,4);
    enMenu->setAccelerationEnabled(true);
    enMenu->setSubMenu(3);  // Max submenu
    enMenu->setSubItem(0,1);  // Home view, Sub menu no item
    enMenu->setSubItem(1,1);  // No sub menu
    enMenu->setSubItem(2,10); // 10 sub item( 10 position of calibration)
    RotatyEncoderMenu::menuValue menu;
    cli();
    //set timer2 interrupt at 1kHz
    TCCR2A = 0;// set entire TCCR2A register to 0
    TCCR2B = 0;// same for TCCR2B
    TCNT2  = 0;//initialize counter value to 0
    // set compare match register for 8khz increments
    OCR2A = 249;// = (16*10^6) / (1000*64) - 1 (must be <256)
    // turn on CTC mode
    TCCR2A |= (1 << WGM21);
    // Set CS21 bit for 64 prescaler
    TCCR2B |= (1 << CS22);   
    // enable timer compare interrupt
    TIMSK2 |= (1 << OCIE2A);    
    sei();
    for(;;){
        menu = enMenu->getValue();
        menuIndex = menu.pos;
        if(menu.event){
          if((int)(menuIndex/100)==1){
              newSetTemp = setTemp + menu.value;
              EEPROM.put(R_SETTEMP, newSetTemp);
              enMenu->goHome();
          }else if((int)(menuIndex/100)==2){
              byte indexCal = (byte)(menuIndex%100);
              
          }
        }
        vTaskDelay(1);  // one tick delay (30ms) in between reads for stability
    }
}
ISR(TIMER2_COMPA_vect){    enMenu->service();}

