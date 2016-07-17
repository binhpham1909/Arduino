
#include "BTone.h"
void BTone::service(void){
	if(millis()<StopTime){
		State != State;
		digitalWrite(Pin, State);
	}else{
		State = LOW;
		digitalWrite(Pin, State);		
	}
};
void BTone::tone(int duration){
	StopTime = millis() + duration;
};
void BTone::noTone(void){
	StopTime = millis();
	State = LOW;
	digitalWrite(Pin, State);	
};