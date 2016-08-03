// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

#ifndef __RotatyEncoderMenu_h__
#define __RotatyEncoderMenu_h__

// ----------------------------------------------------------------------------

#include "ClickEncoder.h"

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------

class RotatyEncoderMenu: public ClickEncoder{
	public:
		typedef enum menuState_e{
			inNone = 0,
			inSubMenu,
			inSubItem,
			inChange,
			inWaitChange,
		} menuState;
	public:
		RotatyEncoderMenu(uint8_t maxSubMenu, uint8_t A, uint8_t B, uint8_t SPK, uint8_t BTN = -1, uint8_t stepsPerNotch = 1, bool active = LOW):ClickEncoder(A, B, BTN, stepsPerNotch, active){
			MAX_SUBMENU = maxSubMenu;
			menu = new uint8_t[MAX_SUBMENU];
			_spkPin = SPK;
		};
		void setSubItem(uint8_t subMenu, uint8_t maxItem);
		boolean update(void);
		uint16_t getMenuPos(void){ return (posSubMenu*100+posSubItem);};
		int16_t getValueChange(void){	return lastEn;};
		menuState getMenuState(void){	return MENU_STATE;};
		void goHome(void);
	private:
		menuState	MENU_STATE = inNone;
		uint8_t MAX_SUBMENU;
		uint8_t *menu;
		uint8_t posSubMenu;
		uint8_t posSubItem;
		uint8_t _spkPin;
		int16_t lastEn;
};

// ----------------------------------------------------------------------------

#endif // __RotatyEncoderMenu_h__
