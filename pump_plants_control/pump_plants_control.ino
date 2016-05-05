#include <EEPROM.h>
#include <LCD5110_Basic.h>
#include <avr/pgmspace.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <DS1302.h>
// PIN connector
// LCD function define - pin 10 11 12 14(A0) 15(A1)
//      SCK  - Pin 8  17
//      MOSI - Pin 9  16
//      DC   - Pin 10 15
//      RST  - Pin 11 11
//      CS   - Pin 12 14
#define LCD_SCK 10
#define LCD_MOSI  11
#define LCD_DC 12
#define LCD_RST 14
#define LCD_CS 13

// Relay pin
#define PUMP_PIN 5
#define LIGHT_PIN 6

// Sensor pin
#define MOISTURE_PIN A3

// Button pin
#define AUTO_BTN 15 //A1

// Rotaty Encoder PIN
#define A_PIN 9
#define B_PIN 8
#define BTN_PIN 7
// RTC PIN
#define RTC_CE 3
#define RTC_IO 4
#define RTC_SCK 2

// Text display menu
const char lb_menu0[] PROGMEM="Pum Control";
const char lb_menu1[] PROGMEM="Mode run";
const char lb_menu2[] PROGMEM="Set Moisture";
const char lb_menu3[] PROGMEM="Timer";
const char lb_menu4[] PROGMEM="Time Enable";
const char lb_menu5[] PROGMEM="Time Disable";
const char lb_menu6[] PROGMEM="Clock time";
const char lb_menu7[] PROGMEM="Clock date";
const char lb_menu8[] PROGMEM="Calibration";
const char* const lb_menu[] PROGMEM ={lb_menu0, lb_menu1, lb_menu2, lb_menu3, lb_menu4, lb_menu5, lb_menu6, lb_menu7, lb_menu8};
char  NowMenu=0;
// define
#define MAX_SUBMENU  9
// Menu state
boolean inSetup = false;
boolean inSubMenu = false;
boolean inSubItem = false;
// Menu position
byte posSubMenu = 0;
byte posSubItem = 0;
int posMenuCurent = 0;
// Menu array
byte maxSubItem[MAX_SUBMENU];

LCD5110 LCD(LCD_SCK,LCD_MOSI,LCD_DC,LCD_RST,LCD_CS);
extern uint8_t HBIlogo[];
extern uint8_t SmallFont[];
//extern uint8_t Arial10In[];
//extern uint8_t Arial10[];
char row[6][14];
// EEPROM
#define _CONFIGS_ 0    
struct configs{
  byte Mode;
  int adc0;
  int adc100;
  float moisSet;
  float moisOffset;
  int timerOn;
  int timerOff;
  byte HHEnable;
  byte MMEnable;
  byte HHDisable;
  byte MMDisable;
} cfg, ncfg;
// Encoder
ClickEncoder *encoder;
int16_t lastEn, valueEn;

// Create a DS1302 object.
DS1302 rtc(RTC_CE,RTC_IO,RTC_SCK);
Time t_now(2016, 5, 4, 10, 00, 50, Time::kSunday);
Time nTime(2016, 5, 4, 10, 00, 50, Time::kSunday);

// pump
boolean pumpNow = false;
void setup(void) {
    Serial.begin(115200);
    // 2 line enable at first setup
    //cfg = {0, 1023, 0, 40, 5, 5, 120, 14, 0, 11, 0};
    //EEPROM.put( _CONFIGS_, cfg );
    EEPROM.get( _CONFIGS_, cfg );
    initGPIO();
    initLCD();
    initECRotaty();
    createMenuList();
}
void loop(void) {
  t_now = rtc.time();
  processEncoder();
  processEncoderBtn();
  updateLCD();
  displayLCD();
  delay(300);
//  controlPump();
  //EEPROM.put( _CONFIGS_, cfg );
}

