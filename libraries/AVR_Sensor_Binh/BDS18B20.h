// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

#ifndef __BDS18B20_h__
#define __BDS18B20_h__

// ----------------------------------------------------------------------------

#include <OneWire.h>

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------

class BDS18B20: public OneWire{
	public:
		BDS18B20(uint8_t pin):OneWire( pin){	init();};
		void init(void);
		float readTemp(void);
		boolean getError(void);
	private:
		byte Addr[8];
		byte Data[2];
		byte ReadSensorStep=0;
		float LastTemp;
		uint32_t WaitTime, LastRead;
		boolean _err=false;
};

// ----------------------------------------------------------------------------

#endif // __RotatyEncoderMenu_h__
