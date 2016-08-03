#include "HeatChamber_NoTune.h"
// the setup function runs once when you press reset or power the board

void setup() {
    Serial.begin(115200);
    Serial.println(B_DV_NAME);
    readEEPROM();
    InitDisplay();
    InitREncoder();
    HPID.SetMode(AUTOMATIC);
    HPID.SetOutputLimits(0, WINDOWS_SIZE);
    CPID.SetMode(AUTOMATIC);
    CPID.SetOutputLimits(0, WINDOWS_SIZE);
    pinMode(HEATER_PIN, OUTPUT);  TAT_RELAY(HEATER_PIN);
    pinMode(COOLER_PIN, OUTPUT); TAT_RELAY(COOLER_PIN); 
    pinMode(COOLER_VAL_PIN, OUTPUT); TAT_RELAY(COOLER_VAL_PIN);  
    double a=ds1.readTemp();
    _startTemp = map(insideT,Calib);   
}
ISR(TIMER1_COMPA_vect){   
    enMenu->service();
    _countHPID = (++_countHPID % WINDOWS_SIZE);
    if(_countHPID==0){
        _onPWM = 0;
        _inChangeHPID = true;
    }
    if(_countHPID == _breakHPID) _onPWM = 1;
    
    _countCPID = (++_countCPID % WINDOWS_SIZE);
    if(_countCPID==0){
        _onCPID = 0;
        _inChangeCPID = true;
    }
    if(_countCPID == _breakCPID) _onCPID = 1;
}
void loop(){
    TaskCheckError();
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
    // String giao tiep: {key}={val}{1}={1}
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
    if(cmd == 1){
        
    }
};

int _coolerDeltaON; 
int _coolerDeltaOFF;
boolean _runControlFirst = true;
void TaskRunControl(void){  // This is a task.
    if(_err){
        TAT_RELAY(HEATER_PIN);
        TAT_RELAY(COOLER_PIN);
        TAT_RELAY(COOLER_VAL_PIN);
        return;
    }
    if(_runControlFirst){
        if(_startTemp > _setTemp){
            _refrigMode = true;
            _coolerDeltaON = 0.3;   // auto cooler, protect block with delta temp to ON OFF 0.5 = 0.3 + 0.2
            _coolerDeltaOFF = 0.2;
        }
        else{
            _refrigMode = false;
            _coolerDeltaON = 5;     // for measure max PID value input, disable auto cooler when temp is high
            _coolerDeltaOFF = 5;    // for measure min PID value input, disable auto cooler when temp is high
        }
        _runControlFirst = false;
    }
    if(!_refrigMode&&(_nowTemp > _setTemp + _coolerDeltaON)){
        _coolerDeltaON = 0.3;
        _coolerDeltaOFF = 0.2;
        _refrigMode = true;
        BAT_RELAY(COOLER_PIN);
        _lastCoolerOn = millis();
        _lastTempCheck = _nowTemp;
    }else if(_refrigMode&&(_nowTemp < _setTemp - _coolerDeltaOFF)){
        _refrigMode = false;
        TAT_RELAY(COOLER_PIN);
        TAT_RELAY(COOLER_VAL_PIN);
        _lastHeaterOn = millis();
        _lastTempCheck = _nowTemp;
    };
    if(!_refrigMode){
        TAT_RELAY(COOLER_PIN);
        TAT_RELAY(COOLER_VAL_PIN);
        //use autotune
        if(abs(_nowTemp - _setTemp) < 1){
            HPID.SetTunings(_PIDK[0],_PIDK[1],_PIDK[2]);
        }
        HPID.Compute();
        if(_inChangeHPID){
            _breakHPID = _outHPID;
            _inChangeHPID = false;
        }
        digitalWrite(HEATER_PIN,_onPWM);
    }else{
        TAT_RELAY(HEATER_PIN);
        CPID.Compute();
        if(_inChangeCPID){
            _breakCPID = _outCPID;
            _inChangeCPID = false;
        }
        digitalWrite(COOLER_VAL_PIN,_onCPID);
    }
}


