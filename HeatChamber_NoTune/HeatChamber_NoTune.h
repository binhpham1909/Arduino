#ifndef HeatChamber_NoTune_h
#define HeatChamber_NoTune_h

#include <avr/pgmspace.h>
#include <TimerOne.h>
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
 
#define HEATER_PIN 9
#define FAN_PIN 10
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
double _startTemp, _setTemp, _nowTemp = 0, _saveTemp;
double Calib[10];
double _PIDK[3];

// PID
#define WINDOWS_SIZE 5000    // PWM for relay, control by compare stick bitween time windows size

double _outHPID;
double _HKp=0, _HKi=0, _HKd=0;

PID HPID(&_nowTemp, &_outHPID, &_setTemp, _HKp, _HKi, _HKd, DIRECT);  // PID for heater;

/*  Timer
                         _holdTimer
_saveTemp..............._____________
                  t0   /             \    t2
_startTemp....._______/               \________
*/
boolean _running = true;
unsigned int _holdTimer;
boolean _timerOn, _finishType;
uint8_t _timerPosition = 0; // 0: upper, 1 balancer, 2: downer, 3: shutdown
uint32_t _lastTimerTime;
double _finishTemp;


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

// EEPROM to save calib and settemp
#define SETTEMP_ADDR    10  // 4 byte ~ float settemp address   
#define START_CALIB_ADDR    14  // 4 byte ~ float x 10 points calib address  
#define START_HPID_ADDR    54  // 4 byte ~ float x 3 points calib address  
#define TIMER_ENABLE_ADDR    66  // 1 byte ~ boolean timer Enable address  
#define TIMER_TIME_ADDR    67  // 2 byte ~ int timer hold time
#define TIMER_FINISH_ADDR    68  // 1 byte ~ boolean timer hold time
#define TIMER_TEMP_FINISH_ADDR    69  // 2 byte ~ int timer hold time
void readEEPROM(void){   
    EEPROM.get(SETTEMP_ADDR, _saveTemp);
    if(isnan(_saveTemp)){
        _setTemp = 0;
        EEPROM.put(SETTEMP_ADDR, _saveTemp);
        EEPROM.get(SETTEMP_ADDR, _saveTemp);
    }    
    for(int i=0; i<10; i++){
        EEPROM.get(START_CALIB_ADDR + i*4, Calib[i]);
        if(isnan(Calib[i])){
            Calib[i] = 0;
            EEPROM.put(START_CALIB_ADDR + i*4, Calib[i]);
            EEPROM.get(START_CALIB_ADDR + i*4, Calib[i]);
        }
    }
    for(int i=0; i<3; i++){
        EEPROM.get(START_HPID_ADDR + i*4, _PIDK[i]);
        if(isnan(_PIDK[i])){
            _PIDK[i] = 0;
            EEPROM.put(START_HPID_ADDR + i*4, _PIDK[i]);
            EEPROM.get(START_HPID_ADDR + i*4, _PIDK[i]);
        }
    }
    EEPROM.get(TIMER_ENABLE_ADDR, _timerOn);
    EEPROM.get(TIMER_TIME_ADDR, _holdTimer);
}

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
        }else if((int)(menuIndex/100) == 2){    // Timer
            byte indexT = (byte)(menuIndex%100);
            if(indexT ==0){
                _timerOn = (boolean)(enMenu->getValueChange())%2;
                EEPROM.put(TIMER_ENABLE_ADDR, _timerOn);
            }else if(indexT ==1){
                _holdTimer = _holdTimer + enMenu->getValueChange();
                EEPROM.put(TIMER_TIME_ADDR, _holdTimer);
            }else if(indexT ==2){
                _finishType = (boolean)(enMenu->getValueChange())%2;
                EEPROM.put(TIMER_FINISH_ADDR, _finishType);
            }else if(indexT ==3){           
                _finishTemp = _finishTemp + (double)(enMenu->getValueChange())/10;
                EEPROM.put(TIMER_TEMP_FINISH_ADDR, _finishTemp);
            }
        }else if((int)(menuIndex/100) == 4){
            byte indexCal = (byte)(menuIndex%100);
            Calib[indexCal] = Calib[indexCal] + (double)(enMenu->getValueChange())/100;
            EEPROM.put(START_CALIB_ADDR + indexCal*4, Calib[indexCal]);
        }else if((int)(menuIndex/100) == 5){
            byte indexK = (byte)(menuIndex%100);
            if(indexK!=2){// #D
                _PIDK[indexK] = _PIDK[indexK] + (double)(enMenu->getValueChange())/100;
            }else{
                _PIDK[indexK] = _PIDK[indexK] + (double)(enMenu->getValueChange())/10;
            }
            EEPROM.put(START_HPID_ADDR + indexK*4, _PIDK[indexK]);
            HPID.SetTunings(_PIDK[0],_PIDK[1],_PIDK[2]);
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
        byte indexT = (byte)(menuIndex%100);
        _timerOn = (boolean)(enMenu->getValueChange())%2;
        LCD.setCursor(0,0);
        if(_timerOn){
            LCD.print(F("Timer ON"));
            LCD.setCursor(10,0);
            LCD.print(_holdTimer);
            LCD.setCursor(0,1);
            if(_finishType==1){
                enMenu->setSubItem(2,4);  // Timer on/off, timer time, timer after type, timer temp after
                LCD.print(F("Finish:"));
                LCD.setCursor(8,1);
                LCD.print(_finishTemp,1);
            }else{
                enMenu->setSubItem(2,3);  // Timer on/off, timer time, timer after type, timer temp after
                LCD.print(F("Finish: Ambient"));
            }
        }else{
            enMenu->setSubItem(2,1);  // Only Timer on/off
            LCD.print(F("Timer OFF"));
        }
        
      LCD.setCursor(0,0); LCD.print(F("Set temp:"));   LCD.setCursor(10,0);    LCD.print(_setTemp + (double)(enMenu->getValueChange())/10,1); 
      if(stt==RotatyEncoderMenu::inChange)   LCD.blink(); else LCD.noBlink();

      
    }else if((int)(menuIndex/100) == 3){
        LCD.setCursor(0,0); LCD.print(F("INSIDE"));   LCD.setCursor(8,0); LCD.print(F("OUTSIDE"));
        LCD.setCursor(0,1);    LCD.print(insideT,2);   LCD.setCursor(8,1);    LCD.print(outsideT,2);   
    }else if((int)(menuIndex/100) == 4){
        byte indexCal = (byte)(menuIndex%100);
        LCD.setCursor(0,0); LCD.print(F("Calib at: "));   LCD.setCursor(9,0);   LCD.print(indexCal*10);    LCD.print(F(" C"));
        LCD.setCursor(0,1); LCD.print(F("Delta: "));   LCD.setCursor(9,1);    LCD.print(Calib[indexCal]+(double)(enMenu->getValueChange())/100,2);
        if(stt==RotatyEncoderMenu::inChange)   LCD.blink(); else LCD.noBlink();          
    }else if((int)(menuIndex/100) == 5){
        byte indexK = (byte)(menuIndex%100);
        int nowSec = (int) (millis()/1000);
        double a = 0;
        LCD.setCursor(0,0); LCD.print("P:");
        if((indexK == 0)){LCD.setCursor(1,0); LCD.print(char(1)); a = (double)(enMenu->getValueChange())/100;}   LCD.setCursor(2,0);     LCD.print(_PIDK[0] + a,2);  if((indexK == 0)) a=0;
        LCD.setCursor(8,0); LCD.print("I:");
        if((indexK == 1)){LCD.setCursor(9,0); LCD.print(char(1)); a = (double)(enMenu->getValueChange())/100;}   LCD.setCursor(10,0);     LCD.print(_PIDK[1] + a,2);  if((indexK == 1)) a=0; 
        LCD.setCursor(0,1); LCD.print("D:");
        if((indexK == 2)){LCD.setCursor(1,1); LCD.print(char(1)); a = (double)(enMenu->getValueChange())/10;}   LCD.setCursor(2,1);     LCD.print(_PIDK[2] + a,1);  if((indexK == 2)) a=0;
        LCD.setCursor(8,1); 
        if((nowSec%2)==0){
            LCD.print("T:");
            LCD.setCursor(10,4);
            LCD.print(_nowTemp,2);
        }else{
            LCD.print("O:");
            LCD.setCursor(10,1);
            LCD.print(_outHPID,4);            
        }
    }
    lastMenuIndex = menuIndex;
}
// Task
void TaskCheckError( void ){
    if(_err){
        _running = false;
        if(millis() - _lastErrBeep > 3000){
            _lastErrBeep = millis();
            tone(BELL_PIN, 500, 300);
        }
        return;
    }/*
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
    }*/
    else if(ds1.getError()){
        _err = true;
        _errCode = 3;
    }else if(ds2.getError()){
        _err = true;
        _errCode = 4;
    }
};
// Task Timer Run

void TaskTimer( void ){
    if(_err)    return;
    if(!_timerOn){
        _running = true;
        _setTemp = _saveTemp;
        return;
    }
    uint32_t tnow = millis();
    _setTemp = _saveTemp;
    if((_timerPosition == 0)&&(_nowTemp == _setTemp)){   // Dat den diem cbang
        _timerPosition = 1;
        _lastTimerTime = tnow;  
        _setTemp = _saveTemp;      
    }else if((_timerPosition == 1)&&((tnow - _lastTimerTime)>(_holdTimer*60000))){   // minute
        _timerPosition = 2; 
        if(_finishType){
            _setTemp = _finishTemp;
        }else{
            _setTemp = _startTemp;            
        }       
    }else if((_timerPosition == 2)&&(abs(_nowTemp - _setTemp)<0.1)){    
        _timerPosition = 3;  
        _running = false;
    }
};

#endif
