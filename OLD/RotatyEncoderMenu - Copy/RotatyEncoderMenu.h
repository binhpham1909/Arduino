// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

#ifndef __RotatyEncoderMenu_h__
#define __RotatyEncoderMenu_h__

// ----------------------------------------------------------------------------

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "Arduino.h"

// ----------------------------------------------------------------------------

#define ENC_NORMAL        (1 << 1)   // use Peter Danneger's decoder
#define ENC_FLAKY         (1 << 2)   // use Table-based decoder

// ----------------------------------------------------------------------------

#ifndef ENC_DECODER
#  define ENC_DECODER     ENC_NORMAL
#endif

#if ENC_DECODER == ENC_FLAKY
#  ifndef ENC_HALFSTEP
#    define ENC_HALFSTEP  1        // use table for half step per default
#  endif
#endif

// ----------------------------------------------------------------------------

class RotatyEncoderMenu{
	public:
		typedef enum menuState_e{
			inNone = 0,
			inSubMenu,
			inSubItem,
			inChange,
		} menuState;
		typedef struct menuValue_s{
			boolean event;
			menuState state;
			uint16_t pos;
			uint16_t value;
		} menuValue;
		typedef enum Button_e {
			Open = 0,
			Closed,
			Pressed,
			Held,
			Released,
			Clicked,
			DoubleClicked
		} Button;	
	public:
		RotatyEncoderMenu(uint8_t A, uint8_t B, uint8_t BTN = -1, uint8_t stepsPerNotch = 1, bool active = LOW);
		void service(void);  
		menuValue getValue(void);
		void setSubMenu(uint8_t maxSubMenu);
		void setSubItem(uint8_t pos,uint8_t maxSubItem);
		void goHome(void);
		void setAccelerationEnabled(const bool &a);
		const bool getAccelerationEnabled() ;
#ifndef WITHOUT_BUTTON
	public:
		Button getButton(void);
		void setDoubleClickEnabled(const bool &d){		doubleClickEnabled = d;	}
		const bool getDoubleClickEnabled(){	return doubleClickEnabled;	}
#endif
	private:
		typedef uint8_t * menuList;
		menuList MenuList;
		uint8_t MaxSubMenu;
		uint8_t posSubMenu;
		uint8_t posSubItem;
		menuValue rtMenu;
		boolean waitChange;
		
		const uint8_t pinA;
		const uint8_t pinB;
		const uint8_t pinBTN;
		const bool pinsActive;
		volatile int16_t delta;
		volatile int16_t last;
		uint8_t steps;
		volatile uint16_t acceleration;
#if ENC_DECODER != ENC_NORMAL
		static const int8_t table[16];
#endif
#ifndef WITHOUT_BUTTON
		volatile Button button;
		bool doubleClickEnabled;
		bool accelerationEnabled;
#endif
};

// ----------------------------------------------------------------------------

#endif // __RotatyEncoderMenu_h__
