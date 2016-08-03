#ifndef HeatChamber_NoTune_h
#define HeatChamber_NoTune_h

#include <avr/pgmspace.h>
#include <RotatyEncoderMenu.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BDS18B20.h>
//#include <BTone.h>
#include <BMacros.h>
#include <EEPROM.h>
#include <PID_v1.h>
#include <PID_AutoTune_v0.h>

// PIN connector diagram
 
#define HEATER_PIN 10
#define COOLER_PIN 11
#define COOLER_VAL_PIN 12
#define BELL_PIN 2
#define INSIDE_PIN 16
#define OUTSIDE_PIN 17
    // Rotaty Encoder PIN
#define A_PIN 6
#define B_PIN 5
#define BTN_PIN 7

// Error checker
#define TIMEOUT_HEATER 900000  // 15p
#define TIMEOUT_COOLER 1800000  // 30p
#define DELTA_CHECK 0.1  // 0.1 oC

boolean _err=false;
int _errCode;
uint32_t _lastErrBeep;
double _lastTempCheck;

// Device global
#define B_DV_NAME   F("Heat Chamber")
#define B_COMPANY   F("HBInvent")
int ledval = 0;
boolean _refrigMode;
uint32_t _lastCoolerOn, _lastHeaterOn;
double _startTemp, _setTemp, _nowTemp = 0;
double Calib[10];
double _PIDK[3];

// PID
#define WINDOWS_SIZE 5000    // PWM for relay, control by compare stick bitween time windows size

int _countHPID, _countCPID; // , windows size 2550 ms, count: counter 1ms/1step, value of point change = PWM output x windows size/255
boolean _onPWM, _onCPID;
boolean _inChangeHPID = false, _inChangeCPID = false;
double _outHPID, _outCPID, _breakHPID, _breakCPID;
double _HKp=2, _HKi=5, _HKd=5;
double _CKp=2, _CKi=5, _CKd=5;

PID HPID(&_nowTemp, &_outHPID, &_setTemp, _HKp, _HKi, _HKd, DIRECT);  // PID for heater;
PID CPID(&_nowTemp, &_outCPID, &_setTemp, _CKp, _CKi, _CKd, DIRECT);   // PID for cooler, used VAL is Direct,

// EEPROM to save calib and settemp
#define SETTEMP_ADDR    10  // 4 byte ~ float settemp address   
#define START_CALIB_ADDR    14  // 4 byte ~ float x 10 points calib address  
#define START_HPID_ADDR    54  // 4 byte ~ float x 3 points calib address  
// Sensor 
double insideT, outsideT;
BDS18B20 ds1(INSIDE_PIN);   // inside
BDS18B20 ds2(OUTSIDE_PIN);   // inside

// LCD
#define LCD_RATE 500 //ms
#define LCD_I2C_ADDR    0x27

