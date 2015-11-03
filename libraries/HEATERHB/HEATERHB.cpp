#include <avr/EEPROM.h>
#include "HEATERHB.h"
#if defined(__AVR__)
	#include "Arduino.h"
	#include <avr/pgmspace.h>
#endif

////////////////////////////////////////
///  Group function Write to EEPROM  ///
////////////////////////////////////////

// read config on startup
void HEATERHB::read_configs(void){
	EEPROM.get( _CONFIGS_, cfg );
};
void HEATERHB::save_configs(void){
	EEPROM.put( _CONFIGS_, cfg );
};

void HEATERHB::set_debug(boolean _debug){
	cfg.debug=_debug;
	save_configs();
	read_configs();
};
boolean HEATERHB::set_serial(String _serial){
	if(_serial.length()==9){
		_serial.toCharArray(cfg.serial, _serial.length()+1);
		save_configs();
		read_configs();
		return true;
	}else{
		return false;
	}
};
void HEATERHB::set_temp(float _temp){
	cfg.DispTemp = _temp;
	save_configs();
	read_configs();
};
// Analysis command : key==value\n

// ==	separate
// "\n"	end
void HEATERHB::DeCommand(String *_request, String _separate,String *_key,String *_val){
    int g_end,g_compare;
    g_compare = _request->indexOf(_separate,0);
    g_end = _request->indexOf('\n',g_compare);
	*_key=_request->substring(0, g_compare);
    *_val=_request->substring(g_compare+_separate.length(), g_end);
}

//////////////////////////////////////////
///     Group function for Serial	   ///
//////////////////////////////////////////

void HEATERHB::SerialEvent(void) {
	while (Serial.available()){
		char inChar = (char)Serial.read();
		SerialString += inChar;
		if (inChar == '\n') {SerialComplete = true;}   
		if(SerialComplete){
			String command,value;
			DeCommand(&SerialString, "==",command,value);
			if((command=="login")&&(!AdminLogin)){
				if(value!=(admin)){
					Serial.println(F("Enter password for login."));
				}else{
					Serial.println(F("You had login lo system"));
					AdminLogin=true;					
				}
			}	
			else if((command=="status")&&AdminLogin){
				
			
			}else{
				Serial.print(F("Command ERROR"));
			}
			SerialString="";
			SerialComplete = false;
		}
	}
}
