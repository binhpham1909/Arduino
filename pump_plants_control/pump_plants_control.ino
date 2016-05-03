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
#define LCD_SCK 17
#define LCD_MOSI  16
#define LCD_DC 15
#define LCD_RST 11
#define LCD_CS 14

// Relay pin
#define PUMP_PIN 8
#define LIGHT_PIN 9

// Sensor pin
#define MOISTURE_PIN A2

// Button pin
#define AUTO_BTN 5

// Rotaty Encoder PIN
#define A_PIN 1
#define B_PIN 2
#define BTN_PIN 3
// RTC PIN
#define RTC_CE 1
#define RTC_IO 2
#define RTC_SCK 3

// BUTTON menu
// define
#define MAX_MENU  2
#define MENU_NORMAL  0
#define MENU_SETTEMP  1
#define MENU_SETHUMI  2

// Text display menu
const char lb_menu0[] PROGMEM="Plant Pum Control";
const char lb_menu1[] PROGMEM="Set moisture 0%";
const char lb_menu2[] PROGMEM="Set moisture 100%";
const char lb_menu3[] PROGMEM="Set moisture";
const char lb_menu4[] PROGMEM="Set clock";
const char lb_menu5[] PROGMEM="Set Timer";
const char lb_menu6[] PROGMEM="OFF Time";
const char* const lb_menu[] PROGMEM ={lb_menu0, lb_menu1, lb_menu2, lb_menu2, lb_menu2};
char  NowMenu=0;
boolean inSetup = false;
boolean inSubMenu = false;
char subItem = 0;
boolean inChange = false;
enum Menu{
  Home=0, setMode, setHour, setMin, setSecond, setDate, setMonth, setYear, setOFFTime=5, setMoistureZero=6, setMoisture100=7, setMoistureReal=8
};
Menu menus=Home;
LCD5110 LCD(LCD_SCK,LCD_MOSI,LCD_DC,LCD_RST,LCD_CS);
extern uint8_t HBIlogo[];
extern uint8_t SmallFont[];

volatile long lasttime;

// EEPROM
#define _CONFIGS_ 0    
struct configs{
  int offTimeStart;
  int offTimeEnd;
  int adc100;
  int adc0;
  int adcReal;
  int adcOffset;
} cfg;

// Encoder
ClickEncoder *encoder;
int16_t lastEn, valueEn;

// Create a DS1302 object.
DS1302 rtc(RTC_CE,RTC_IO,RTC_SCK);
Time t_now;
void setup(void) {
//  save_configs();
  Serial.begin(115200);
  EEPROM.get( _CONFIGS_, cfg );
  pinMode(LIGHT_PIN, OUTPUT);  digitalWrite(LIGHT_PIN, HIGH);
  pinMode(PUMP_PIN, OUTPUT); digitalWrite(PUMP_PIN, HIGH);
  
  LCD.InitLCD();
  LCD.drawBitmap(0, 0, HBIlogo, 84, 48); delay(2000);
  LCD.invert(true); delay(500); LCD.invert(false);  delay(500);
  LCD.setFont(SmallFont);
  LCD.clrScr(); delay(500);

  encoder = new ClickEncoder(A_PIN,B_PIN,BTN_PIN);
  encoder->setAccelerationEnabled(true);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
  lastEn = -1;
}

void loop(void) {
  t_now = rtc.time();
  processEncoder();
  processEncoderBtn();
  displayLCD(menus);
  controlPump();
}
void save_configs(void){
  EEPROM.put( _CONFIGS_, cfg );
};
void displayLCD(Menu menu){
  strcpy_P(row1, (char*)pgm_read_word(&(lb_menu[NowMenu])));
  if(!inSetup){
    LCD.clrScr();
    LCD.setFont(SmallFont);
    LCD.print("Moisture" , LEFT, 0);
  }else{
    LCD.clrScr();
    LCD.setFont(SmallFont);
    if(inChange){
    
    }else{
    
    }  
  }
};
void controlPump(){
  
};
int calMoisture(int adcVal){
  
};
void timerIsr() {
  encoder->service();
};
void setTime(Time& t){
  // Initialize a new chip by turning off write protection and clearing the
  // clock halt flag. These methods needn't always be called. See the DS1302
  // datasheet for details.
  rtc.writeProtect(false);
  rtc.halt(false);

  // Make a new time object to set the date and time.
  // Sunday, September 22, 2013 at 01:38:50.
  //Time t(2013, 9, 22, 1, 38, 50, Time::kSunday);

  // Set the time and date on the chip.
  rtc.time(t);
};
void processEncoder(){
  valueEn += encoder->getValue();
  if (valueEn == lastEn) {
    return;
  };
  if (!inSetup) {
    lastEn = valueEn;
    return;
  }
  switch(menus){
    case Home: lastEn = valueEn; break;
    
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
void handlerPressed(){};
void handlerHeld(){
  inSetup = !inSetup;
  if(!inSetup) {
    menu=Home;
  }
  else{ 
    menu=setMode;
  }
  inSubMenu = false;
  inChange = false;
  subItem = 0;
};
void handlerReleased(){};
void handlerClicked(){
  if(inSetup&&(subItem==0)){
    inSubMenu = !inSubMenu;
  }else if(inSetup&&inSubmenu&&(subItem!=0)){
    inChange = !inChange;
  }
};
void handlerDoubleClicked(){
  
};
