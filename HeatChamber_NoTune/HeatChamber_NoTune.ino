#include "HeatChamber_NoTune.h"
// the setup function runs once when you press reset or power the board

void setup() {
    Serial.begin(115200);
    Serial.println(B_DV_NAME);
    readEEPROM();
    InitDisplay();
    Timer1.initialize(1000);    //set timer1 interrupt at 1kHz
    Timer1.attachInterrupt(Timer1ISR);
    Timer1.pwm(HEATER_PIN, 1023);
    enMenu = new RotatyEncoderMenu(5,A_PIN,B_PIN,BELL_PIN,BTN_PIN,4);
    enMenu->setAccelerationEnabled(true);
    enMenu->setSubItem(0,1);  // Home view, Sub menu no item
    enMenu->setSubItem(1,1);  // No sub menu
    enMenu->setSubItem(2,4);  // Timer on/off, timer time, timer after type, timer temp after
    enMenu->setSubItem(3,1);  // No sub menu
    enMenu->setSubItem(4,10); // 10 sub item( 10 position of calibration)
    enMenu->setSubItem(5,3);  // No sub menu
    enMenu->goHome();

    HPID.SetMode(AUTOMATIC);
    HPID.SetOutputLimits(0, 1023);
    HPID.SetTunings(_PIDK[0],_PIDK[1],_PIDK[2]);
    pinMode(COOLER_PIN, OUTPUT); TAT_RELAY(COOLER_PIN); 
    pinMode(COOLER_VAL_PIN, OUTPUT); TAT_RELAY(COOLER_VAL_PIN);  
    pinMode(FAN_PIN, OUTPUT); TAT_RELAY(FAN_PIN);
    double a=ds1.readTemp();
    _startTemp = map(insideT,Calib);   
}
void Timer1ISR(void){   
    enMenu->service();
}
void loop(){
    TaskCheckError();
    TaskTimer();
    TaskSerial();
    TaskDisplay();
    TaskInput();
    if(!_err){
        TaskReadSensor();
        TaskRunControl();        
    }
//    Serial.println(getMemoryFree());
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/


void TaskSerial(void)  // This is a task.
{
    while (!serialComplete&&Serial.available()) {
        char inChar = (char)Serial.read();
        serialInput += inChar;
        if (inChar == '\n') serialComplete = true;
        Serial.flush();
    }
    if(!serialComplete) return;
    // String giao tiep: {key}={val}    ex: {1}={1}
    String val;
    int key = deValue(serialInput, val);
    hanlerSCmd(key, val);
    // process key-command at here
   //     Serial.println(comm);
    //    Serial.println(val);
    serialInput = "";
    serialComplete = false;
}
void hanlerSCmd(int& cmd, String& val){
    if(cmd == 0){   // status now
        Serial.print(F("{Device}={"));   Serial.print(B_DV_NAME); Serial.print(F("},"));
        Serial.print(F("{SetTemp}={"));   Serial.print(_setTemp); Serial.print(F("},"));
        Serial.print(F("{NowTemp}={"));   Serial.print(_nowTemp); Serial.print(F("},"));
        Serial.print(F("{P}={"));   Serial.print(_PIDK[0]); Serial.print(F("},"));
        Serial.print(F("{I}={"));   Serial.print(_PIDK[1]); Serial.print(F("},"));
        Serial.print(F("{D}={"));   Serial.print(_PIDK[2]); Serial.print(F("},"));
        for(uint8_t i=0;i<=9;i++){
            Serial.print(F("{CalibAt"));   Serial.print(i*10);   Serial.print(F("}={")); Serial.print(Calib[i]); Serial.print(F("}"));  if(i<9){Serial.print(F(","));}
        }
        Serial.print(F("\n"));
    }else if(cmd == 1){   // status now
        Serial.print(F("Device:"));   Serial.println(B_DV_NAME);
        Serial.print(F("Set Temp:"));   Serial.print(_setTemp); Serial.print(F("\t"));
        Serial.print(F("Temp Now:"));   Serial.print(_nowTemp); Serial.println(F("\t"));
        Serial.print(F("P:"));   Serial.print(_PIDK[0]); Serial.print(F("\t"));
        Serial.print(F("I:"));   Serial.print(_PIDK[1]); Serial.print(F("\t"));
        Serial.print(F("D:"));   Serial.print(_PIDK[2]); Serial.println(F("\t"));
        for(uint8_t i=0;i<=9;i++){
            Serial.print(F("Calib at "));   Serial.print(i*10); Serial.print(F("oC:\t"));   Serial.println(Calib[i]);
        }
    }else if(cmd == 2){ // settemp
        Serial.print(F("Old set Temp:"));   Serial.print(_setTemp); Serial.print(F("\t"));
        _setTemp = val.toFloat();
        Serial.print(F("New set temp:"));   Serial.print(_setTemp); Serial.print(F("\t"));
        Serial.print(F("Temp Now:"));   Serial.print(_nowTemp); Serial.print(F("\t"));
        EEPROM.put(SETTEMP_ADDR, _setTemp);
        Serial.println(F("Saved"));
    }else if((cmd >=10)&&(cmd < 20)){ // 10-19 Calib at 0 10 20 ... 90
        uint8_t i = (uint8_t)(cmd%10);
        Serial.print(F("Old delta calib at "));   Serial.print(i*10); Serial.print(F("oC:\t"));    Serial.println(Calib[i],2);
        Calib[i] = val.toFloat();
        Serial.print(F("New delta calib at "));   Serial.print(i*10); Serial.print(F("oC:\t"));    Serial.println(Calib[i],2);
        EEPROM.put(START_CALIB_ADDR + i*4, Calib[i]);
        Serial.println(F("Saved"));
    }else if((cmd >=20)&&(cmd < 23)){ // P I D value
        uint8_t i = (uint8_t)(cmd%10);
        Serial.print(F("Old PID: "));
        if(i==0){
            Serial.print(F("P= "));
        }else if(i==1){
            Serial.print(F("I= "));
        }else if(i==2){
            Serial.print(F("D= "));
        }
        Serial.println(_PIDK[i],2);
        _PIDK[i] = val.toFloat();
        Serial.print(F("New PID: "));
        if(i==0){
            Serial.print(F("P= "));
        }else if(i==1){
            Serial.print(F("I= "));
        }else if(i==2){
            Serial.print(F("D= "));
        }
        Serial.println(_PIDK[i],2);
        EEPROM.put(START_HPID_ADDR + i*4, _PIDK[i]);
        Serial.println(F("Saved"));
    }
};

int _coolerDeltaON; 
int _coolerDeltaOFF;
boolean _runControlFirst = true;
void TaskRunControl(void){  // This is a task.
    if(!_running){
        Timer1.setPwmDuty(HEATER_PIN, 1023);
        TAT_RELAY(COOLER_PIN);
        TAT_RELAY(COOLER_VAL_PIN);
        TAT_RELAY(FAN_PIN);
        return;
    }
    if(_runControlFirst){   // Kiem tra nhiet do khi khoi dong voi nhiet do set
        if(_startTemp > _setTemp){  // Neu lon hon thi bat may nen
            _refrigMode = true;
            _coolerDeltaON = 0.3;   // auto cooler, protect block with delta temp to ON OFF 0.5 = 0.3 + 0.2
            _coolerDeltaOFF = 0.2;
        }
        else{   // Neu nho hon thi gia nhiet
            _refrigMode = false;
            _coolerDeltaON = 0.0;     // for measure max PID value input, disable auto cooler when temp is high
            _coolerDeltaOFF = 0.05;    // for measure min PID value input, disable auto cooler when temp is high
        }
        _runControlFirst = false;
    }
    if(!_refrigMode&&(_nowTemp > (_setTemp + _coolerDeltaON))){
        _refrigMode = true;
        BAT_RELAY(COOLER_PIN);
        TAT_RELAY(FAN_PIN);
        _lastCoolerOn = millis();
        _lastTempCheck = _nowTemp;
    }else if(_refrigMode&&(_nowTemp < (_setTemp - _coolerDeltaOFF))){
        _refrigMode = false;
        TAT_RELAY(COOLER_PIN);
        TAT_RELAY(COOLER_VAL_PIN);
        BAT_RELAY(FAN_PIN);
        _lastHeaterOn = millis();
        _lastTempCheck = _nowTemp;
    };
    if(!_refrigMode){
        TAT_RELAY(COOLER_PIN);
        TAT_RELAY(COOLER_VAL_PIN);
        BAT_RELAY(FAN_PIN);
        HPID.Compute();      
        Timer1.setPwmDuty(HEATER_PIN, 1023 - _outHPID);
    }else{
        TAT_RELAY(FAN_PIN);
        Timer1.setPwmDuty(HEATER_PIN, 1023);    // OFF
        if(_nowTemp > _setTemp){
            TAT_RELAY(COOLER_VAL_PIN);
        }else{
            BAT_RELAY(COOLER_VAL_PIN);
        }
    }
}


