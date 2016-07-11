#ifndef HeatChamber_h
#define HeatChamber_h

#include <avr/pgmspace.h>
#include <RotatyEncoderMenu.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>

// Device global
#define B_DV_NAME   F("Hello")
#define B_COMPANY   F("HBInvent")
boolean err=0;
float setTemp, nowTemp, newSetTemp;
int deltaCalib[10];
int ErrorCode;
int ledval = 0;
//Serial

String serialInput="";
boolean serialComplete = false;

// Relay
#define BAT(X) digitalWrite(X, LOW)  
#define TAT(X) digitalWrite(X, HIGH)  
#define HEATER_PIN 5
#define COOLER_PIN 6
#define COOLER_VAL_PIN 7
#define FAN_PIN 7
#define BELL_PIN 7

// Sensor 
#define INSIDE_PIN 14
#define OUTSIDE_PIN 15

float insideT, outsideT;
OneWire ds1(INSIDE_PIN);   // inside
OneWire ds2(OUTSIDE_PIN);   // outside
byte addr1[8];
byte addr2[8];
byte data1[2], data2[2];

byte TaskReadSensorStep=0;
uint32_t TaskReadSensorNextTime, TaskReadSensorLastTime;
void InitSensor(void){
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
};
void TaskReadSensor(void)  // This is a task.
{
    if((millis()-TaskReadSensorLastTime)<TaskReadSensorNextTime) return;
    if(TaskReadSensorStep==0){
        ds1.reset();
        ds1.select(addr1);
        ds1.write(0x44); // start conversion, read temperature and store it in the scratchpad
        ds2.reset();
        ds2.select(addr2);
        ds2.write(0x44); // start conversion, read temperature and store it in the scratchpad
        
        TaskReadSensorLastTime = millis();  
        TaskReadSensorNextTime = 450;  // wait for convertion    
        TaskReadSensorStep++;
    }else if(TaskReadSensorStep==1){
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
            
        TaskReadSensorLastTime = millis();  
        TaskReadSensorNextTime = 0;      
        TaskReadSensorStep=0;  
    }
};
// Rotaty Encoder PIN
#define A_PIN 2
#define B_PIN 3
#define BTN_PIN 4

RotatyEncoderMenu *enMenu;
RotatyEncoderMenu::menuValue menu;
int menuIndex,lastMenuIndex;
void InitREncoder(){
    cli();
    //set timer2 interrupt at 1kHz
    TCCR2A = 0;// set entire TCCR2A register to 0
    TCCR2B = 0;// same for TCCR2B
    TCNT2  = 0;//initialize counter value to 0
    OCR2A = 249;// set compare match register for 1khz increments: (16*10^6) / (1000*64) - 1 (must be <256)
    TCCR2A |= (1 << WGM21);// turn on CTC mode
    TCCR2B |= (1 << CS22);   // Set CS21 bit for 64 prescaler
    TIMSK2 |= (1 << OCIE2A);    // enable timer compare interrupt
    sei();
    enMenu = new RotatyEncoderMenu(A_PIN,B_PIN,BTN_PIN,4);
    enMenu->setAccelerationEnabled(true);
    enMenu->setSubMenu(3);  // Max submenu
    enMenu->setSubItem(0,1);  // Home view, Sub menu no item
    enMenu->setSubItem(1,1);  // No sub menu
    enMenu->setSubItem(2,10); // 10 sub item( 10 position of calibration)
};
void TaskInput(void)  // This is a task.
{
    menu = enMenu->getValue();
    menuIndex = menu.pos;
    if(menu.event){
      if((int)(menuIndex/100)==1){
          newSetTemp = setTemp + menu.value;
//              EEPROM.put(R_SETTEMP, newSetTemp);
          enMenu->goHome();
      }else if((int)(menuIndex/100)==2){
          byte indexCal = (byte)(menuIndex%100);
          
      }
    }
}

// EEPROM 
#define R_SETTEMP 10
#define LCD_RATE 500 //ms
// LCD
#define LCD_I2C_ADDR    0x27
LiquidCrystal_I2C LCD(LCD_I2C_ADDR,16,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display
uint32_t TaskLCDLastTime;
void InitDisplay(void){
    LCD.init();                      // initialize the lcd
    LCD.clear();  // Reset the display  
    LCD.backlight();  //Backlight ON if under program control 
    LCD.home(); LCD.print(B_COMPANY);
    LCD.setCursor(0,1); LCD.print(B_DV_NAME);
    delay(1000);
    LCD.clear();
};
void TaskDisplay(void)  // This is a task.
{
    if((millis()-TaskLCDLastTime)<LCD_RATE) return;
    TaskLCDLastTime = millis();
    if(lastMenuIndex != menuIndex)  LCD.clear();
    LCD.home();
    if((int)(menuIndex/100) == 0){  // Home
      LCD.setCursor(0,0); LCD.print(F("Set temp:"));   LCD.setCursor(10,0);    LCD.print(setTemp,1);
      LCD.setCursor(0,1); LCD.print(F("Now temp:"));   LCD.setCursor(10,1);    LCD.print(insideT,1);
    }else if((int)(menuIndex/100) == 1){
      LCD.setCursor(0,0); LCD.print(F("Set temp:"));   LCD.setCursor(10,0);    LCD.print(newSetTemp,1);          
    }else if((int)(menuIndex/100) == 2){
      LCD.setCursor(0,0); LCD.print(F("Now sensor temp:"));   LCD.setCursor(9,1);    LCD.print(insideT,1);          
    }else if((int)(menuIndex/100) == 3){
      byte indexCal = (byte)(menuIndex%100);
      LCD.setCursor(0,0); LCD.print(F("Calib at: "));   LCD.setCursor(9,0);   LCD.print(indexCal);    LCD.print(F(" C"));
      LCD.setCursor(0,0); LCD.print(F("Delta: "));   LCD.setCursor(9,1);    LCD.print(deltaCalib[indexCal]/100,2);          
    }
    lastMenuIndex = menuIndex;
}

// Task
void TaskRunControl( void );

#endif
