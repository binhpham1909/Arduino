/* DHT library

MIT license
written by Adafruit Industries
*/
//Updated by S.Antonio
#include "DHT_STM.h"

#define MIN_INTERVAL 2000

DHT::DHT(uint8_t pin, uint8_t type, uint8_t count) {
	_pin = pin;
	_type = type;
	_bit = digitalPinToBitMask(pin);
	//_port = digitalPinToPort(pin);
	//_maxcycles = 72000L;//microsecondsToClockCycles(1000);  // 1 millisecond timeout for
	_maxcycles = F_CPU/1000;//suggested by RogerClark.
												 // reading pulses from DHT sensor.
	// Note that count is now ignored as the DHT reading algorithm adjusts itself
	// basd on the speed of the processor.
}

void DHT::begin(void) {
	// set up the pins!
	pinMode(_pin, INPUT_PULLUP);
	// Using this value makes sure that millis() - lastreadtime will be
	// >= MIN_INTERVAL right away. Note that this assignment wraps around,
	// but so will the subtraction.
	_lastreadtime = -MIN_INTERVAL;
	DEBUG_PRINT("Max clock cycles: "); DEBUG_PRINTLN(_maxcycles, DEC);
}

//boolean S == Scale.  True == Fahrenheit; False == Celcius
float DHT::readTemperature(bool S, bool force) {
	float f = NAN;
	if (read(force)) {
		switch (_type) {
			case DHT11:
				f = data[2];
				if(S) {
					f = convertCtoF(f);
				}
			break;
			case DHT22:
			case DHT21:
				f = data[2] & 0x7F;
				f *= 256;
				f += data[3];
				f /= 10;
				if (data[2] & 0x80) {	f *= -1;}
				if(S) {	f = convertCtoF(f);}
			break;
		}
	}
	return f;
}

float DHT::convertCtoF(float c) {
	return c * 9 / 5 + 32;
}

float DHT::convertFtoC(float f) {
	return (f - 32) * 5 / 9;
}

float DHT::readHumidity(bool force) {
	float f = NAN;
	if (read()) {
		switch (_type) {
			case DHT11:
			  f = data[0];
			  break;
			case DHT22:
			case DHT21:
			  f = data[0];
			  f *= 256;
			  f += data[1];
			  f /= 10;
			  break;
		}
	}
	return f;
}

//boolean isFahrenheit: True == Fahrenheit; False == Celcius
float DHT::computeHeatIndex(float temperature, float percentHumidity, bool isFahrenheit) {
  // Adapted from equation at: https://github.com/adafruit/DHT-sensor-library/issues/9 and
  // Wikipedia: http://en.wikipedia.org/wiki/Heat_index
  if (!isFahrenheit) {
    // Celsius heat index calculation.
    return -8.784695 +
             1.61139411 * temperature +
             2.338549   * percentHumidity +
            -0.14611605 * temperature*percentHumidity +
            -0.01230809 * pow(temperature, 2) +
            -0.01642482 * pow(percentHumidity, 2) +
             0.00221173 * pow(temperature, 2) * percentHumidity +
             0.00072546 * temperature*pow(percentHumidity, 2) +
            -0.00000358 * pow(temperature, 2) * pow(percentHumidity, 2);
  }else {
    // Fahrenheit heat index calculation.
    return -42.379 +
             2.04901523 * temperature +
            10.14333127 * percentHumidity +
            -0.22475541 * temperature*percentHumidity +
            -0.00683783 * pow(temperature, 2) +
            -0.05481717 * pow(percentHumidity, 2) +
             0.00122874 * pow(temperature, 2) * percentHumidity +
             0.00085282 * temperature*pow(percentHumidity, 2) +
            -0.00000199 * pow(temperature, 2) * pow(percentHumidity, 2);
  }
}

