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
#define A_PIN 8
#define B_PIN 9
#define BTN_PIN 7
// RTC PIN
#define RTC_CE 3
#define RTC_IO 4
#define RTC_SCK 2

// Text display menu
const char lb_menu0[] PROGMEM="Plant Pum Control";
const char lb_menu1[] PROGMEM="Set moisture 0%";
const char lb_menu2[] PROGMEM="Set moisture 100%";
const char lb_menu3[] PROGMEM="Set moisture";
const char lb_menu4[] PROGMEM="Set clock";
const char lb_menu5[] PROGMEM="Set Timer";
const char lb_menu6[] PROGMEM="OFF Time";
const char* const lb_menu[] PROGMEM ={lb_menu0, lb_menu1, lb_menu2, lb_menu3, lb_menu4, lb_menu5, lb_menu6};
char  NowMenu=0;
// define
#define MAX_SUBMENU  7
// Menu state
boolean inSetup = false;
boolean inSubMenu = false;
boolean inSubItem = false;
boolean inChange = false;
// Menu position
byte posSubMenu = 0;
byte posSubItem = 0;
int posMenuCurent = 0;
// Menu array
byte maxSubItem[MAX_SUBMENU];

LCD5110 LCD(LCD_SCK,LCD_MOSI,LCD_DC,LCD_RST,LCD_CS);
extern uint8_t HBIlogo[];
extern uint8_t SmallFont[];
String rowLCD[4];
// EEPROM
#define _CONFIGS_ 0    
struct configs{
  byte Mode;
  
} cfg;

// Encoder
ClickEncoder *encoder;
int16_t lastEn, valueEn;

// Create a DS1302 object.
DS1302 rtc(RTC_CE,RTC_IO,RTC_SCK);
Time t_now(2013, 9, 22, 1, 38, 50, Time::kSunday);

// New value to save
byte nMode;

void setup(void) {
    Serial.begin(115200);
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
  displayLCD();
  controlPump();
  //EEPROM.put( _CONFIGS_, cfg );
}

// Sub program
void initLCD(){
    LCD.InitLCD();
    LCD.drawBitmap(0, 0, HBIlogo, 84, 48); delay(2000);
    LCD.invert(true); delay(500); LCD.invert(false);  delay(500);
    LCD.setFont(SmallFont);
    LCD.print("HBInvent.vn" , LEFT, 30);    delay(500);
    LCD.clrScr();
};
void initECRotaty(){
    encoder = new ClickEncoder(A_PIN,B_PIN,BTN_PIN);
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
    return (float)(analogRead(MOISTURE_PIN)*(cfg.adc100-cfg.adc0)+100*cfg.adc0);
};
// Control
void controlPump(){
  
};