// Sub program
void initLCD(){
    LCD.InitLCD();
    LCD.setContrast(90);
    LCD.drawBitmap(0, 0, HBIlogo, 84, 48); delay(2000);
    LCD.invert(true); delay(500); LCD.invert(false);  delay(500);
    LCD.setFont(SmallFont);
    LCD.clrScr();
    LCD.print("HBInvent.vn" , LEFT, 30);    delay(500);
};
void initECRotaty(){
    encoder = new ClickEncoder(A_PIN,B_PIN,BTN_PIN,4);
    encoder->setAccelerationEnabled(true);
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
    lastEn = -1;    
};
void initGPIO(){
    pinMode(LIGHT_PIN, OUTPUT);  digitalWrite(LIGHT_PIN, HIGH);
    pinMode(PUMP_PIN, OUTPUT); digitalWrite(PUMP_PIN, HIGH);    
};
/// Menu with encoder rotaty - MVC
int readADC(){
    return analogRead(MOISTURE_PIN);
};
float readMoisture(){
    return (float)(100*(analogRead(MOISTURE_PIN)-cfg.adc0)/(cfg.adc100-cfg.adc0));
};
// Control
void controlPump(){
    float nowMois = readMoisture();
    boolean pumpNowAuto, pumpNowMan, pumpNowTimer, pumpAllow;
    if((cfg.Mode == 0)   ){
        pumpNowMan = true;
    }else{
        pumpNowMan = false;
    };
    if((cfg.Mode == 1)&&(nowMois<=cfg.moisSet - cfg.moisOffset)&&!(nowMois>=cfg.moisSet + cfg.moisOffset)){
        pumpNowAuto = true;
    }else{
        pumpNowAuto = false;
    };
    if((cfg.Mode == 2)   ){
        pumpNowTimer = true;
    }else{
        pumpNowTimer = false;
    };
    if(((t_now.hr<=cfg.HHEnable)&&(t_now.min<=cfg.MMEnable))||((t_now.hr>=cfg.HHDisable)&&(t_now.min>=cfg.MMDisable))){
        pumpAllow = true;
    }else{
        pumpAllow = false;
    };
    pumpNow = !((pumpNow||pumpNowAuto||pumpNowMan)&&pumpAllow);
    digitalWrite(13, pumpNow);
};

float calMoisture(int adcVal){
  return (float)(adcVal*(cfg.adc100-cfg.adc0)+100*cfg.adc0);
};