boolean DHT::read(bool force) {
	// Check if sensor was read less than two seconds ago and return early
	// to use last reading.
	uint8_t datat[5];
	uint32_t cycles[80];
	// Reset 40 bits of received data to zero.
	datat[0] = datat[1] = datat[2] = datat[3] = datat[4] = 0;
	uint32_t currenttime = millis();
	if (!force && ((currenttime - _lastreadtime) < 2000)) {
		return _lastresult; // return last correct measurement
	}
	if((millis()-_lastStepTime)<_nextDelay) return _lastresult;
	// Send start signal.  See DHT datasheet for full signal diagram:
	//   http://www.adafruit.com/datasheets/Digital%20humidity%20and%20temperature%20sensor%20AM2302.pdf
	if(_step==0){
		_lastreadtime = currenttime;
		// Go into high impedence state to let pull-up raise data line level and
		// start the reading process.
		digitalWrite(_pin, HIGH);
		_lastStepTime = millis();
		_step++;
		_nextDelay = 250;
	}else if(_step == 1){
		// First set data line low for 20 milliseconds.
		pinMode(_pin, OUTPUT);
		digitalWrite(_pin, LOW);
		_lastStepTime = millis();
		_step++;
		_nextDelay = 20;		
	}else if(_step == 2){
		// Turn off interrupts temporarily because the next sections are timing critical
		// and we don't want any interruptions.
		InterruptLock lock;

		// End the start signal by setting data line high for 40 microseconds.
		digitalWrite(_pin, HIGH);
		delayMicroseconds(40);

		// Now start reading the data line to get the value from the DHT sensor.
		pinMode(_pin, INPUT_PULLUP);
		delayMicroseconds(10);  // Delay a bit to let sensor pull data line low.

		// First expect a low signal for ~80 microseconds followed by a high signal
		// for ~80 microseconds again.
		if (expectPulse(LOW) == 0) {
			DEBUG_PRINTLN(F("Timeout waiting for start signal low pulse."));
			_lastresult = false;
			_lastStepTime = millis();
			_step=0;
			_nextDelay = 0;
			return _lastresult;
		}
		if (expectPulse(HIGH) == 0) {
			DEBUG_PRINTLN(F("Timeout waiting for start signal high pulse."));
			_lastresult = false;
			_lastStepTime = millis();
			_step=0;
			_nextDelay = 0;
			return _lastresult;
		}

		// Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
		// microsecond low pulse followed by a variable length high pulse.  If the
		// high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
		// then it's a 1.  We measure the cycle count of the initial 50us low pulse
		// and use that to compare to the cycle count of the high pulse to determine
		// if the bit is a 0 (high state cycle count < low state cycle count), or a
		// 1 (high state cycle count > low state cycle count). Note that for speed all
		// the pulses are read into a array and then examined in a later step.
		for (int i=0; i<80; i+=2) {
			cycles[i]   = expectPulse(LOW);
			cycles[i+1] = expectPulse(HIGH);
		}	
		// Timing critical code is now complete.
		// Inspect pulses and determine which ones are 0 (high state cycle count < low
		// state cycle count), or 1 (high state cycle count > low state cycle count).
		for (int i=0; i<40; ++i) {
			uint32_t lowCycles  = cycles[2*i];
			uint32_t highCycles = cycles[2*i+1];
			if ((lowCycles == 0) || (highCycles == 0)) {
				DEBUG_PRINTLN(F("Timeout waiting for pulse."));
				_lastresult = false;
				_lastStepTime = millis();
				_step=0;
				_nextDelay = 0;
				return _lastresult;
			}
			datat[i/8] <<= 1;
			// Now compare the low and high cycle times to see if the bit is a 0 or 1.
			if (highCycles > lowCycles) {
			  // High cycles are greater than 50us low cycle count, must be a 1.
			  datat[i/8] |= 1;
			}
			// Else high cycles are less than (or equal to, a weird case) the 50us low
			// cycle count so this must be a zero.  Nothing needs to be changed in the
			// stored data.
		}
		DEBUG_PRINTLN(F("Received:"));
		DEBUG_PRINT(datat[0], HEX); DEBUG_PRINT(F(", "));
		DEBUG_PRINT(datat[1], HEX); DEBUG_PRINT(F(", "));
		DEBUG_PRINT(datat[2], HEX); DEBUG_PRINT(F(", "));
		DEBUG_PRINT(datat[3], HEX); DEBUG_PRINT(F(", "));
		DEBUG_PRINT(datat[4], HEX); DEBUG_PRINT(F(" =? "));
		DEBUG_PRINTLN((data[0] + data[1] + data[2] + data[3]) & 0xFF, HEX);
		// Check we read 40 bits and that the checksum matches.
		if (datat[4] == ((datat[0] + datat[1] + datat[2] + datat[3]) & 0xFF)) {
			data[0] = datat[0];
			data[1] = datat[1];
			data[2] = datat[2];
			data[3] = datat[3];
			_lastresult = true;
			_lastStepTime = millis();
			_step=0;
			_nextDelay = 0;
			return _lastresult;
		}else {
			DEBUG_PRINTLN(F("Checksum failure!"));
			_lastresult = false;
			_lastStepTime = millis();
			_step=0;
			_nextDelay = 0;
			return _lastresult;
		}
	}
}

// Expect the signal line to be at the specified level for a period of time and
// return a count of loop cycles spent at that level (this cycle count can be
// used to compare the relative time of two pulses).  If more than a millisecond
// ellapses without the level changing then the call fails with a 0 response.
// This is adapted from Arduino's pulseInLong function (which is only available
// in the very latest IDE versions):
//   https://github.com/arduino/Arduino/blob/master/hardware/arduino/avr/cores/arduino/wiring_pulse.c
uint32_t DHT::expectPulse(bool level) {
	uint32_t count = 0;
	// On AVR platforms use direct GPIO port access as it's much faster and better
	// for catching pulses that are 10's of microseconds in length:
	#ifdef __AVR
	uint8_t portState = level ? _bit : 0;
	//while ((*portInputRegister(_port) & _bit) == portState) {
	while ((*portInputRegister(digitalPinToPort(pin)) & _bit) == portState) {   
		if (count++ >= _maxcycles) {
			return 0; // Exceeded timeout, fail.
		}
	}
	// Otherwise fall back to using digitalRead (this seems to be necessary on ESP8266
	// right now, perhaps bugs in direct port access functions?).
	#else
	while (digitalRead(_pin) == level) {
		if (count++ >= _maxcycles) {
			return 0; // Exceeded timeout, fail.
		}
	}
	#endif
	return count;
}
