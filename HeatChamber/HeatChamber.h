#ifndef HeatChamber_h
#define HeatChamber_h

#include <avr/pgmspace.h>
#include <RotatyEncoderMenu.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BDS18B20.h>
#include <BTone.h>
#include <EEPROM.h>

// Device global
#define B_DV_NAME   F("Heat Chamber")
#define B_COMPANY   F("HBInvent")
boolean err=0;

int ErrorCode;
int ledval = 0;

float realTemp(float x, int ca[10])
{
    int id = (int) (x/10);
//    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    return (x - (float)id*10) * (10 + (float)ca[id+1]/100 - (float)ca[id]/100) / 10 + id*10 + (float)ca[id]/100;
}

// Relay
#define BAT(X) digitalWrite(X, LOW)  
#define TAT(X) digitalWrite(X, HIGH)  
#define HEATER_PIN 5
#define COOLER_PIN 6
#define COOLER_VAL_PIN 7
#define FAN_PIN 7
#define BELL_PIN 7
// System init
#define SETTEMP_ADDR    10  // 4byte
#define START_CALIB_ADDR    14
float setTemp, nowTemp;
int Calib[10];
void InitSystem(void){
    EEPROM.get(SETTEMP_ADDR, setTemp);
    int range = (int)(setTemp/10);
    for(int i=0; i<10; i++){
        EEPROM.get(START_CALIB_ADDR + i*2, Calib[i]);
    }
}
// Speaker
BTone spk(BELL_PIN);

// Sensor 
#define INSIDE_PIN 14
#define OUTSIDE_PIN 15

float insideT, outsideT;
BDS18B20 ds1(INSIDE_PIN);   // inside
BDS18B20 ds2(OUTSIDE_PIN);   // inside

void InitSensor(void){
    ds1.init();
    ds2.init();
};
void TaskReadSensor(void){  // This is a task.
    insideT = ds1.readTemp();
    nowTemp = realTemp(insideT,Calib);
    outsideT = ds2.readTemp();
};
// Rotaty Encoder PIN
#define A_PIN 2
#define B_PIN 3
#define BTN_PIN 4
uint16_t lastMenuIndex,menuIndex;
RotatyEncoderMenu *enMenu;
RotatyEncoderMenu::menuState stt;
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
    enMenu = new RotatyEncoderMenu(4,A_PIN,B_PIN,BTN_PIN,4);
    enMenu->setAccelerationEnabled(true);
    enMenu->setSubItem(0,1);  // Home view, Sub menu no item
    enMenu->setSubItem(1,1);  // No sub menu
    enMenu->setSubItem(2,1);  // No sub menu
    enMenu->setSubItem(3,10); // 10 sub item( 10 position of calibration)
    enMenu->goHome();
};
void TaskInput(void)  // This is a task.
{
    boolean inEvt = enMenu->update();
    menuIndex = enMenu->getMenuPos();
    stt = enMenu->getMenuState();
    if(inEvt){
        Serial.print(F("evt: "));  Serial.println(inEvt);
        Serial.print(F("Pos Menu: "));  Serial.println(menuIndex);
        Serial.print(F("Delta: "));  Serial.println(enMenu->getValueChange());
        if(menuIndex==100){
            setTemp = setTemp + (float)(enMenu->getValueChange())/10;
            EEPROM.put(SETTEMP_ADDR, setTemp);
            InitSystem();
        }else if((int)(menuIndex/100) == 3){
            byte indexCal = (byte)(menuIndex%100);
            Calib[indexCal] = Calib[indexCal] + enMenu->getValueChange();
            EEPROM.put(START_CALIB_ADDR + indexCal*2, Calib[indexCal]);
            InitSystem();
        }
        enMenu->goHome();
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
      LCD.setCursor(0,1); LCD.print(F("Now temp:"));   LCD.setCursor(10,1);    LCD.print(nowTemp,1);
      if(stt==RotatyEncoderMenu::inChange)   LCD.blink(); else LCD.noBlink();
    }else if((int)(menuIndex/100) == 1){
      LCD.setCursor(0,0); LCD.print(F("Set temp:"));   LCD.setCursor(10,0);    LCD.print(setTemp + (float)(enMenu->getValueChange())/10,1); 
      if(stt==RotatyEncoderMenu::inChange)   LCD.blink(); else LCD.noBlink();
    }else if((int)(menuIndex/100) == 2){
      LCD.setCursor(0,0); LCD.print(F("Now sensor temp:"));   LCD.setCursor(9,1);    LCD.print(insideT,1);          
    }else if((int)(menuIndex/100) == 3){
      byte indexCal = (byte)(menuIndex%100);
      LCD.setCursor(0,0); LCD.print(F("Calib at: "));   LCD.setCursor(9,0);   LCD.print(indexCal*10);    LCD.print(F(" C"));
      LCD.setCursor(0,1); LCD.print(F("Delta: "));   LCD.setCursor(9,1);    LCD.print(Calib[indexCal]+enMenu->getValueChange());
      if(stt==RotatyEncoderMenu::inChange)   LCD.blink(); else LCD.noBlink();          
    }
    lastMenuIndex = menuIndex;
}

// Task
void TaskRunControl( void );

#endif
