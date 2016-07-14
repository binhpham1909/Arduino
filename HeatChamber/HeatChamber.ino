#include "HeatChamber.h"
// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);
    Serial.println(B_DV_NAME);
    InitSystem();
    InitDisplay();
    InitSensor();
    InitREncoder();
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
//    pinMode(HEATER_PIN, OUTPUT);  TAT(LIGHT_PIN);
//    pinMode(COOLER_PIN, OUTPUT); TAT(PUMP_PIN); 
//    pinMode(COOLER_VAL_PIN, OUTPUT); TAT(PUMP_PIN);  
//    pinMode(FAN_PIN, OUTPUT); BAT(PUMP_PIN);    
//    pinMode(BELL_PIN, OUTPUT); BAT(BELL_PIN); 
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
    String key;
    String val;
    int startS = serialInput.indexOf('{',0);
    int endS = serialInput.indexOf('}',startS+1);
    key = serialInput.substring(startS+1,endS);
    startS = serialInput.indexOf('{', endS+1);
    endS = serialInput.indexOf('}', startS+1);
    val = serialInput.substring(startS + 1, endS);
    int comm = key.toInt();
    hanlerSCmd(comm, val);
    // process key-command at here
   //     Serial.println(comm);
    //    Serial.println(val);
    serialInput = "";
    serialComplete = false;
}
void hanlerSCmd(int& cmd, String& val){
    
};
void TaskRunControl(void)  // This is a task.
{


}