float calMoisture(int adcVal){
  return (float)(adcVal*(cfg.adc100-cfg.adc0)+100*cfg.adc0);
};
void setTime(Time& t){
  rtc.writeProtect(false);
  rtc.halt(false);
  // Make a new time object to set the date and time.
  // Sunday, September 22, 2013 at 01:38:50.
  //Time t(2013, 9, 22, 1, 38, 50, Time::kSunday);
  // Set the time and date on the chip.
  rtc.time(t);
};
void timerIsr() {
  encoder->service();
};
void saveItem(){
    switch(posMenuCurent){
        case 100:  break;
        case 101:  break;  // mode run
        case 200:  break;
        case 201:  break;  // mode run
        case 202:  break;
        case 300:  break;
        case 301:  break;  // mode run
        case 302:  break;
        case 400:  break;
        case 401:  break;  // mode run
        case 402:  break;
        case 403:  break;
        case 500:  break;
        case 501:  break;  // mode run
        case 502:  break;
        case 503:  break;
        case 600:  break;
        case 601:  break;  // mode run
        case 602:  break;
    };
};
// View
void displayLCD(){
    if(!inSetup){
        LCD.clrScr();
        LCD.setFont(SmallFont); // font 6x8
        LCD.print("Moisture: " + String("Value") , LEFT, 0);
        LCD.print("Last ON: " + String("Value") , LEFT, 0);
        LCD.print("Last ON time: " + String("Value") , LEFT, 0);
        LCD.print("Moisture: " + String("Value") , LEFT, 0);
        LCD.print("Moisture: " + String("Value") , LEFT, 0);
    }else{
        LCD.clrScr();
        char buff[14];
        switch(posMenuCurent){
            case 100:
                nMode = cfg.Mode;
                strcpy_P(row1, (char*)pgm_read_word(&(lb_menu[posSubMenu])));
                rowLCD[0] = str(buff);
                rowLCD[1] = "";
                rowLCD[2] = getModeLabel(nMode);
                rowLCD[3] = "";
            break;
            case 101:  break;  // mode run
            case 200:  break;
            case 201:  break;  // mode run
            case 202:  break;
            case 300:  break;
            case 301:  break;  // mode run
            case 302:  break;
            case 400:  break;
            case 401:  break;  // mode run
            case 402:  break;
            case 403:  break;
            case 500:  break;
            case 501:  break;  // mode run
            case 502:  break;
            case 503:  break;
            case 600:  break;
            case 601:  break;  // mode run
            case 602:  break;
        };
        for(char i=0; i<maxSubItem[posSubMenu]; i++){
            if(posSubItem==i){
                LCD.setFont(Arial10In);
            }else{
                LCD.setFont(Arial10);
            }
            LCD.print(rowLCD[i], LEFT, i*10+1);
        }
    }
};
String getModeLabel(byte modes){
    if(modes==0)    return "Manual";
    else if(modes==1)    return "Auto";
    else if(modes==2)    return "Timer";
};
// Model
void createMenuList(){
    maxSubItem[0] = 2;  // Set mode run
    maxSubItem[1] = 3;  // Set moisture: moisture, offset
    maxSubItem[2] = 3;  // Set timer: time on, time off
    maxSubItem[3] = 4;  // Set time enable: hh, mm, ss
    maxSubItem[4] = 4;  // Set time disable: hh, mm, ss
    maxSubItem[5] = 4;  // Set RTC clock: hh, mm, ss
    maxSubItem[6] = 3;  // Calibration: adc0, adc100
//    maxSubItem[7] = 11;  // Program: adc0, adc100
};
void processEncoder(){
    valueEn += encoder->getValue();
    if (valueEn == lastEn) {
        return;
        };
    if (!inSetup) {
        lastEn = valueEn;
        return;
    };
    int posDelta = valueEn - lastEn;
    lastEn = valueEn;
    if(!inSubMenu){
        posSubMenu = (int)(posSubMenu + posDelta)%MAX_SUBMENU;
    }else if(inSubMenu&&!inSubItem){
        posSubItem = (int)(posSubItem + posDelta)%maxSubItem[posSubMenu];
    }else if(inSubMenu&&inSubItem&&inChange){
// user insert function for subItem change at here
        switch(posMenuCurent){
            case 100:  break;
            case 101:  break;  // mode run
            case 200:  break;
            case 201:  break;  // mode run
            case 202:  break;
            case 300:  break;
            case 301:  break;  // mode run
            case 302:  break;
            case 400:  break;
            case 401:  break;  // mode run
            case 402:  break;
            case 403:  break;
            case 500:  break;
            case 501:  break;  // mode run
            case 502:  break;
            case 503:  break;
            case 600:  break;
            case 601:  break;  // mode run
            case 602:  break;
        };
    };
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
    inChange = false;
};
void handlerClicked(){
    if(!inSetup) {
        inSubMenu = false;
        inChange = false;
        inSubItem = false;
        posSubMenu = 0;
        posSubItem = 0;
        posMenuCurent = 0;
        updatePosMenu();
        return;
    };
    if(inSubMenu){
        if(inSubItem){
            if(inChange){
                inChange = false;
                saveItem();                
            }else{
                inChange = true;
            }
        }else{
            inSubItem = true;
            inChange = false;
        }
    }else{
        inSubMenu = true;
        inSubItem = false;
        inChange = false;
        posSubItem = 0;
    }
    updatePosMenu();
};
void handlerPressed(){};
void handlerReleased(){};
void handlerDoubleClicked(){};
void updatePosMenu(){
    posMenuCurent = posSubMenu*100 + posSubItem;
}

