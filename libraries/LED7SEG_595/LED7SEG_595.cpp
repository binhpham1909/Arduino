
#include "LED7SEG_595.h"
Led7seg595::Led7seg595(uint8_t data, uint8_t clock, uint8_t latch, uint8_t numOfLed, boolean anotShare=true){
	_data = data;
	_clock = clock;
	_latch = latch;
	pinMode(_data, OUTPUT);
	pinMode(_clock, OUTPUT);
	pinMode(_latch, OUTPUT);
	_maxLed = numOfLed;
	_anotShare = anotShare;
}
void Led7seg595::print(String& input){
	int len = input.length();
	if(len <= _maxLed){
		printOut(input);
	}else{
		String out = input.substring(_lastIdx,_lastIdx + _maxLed);
		printOut(out);
		if(_lastIdx == 0)	_RtoL = true;
		if(_lastIdx == (len - _maxLed))	_RtoL = false;
		if(millis() - _lastTime > 300){
			_lastTime = millis();
			if(_RtoL)	_lastIdx++;
			else _lastIdx--;			
		}
	}
}
void Led7seg595::printOut(String& input){
	int len = input.length();
	int dotPos = input.indexOf('.');
	digitalWrite(_latch, LOW);
	if(dotPos>0){
		for(int i=0; i<dotPos-1; i++){
			shift(getHex(input.charAt(i)));
		}
		uint8_t numS = getHex(input.charAt(dotPos-1));
		bitSet(numS, 0);	// view dot
		shift(numS);
		for(int i=dotPos + 1; i<len; i++){
			shift(getHex(input.charAt(i)));
		}
	}else{
		for(int i=0; i<len; i++){
			shift(getHex(input.charAt(i)));
		}		
	}
	digitalWrite(_latch, HIGH);
}
void Led7seg595::shift(uint8_t txt){
	if(_anotShare)	shiftOut(_data, _clock, MSBFIRST, ~txt); // anot chung
	else	shiftOut(_data, _clock, MSBFIRST, txt); // catot chung	
}
uint8_t Led7seg595::getHex(char k){
	if(k == 48){
		return ledHEX[0];
	}else if(k == 49){
		return ledHEX[1];
	}else if(k == 50){
		return ledHEX[2];
	}else if(k == 51){
		return ledHEX[3];
	}else if(k == 52){
		return ledHEX[4];
	}else if(k == 53){
		return ledHEX[5];
	}else if(k == 54){
		return ledHEX[6];
	}else if(k == 55){
		return ledHEX[7];
	}else if(k == 56){
		return ledHEX[8];
	}else if(k == 57){
		return ledHEX[9];
	}else if(k == 65){
		return ledHEX[10];
	}else if(k == 66){
		return ledHEX[11];
	}else if(k == 67){
		return ledHEX[12];
	}else if(k == 68){
		return ledHEX[13];
	}else if(k == 69){
		return ledHEX[14];
	}else if(k == 70){
		return ledHEX[15];
	}else if(k == 71){
		return ledHEX[16];
	}else if(k == 72){
		return ledHEX[17];
	}else if(k == 82){
		return ledHEX[18];
	}else if(k == 45){
		return ledHEX[19];
	}else return ledHEX[20];
}