LiquidCrystal_I2C LCD(LCD_I2C_ADDR,16,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display
uint32_t _lastUpdateLCD;

// Serial
String serialInput="";
boolean serialComplete = false;

// Menu with Encoder
uint16_t lastMenuIndex,menuIndex;
RotatyEncoderMenu *enMenu;
RotatyEncoderMenu::menuState stt;

// Declare function
void readEEPROM(void);
void InitREncoder();
void InitDisplay(void);
void TaskReadSensor(void);
void TaskInput(void);
void TaskDisplay(void);
void TaskCheckError( void );
void TaskRunControl( void );
void changeAutoTune();
void AutoTuneHelper(boolean start);


void readEEPROM(void){
    // for first install
    /*
    float xx=0;
    EEPROM.put(SETTEMP_ADDR, xx);
    for(int i=0; i<10; i++){
        EEPROM.put(START_CALIB_ADDR + i*4, xx);
    }
    */
    
    EEPROM.get(SETTEMP_ADDR, _setTemp);
    int range = (int)(_setTemp/10);
    for(int i=0; i<10; i++){
        EEPROM.get(START_CALIB_ADDR + i*4, Calib[i]);
    }
    for(int i=0; i<3; i++){
        EEPROM.get(START_HPID_ADDR + i*4, _PIDK[i]);
        if(isnan(_PIDK[i])){
            _PIDK[i] = 0;
            EEPROM.put(START_HPID_ADDR + i*4, _PIDK[i]);
            EEPROM.get(START_HPID_ADDR + i*4, _PIDK[i]);
        }
    }
}

void InitREncoder(){
    cli();//stop interrupts
    //set timer1 interrupt at 1kHz
    TCCR1A = 0;// set entire TCCR1A register to 0
    TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;//initialize counter value to 0
    // set timer count for 1khz increments
    OCR1A = 1999;// = (16*10^6) / (1000*8) - 1
    //had to use 16 bit timer1 for this bc 1999>255, but could switch to timers 0 or 2 with larger prescaler
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS11 bit for 8 prescaler
    TCCR1B |= (1 << CS11);  
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    sei();//allow interrupts
    enMenu = new RotatyEncoderMenu(5,A_PIN,B_PIN,BELL_PIN,BTN_PIN,4);
    enMenu->setAccelerationEnabled(true);
    enMenu->setSubItem(0,1);  // Home view, Sub menu no item
    enMenu->setSubItem(1,1);  // No sub menu
    enMenu->setSubItem(2,1);  // No sub menu
    enMenu->setSubItem(3,10); // 10 sub item( 10 position of calibration)
    enMenu->setSubItem(4,3);  // No sub menu
    enMenu->goHome();
};

void InitDisplay(void){
    byte _arrow[8] = {B10000, B01000, B00100, B00010, B00010, B00100, B01000, B10000};
    byte _blank[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000};
    LCD.init();                      // initialize the lcd
    LCD.createChar(0,_blank);
    LCD.createChar(1,_arrow);
    LCD.clear();  // Reset the display  
    LCD.backlight();  //Backlight ON if under program control 
    LCD.home(); LCD.print(B_COMPANY);
    LCD.setCursor(0,1); LCD.print(B_DV_NAME);
    delay(1000);
    LCD.clear();
};

void TaskReadSensor(void){  // This is a task.
    insideT = ds1.readTemp();
    _nowTemp = map(insideT,Calib);
    outsideT = ds2.readTemp();
};

void TaskInput(void)  // This is a task.
{
    if(_err){
        enMenu->goHome();
        return;
    }
    boolean inEvt = enMenu->update();
    menuIndex = enMenu->getMenuPos();
    stt = enMenu->getMenuState();
    if(inEvt){
        if(menuIndex==100){
            _setTemp = _setTemp + (double)(enMenu->getValueChange())/10;
            EEPROM.put(SETTEMP_ADDR, _setTemp);
        }else if((int)(menuIndex/100) == 3){
            byte indexCal = (byte)(menuIndex%100);
            Calib[indexCal] = Calib[indexCal] + (double)(enMenu->getValueChange())/100;
            EEPROM.put(START_CALIB_ADDR + indexCal*4, Calib[indexCal]);
        }else if((int)(menuIndex/100) == 4){
            byte indexK = (byte)(menuIndex%100);
            _PIDK[indexK] = _PIDK[indexK] + (double)(enMenu->getValueChange())/10;
            EEPROM.put(START_HPID_ADDR + indexK*4, _PIDK[indexK]);
        }
        enMenu->goHome();
    }
}

void TaskDisplay(void)  // This is a task.
{
    if((millis()-_lastUpdateLCD)<LCD_RATE) return;
    _lastUpdateLCD = millis();
    if(_err){
        LCD.clear();
        LCD.setCursor(0,0); LCD.print(F("Error:"));
        LCD.setCursor(0,1);
        if((_errCode==1)){
            LCD.print(F("COOLER BLOCK"));
        }else if((_errCode==2)){
            LCD.print(F("HEATER"));
        }else if((_errCode==3)){
            LCD.print(F("INSIDE SENSOR"));
        }else if((_errCode==4)){
            LCD.print(F("OUTSIDE SENSOR"));
        }
        return;
    }
    if(lastMenuIndex != menuIndex)  LCD.clear();
    LCD.home();
    if((int)(menuIndex/100) == 0){  // Home
      LCD.setCursor(0,0); LCD.print(F("Set temp:"));   LCD.setCursor(10,0);    LCD.print(_setTemp,1);
      LCD.setCursor(0,1); LCD.print(F("Now temp:"));   LCD.setCursor(10,1);    LCD.print(_nowTemp,1);
      if(stt==RotatyEncoderMenu::inChange)   LCD.blink(); else LCD.noBlink();
    }else if((int)(menuIndex/100) == 1){
      LCD.setCursor(0,0); LCD.print(F("Set temp:"));   LCD.setCursor(10,0);    LCD.print(_setTemp + (double)(enMenu->getValueChange())/10,1); 
      if(stt==RotatyEncoderMenu::inChange)   LCD.blink(); else LCD.noBlink();
    }else if((int)(menuIndex/100) == 2){
      LCD.setCursor(0,0); LCD.print(F("INSIDE"));   LCD.setCursor(8,0); LCD.print(F("OUTSIDE"));
      LCD.setCursor(0,1);    LCD.print(insideT,2);   LCD.setCursor(8,1);    LCD.print(outsideT,2);   
    }else if((int)(menuIndex/100) == 3){
        byte indexCal = (byte)(menuIndex%100);
        LCD.setCursor(0,0); LCD.print(F("Calib at: "));   LCD.setCursor(9,0);   LCD.print(indexCal*10);    LCD.print(F(" C"));
        LCD.setCursor(0,1); LCD.print(F("Delta: "));   LCD.setCursor(9,1);    LCD.print(Calib[indexCal]+(double)(enMenu->getValueChange())/100,2);
        if(stt==RotatyEncoderMenu::inChange)   LCD.blink(); else LCD.noBlink();          
    }else if((int)(menuIndex/100) == 4){
        byte indexK = (byte)(menuIndex%100);
        int nowSec = (int) (millis()/1000);
        LCD.clear();
        LCD.setCursor(0,0); if((indexK == 0)&&(stt==RotatyEncoderMenu::inChange)){LCD.print(char(1));}   LCD.setCursor(1,0);     LCD.print(_PIDK[0],2);
        LCD.setCursor(8,0); if((indexK == 1)&&(stt==RotatyEncoderMenu::inChange)){LCD.print(char(1));}   LCD.setCursor(9,0);     LCD.print(_PIDK[1],2);   
        LCD.setCursor(0,1); if((indexK == 2)&&(stt==RotatyEncoderMenu::inChange)){LCD.print(char(1));}   LCD.setCursor(1,1);     LCD.print(_PIDK[2],2);
        LCD.setCursor(9,1); 
        if((nowSec%2)==0){
            LCD.print("I");
            LCD.setCursor(10,1);
            LCD.print(_nowTemp,2);
        }else{
            LCD.print("O");
            LCD.setCursor(10,1);
            LCD.print(_outHPID,2);            
        }
    }
    lastMenuIndex = menuIndex;
}
// Task
void TaskCheckError( void ){
    if(_err){
        if(millis() - _lastErrBeep > 3000){
            _lastErrBeep = millis();
            tone(BELL_PIN, 500, 300);
        }
        return;
    }
    if(_refrigMode&&(millis()-_lastCoolerOn > TIMEOUT_COOLER)){
        if(_lastTempCheck - _nowTemp < DELTA_CHECK){
            _err = true;
            _errCode = 1;    
        }
    }else if(!_refrigMode&&(millis()-_lastHeaterOn > TIMEOUT_HEATER)){
        if(_nowTemp - _lastTempCheck < DELTA_CHECK){
            _err = true;
            _errCode = 2;    
        }
    }else if(ds1.getError()){
        _err = true;
        _errCode = 3;
    }else if(ds2.getError()){
        _err = true;
        _errCode = 4;
    }
};
// Task

#endif
