// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#include "DS18B20_STM.h"
boolean BDS18B20::init(void){
	if ( !search(Addr)) {
		Serial.println(F("Inside SS Err"));
        return false;
    }else{
        Serial.print(F("SS Addr= "));
        for(byte i = 0; i < 8; i++) {
            Serial.print(F(" "));
            Serial.print(Addr[i], HEX);
        }
        Serial.println(F("."));
		
		return true;
    }
};
float BDS18B20::readTemp(){
    if((millis()-LastRead)<WaitTime) return LastTemp;
    if(ReadSensorStep==0){
        reset();
        select(Addr);
        write(0x44); // start conversion, read temperature and store it in the scratchpad
        
        LastRead = millis();  
        WaitTime = 450;  // wait for convertion    
        ReadSensorStep++;
    }else if(ReadSensorStep==1){
        byte present = reset();  //now we can read the temp sensor data
        select(Addr);
        write(0xBE); // Read Scratchpad
        for (int i = 0; i < 2; i++) { // Only read the bytes you need? there is more there
            Data[i] = read();
        }
        byte MSB = Data[1];
        byte LSB = Data[0];
        LastTemp = ((MSB << 8) | LSB); //using two’s compliment
        LastTemp = LastTemp / 16; //this converts to C
            
        LastRead = millis();  
        WaitTime = 0;      
        ReadSensorStep=0;  
    }
	return LastTemp;
};