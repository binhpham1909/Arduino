#include "HeatChamber.h"
// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);
    Serial.println(B_DV_NAME);
    readEEPROM();
    InitDisplay();
    InitREncoder();
    ACPWM.SetMode(AUTOMATIC);
//    pinMode(HEATER_PIN, OUTPUT);  TAT(LIGHT_PIN);
    pinMode(COOLER_PIN, OUTPUT); TAT_RELAY(COOLER_PIN); 
    pinMode(COOLER_VAL_PIN, OUTPUT); TAT_RELAY(COOLER_VAL_PIN);  
    pinMode(FAN_PIN, OUTPUT); BAT_RELAY(FAN_PIN);    
}
ISR(TIMER2_COMPA_vect){   
    enMenu->service();
    spk.service();
}
void loop(){
    TaskSerial();
    TaskReadSensor();
    TaskRunControl();
    TaskDisplay();
    TaskInput();
//    Serial.println(getMemoryFree());
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

String serialInput="";
boolean serialComplete = false;
void TaskSerial(void)  // This is a task.
{
    while (!serialComplete&&Serial.available()) {
        char inChar = (char)Serial.read();
        serialInput += inChar;
        if (inChar == '\n') serialComplete = true;
    }
    if(!serialComplete) return;
    // String giao tiep: {key}={val}
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
    if(cmd == 1001){
        
    }
};
#define deltaON 0.2 
boolean refrigMode;
void TaskRunControl(void){  // This is a task.
    if(!refrigMode&&(outsideT > setTemp+deltaON)){
        refrigMode = true;
        BAT_RELAY(COOLER_PIN);
    }else if(refrigMode&&(outsideT < setTemp-deltaON)){
        refrigMode = false;
        TAT_RELAY(COOLER_PIN);
    };
    if(!refrigMode){
        ACPWM.Compute();
//        Serial.println(PIDOutPut);
        analogWrite(HEATER_PIN,PIDOutPut);    
    }else{
        if(setTemp > nowTemp){
            TAT_RELAY(COOLER_VAL_PIN);
        }else{
            BAT_RELAY(COOLER_VAL_PIN);
        }
    }
}