void setTime(Time& t){  //Sunday, September 22, 2013 at 01:38:50. Time t(2013, 9, 22, 1, 38, 50, Time::kSunday);
  rtc.writeProtect(false);
  rtc.halt(false);
  rtc.time(t);
};
void timerIsr() {
  encoder->service();
};
void saveItem(){
    switch(posSubMenu){
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 8:
                {
                    EEPROM.put( _CONFIGS_, ncfg );
                    EEPROM.get( _CONFIGS_, cfg );   
                    break;             
                }
            case 6:
                nTime.date = t_now.date;
                nTime.mon = t_now.mon;
                nTime.yr = t_now.yr;
                setTime(nTime);
            break;  // mode run
            case 7:  
                nTime.hr = t_now.hr;
                nTime.min = t_now.min;
                nTime.sec = t_now.sec;            
                setTime(nTime);
            break;
            default: break;
    };
};
// View
void displayLCD(){
      LCD.clrScr();
      if(posSubItem==0){  LCD.invertText(true); }else{  LCD.invertText(false);}; LCD.print(row[0], LEFT, 0);
      if(posSubItem==1){  LCD.invertText(true); }else{  LCD.invertText(false);}; LCD.print(row[1], LEFT, 8);
      if(posSubItem==2){  LCD.invertText(true); }else{  LCD.invertText(false);}; LCD.print(row[2], LEFT, 16);
      if(posSubItem==3){  LCD.invertText(true); }else{  LCD.invertText(false);}; LCD.print(row[3], LEFT, 24);
      if(posSubItem==4){  LCD.invertText(true); }else{  LCD.invertText(false);}; LCD.print(row[4], LEFT, 32);
      if(posSubItem==5){  LCD.invertText(true); }else{  LCD.invertText(false);}; LCD.print(row[5], LEFT, 40);
};
void updateLCD(){
    String str;   
    if(!inSetup){
//        LCD.setFont(SmallFont); // font 6x8
        strcpy_P(row[0], (char*)pgm_read_word(&(lb_menu[posSubMenu])));
        str = String(t_now.date) + "-" + String(t_now.mon) + "-" + String(t_now.yr);
        str.toCharArray(row[1],13);
        str = String(t_now.hr) + ":" + String(t_now.min) + ":" + String(t_now.sec);
        str.toCharArray(row[2],13);
        str = "M%: "+String(readMoisture());
        str.toCharArray(row[3],13);
        if(pumpNow){
            strcpy(row[4], "PUMP: ON");
        }
        else{
            strcpy(row[4], "PUMP: OFF");
        }
        strcpy(row[5], "");
    }else{
        switch(posSubMenu){
            case 1:
                strcpy_P(row[0], (char*)pgm_read_word(&(lb_menu[posSubMenu])));
                strcpy(row[2], "");
                str = getModeLabel(ncfg.Mode);
                str.toCharArray(row[1],13);
                strcpy(row[3], "");
                strcpy(row[4], "");
                strcpy(row[5], "");
            break;
            case 2:  
                strcpy_P(row[0], (char*)pgm_read_word(&(lb_menu[posSubMenu])));
                str = "Do am: "+ String(ncfg.moisSet);
                str.toCharArray(row[1],13);
                str = "Offset: "+ String(ncfg.moisOffset);
                str.toCharArray(row[2],13);
                strcpy(row[3], "");
                strcpy(row[4], "");
                strcpy(row[5], "");            
            break;
            case 3:
                strcpy_P(row[0], (char*)pgm_read_word(&(lb_menu[posSubMenu])));
                str = "ON : "+ String(ncfg.timerOn);
                str.toCharArray(row[1],13);
                str = "OFF: "+ String(ncfg.timerOff);
                str.toCharArray(row[2],13);
                strcpy(row[3], "");
                strcpy(row[4], "");
                strcpy(row[5], "");             
            break;
            case 4:
                strcpy_P(row[0], (char*)pgm_read_word(&(lb_menu[posSubMenu])));
                str = "HH: "+ String(ncfg.HHEnable);
                str.toCharArray(row[1],13);
                str = "MM: "+ String(ncfg.MMEnable);
                str.toCharArray(row[2],13);
                strcpy(row[3], "");
                strcpy(row[4], ""); 
                strcpy(row[5], "");            
            break;  // mode run
            case 5:
                strcpy_P(row[0], (char*)pgm_read_word(&(lb_menu[posSubMenu])));
                str = "HH: "+ String(ncfg.HHDisable);
                str.toCharArray(row[1],13);
                str = "MM: "+ String(ncfg.MMDisable);
                str.toCharArray(row[2],13);
                strcpy(row[3], "");
                strcpy(row[4], ""); 
                strcpy(row[5], "");            
            break;  // mode run
            case 6:
                strcpy_P(row[0], (char*)pgm_read_word(&(lb_menu[posSubMenu])));
                str = "HH: "+ String(nTime.hr);
                str.toCharArray(row[1],13);
                str = "MM: "+ String(nTime.min);
                str.toCharArray(row[2],13);
                str = "SS: "+ String(nTime.sec);
                str.toCharArray(row[3],13);
                strcpy(row[4], ""); 
                strcpy(row[5], "");            
            break;
            case 7:
                strcpy_P(row[0], (char*)pgm_read_word(&(lb_menu[posSubMenu])));
                str = "dd  : "+ String(nTime.date);
                str.toCharArray(row[1],13);
                str = "mm  : "+ String(nTime.mon);
                str.toCharArray(row[2],13);
                str = "yyyy: "+ String(nTime.yr);
                str.toCharArray(row[3],13);
                strcpy(row[4], ""); 
                strcpy(row[5], "");            
            break;
            case 8:
                strcpy_P(row[0], (char*)pgm_read_word(&(lb_menu[posSubMenu])));
                str = "0%  : "+ String(ncfg.adc0);
                str.toCharArray(row[1],13);
                str = "100%: "+ String(ncfg.adc100);
                str.toCharArray(row[2],13);
                strcpy(row[3], "");
                strcpy(row[4], ""); 
                strcpy(row[5], "");            
            break;
            default: break;
        };
    }
};
String getModeLabel(byte modes){
    if(modes==0)    return "Manual";
    else if(modes==1)    return "Auto";
    else if(modes==2)    return "Timer";
    else return "None";
};
// Model
void createMenuList(){
    maxSubItem[0] = 1;  // 
    maxSubItem[1] = 2;  // Set mode run
    maxSubItem[2] = 3;  // Set moisture: moisture, offset
    maxSubItem[3] = 3;  // Set timer: time on, time off
    maxSubItem[4] = 3;  // Set time enable: hh, mm
    maxSubItem[5] = 3;  // Set time disable: hh, mm
    maxSubItem[6] = 4;  // Set RTC clock: hh, mm, ss
    maxSubItem[7] = 4;  // Set RTC clock: dd, mm, yy
    maxSubItem[8] = 3;  // Calibration: adc0, adc100
//    maxSubItem[7] = 11;  // Program: adc0, adc100
};
boolean processEncoder(){
    valueEn += encoder->getValue();
    if (valueEn == lastEn) {
        return 0;
        };
    if (!inSetup) {
        lastEn = valueEn;
        return 0;
    };
    int posDelta = valueEn - lastEn;
    lastEn = valueEn;
    if(!inSubMenu){
        posSubMenu = (int)(posSubMenu + posDelta)%MAX_SUBMENU;
        if(!posSubMenu) posSubMenu=1;
    }else if(inSubMenu&&!inSubItem){
        posSubItem = (int)(posSubItem + posDelta)%maxSubItem[posSubMenu];
    }else if(inSubMenu&&inSubItem){
// user insert function for subItem change at here
        switch(posMenuCurent){
            case 101:
                ncfg.Mode = (byte)(ncfg.Mode + posDelta) % 3; // 3mode MANUAL, AUTO, TIMER
            break;  // mode run
            case 201:
                ncfg.moisSet = (float)(ncfg.moisSet + posDelta*0.1);
            break;  // mode run
            case 202:
                ncfg.moisOffset = (float)(ncfg.moisOffset + posDelta*0.1);
            break;
            case 301:
                ncfg.timerOn = (int)(ncfg.timerOn + posDelta);
            break;  // mode run
            case 302:
                ncfg.timerOff = (int)(ncfg.timerOff + posDelta);
            break;
            case 401:
                ncfg.HHEnable = (byte)(ncfg.HHEnable + posDelta) % 24;
            break;  // mode run
            case 402:
                ncfg.MMEnable = (byte)(ncfg.MMEnable + posDelta) % 60;
            break;
            case 501:
                ncfg.HHDisable = (byte)(ncfg.HHDisable + posDelta) % 24;
            break;  // mode run
            case 502:
                ncfg.MMDisable = (byte)(ncfg.MMDisable + posDelta) % 60;
            break;
            case 601:
                nTime.hr = (byte)(nTime.hr + posDelta) % 24;
            break;  // mode run
            case 602:  
                nTime.min = (byte)(nTime.min + posDelta) % 60;
            break;
            case 603:
                nTime.sec = (byte)(nTime.sec + posDelta) % 60;
            break;
            case 701:
                nTime.date = (byte)(nTime.date + posDelta) % 32;
            break;  // mode run
            case 702:  
                nTime.mon = (byte)(nTime.mon + posDelta) % 13;
            break;
            case 703:
                nTime.yr = (int)(nTime.yr + posDelta);
            break;
            case 801:
                ncfg.adc0 = (int)(ncfg.adc0 + posDelta) % 1024;
            break;  // mode run
            case 802:
                ncfg.adc100 = (int)(ncfg.adc100 + posDelta) % 1024;
            break;
        };
    };
    return 1;
};
void processEncoderBtn(){
  ClickEncoder::Button b = encoder->getButton();
  if (b != ClickEncoder::Open) {
    switch(b){
      case ClickEncoder::Pressed: handlerPressed(); break;
      case ClickEncoder::Held: handlerHeld(); break;
      case ClickEncoder::Released: handlerReleased(); break;
      case ClickEncoder::Clicked: handlerClicked(); break;
      case ClickEncoder::DoubleClicked: handlerDoubleClicked(); break;
    }; 
  }
};
// Hold button in/out to Setup menu
void handlerHeld(){
    if(inSetup){
        inSetup = false;
        posSubMenu = 0;
    }else{
        inSetup = true;
        posSubMenu = 1;
    }
    inSubMenu = false;
    inSubItem = false;
    updatePosMenu();
};
void handlerClicked(){
    updatePosMenu();
    if(!inSetup) {
        inSubMenu = false;
        inSubItem = false;        
        posSubMenu = 0;
        posSubItem = 0;
        return;
    };
    if(inSubMenu){
        if(inSubItem){
            if(posSubItem != 0){
                saveItem();                
            }else{
                inSubMenu = false;
            };
            inSubItem = false;
        }else{
            if(posSubItem != 0){
                inSubItem = true;                
            }else{
                inSubItem = false;
                inSubMenu = false;
            }
        };
    }else{
        inSubMenu = true;
        inSubItem = false;
        posSubItem = 0;
    }
    updatePosMenu();
};
void handlerPressed(){};
void handlerReleased(){};
void handlerDoubleClicked(){};
void updatePosMenu(){
    posMenuCurent = posSubMenu*100 + posSubItem;
    if(!inSubItem){ncfg = cfg;}
    nTime = t_now;
}

