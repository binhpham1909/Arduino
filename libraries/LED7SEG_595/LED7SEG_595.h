// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

#ifndef __Led7seg595_h__
#define __Led7seg595_h__

// ----------------------------------------------------------------------------

#include <Arduino.h>

/* ----------------------------------------------------------------------------
HC595	LED7seg Anod	LED7seg Catod
A			A				A
B			B				B
C			C				C
D			D				D
E			E				E
F			F				F
G			G				G
H			DOT				DOT
// ----------------------------------------------------------------------------*/


class Led7seg595{
	public:
		Led7seg595(uint8_t data, uint8_t clock, uint8_t latch, uint8_t numOfLed, boolean anotShare);
		void print(String& input);
	private:
		void printOut(String& input);
		void shift(uint8_t txt);
		uint8_t getHex(char k);
		uint8_t ledHEX[21]= {0x77, 0x41, 0x3B, 0x6B, 0x4D, 0x6E, 0x7E, 0x47, 0x7F, 0x6F, 0x5F, 0x7C, 0x36, 0x79, 0x3E, 0x1E, 0x76, 0x5D, 0x18, 0x08, 0x00};
////////////////////////////  0,   1,	 2,    3, 	 4,     5, 	 6,    7, 	 8, 	9, 	 A,    B, 	 C,    D,    E,    F,    G,    H,	 R,     -,  NULL
		uint8_t	_data, _clock, _latch;
		int _maxLed;
		boolean _anotShare;
		int _lastIdx = 0;
		uint32_t _lastTime;
		boolean _RtoL = true;
};

// ----------------------------------------------------------------------------

#endif // __Led7seg595_h__
