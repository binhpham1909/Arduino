// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#include "RotatyEncoderMenu.h"

void RotatyEncoderMenu::setSubItem(uint8_t subMenu, uint8_t maxItem){
	menu[subMenu] = maxItem;
};

boolean RotatyEncoderMenu::update(void){
	Button b = getButton();
	uint16_t valEn = getValue();
//	Serial.print(F("Menu pos:"));	Serial.println(MENU_STATE);
//	Serial.print(F("Valen :"));	Serial.println(MENU_STATE);
	if(MENU_STATE == inWaitChange){	// if wait for change value, irgrone all action click
		return 1;
	}
	if ((b != Open)&&(b==Clicked)) {
		if(MENU_STATE == inNone){
			tone(_spkPin,500,50);
			MENU_STATE = inSubMenu;
			posSubMenu = 1;
		}else if(MENU_STATE == inSubMenu){
			tone(_spkPin,500,50);
			MENU_STATE = inSubItem;
		}else if(MENU_STATE == inSubItem){
			tone(_spkPin,500,300);
			MENU_STATE = inChange;
		}else if(MENU_STATE == inChange){
			tone(_spkPin,500,50);
			MENU_STATE = inWaitChange;
		}
	};
	if(MENU_STATE == inNone){
		lastEn = 0;
	}else if(MENU_STATE == inSubMenu){
		posSubMenu = (int)(posSubMenu + valEn)%MAX_SUBMENU;
		if(!posSubMenu) posSubMenu=1;
	}else if(MENU_STATE == inSubItem){
		posSubItem = (int)(posSubItem + valEn)%menu[posSubMenu];
	}else if(MENU_STATE == inChange){
		lastEn += valEn;
	}
	return 0;
};
void RotatyEncoderMenu::goHome(void){
	MENU_STATE = inNone;
	lastEn = 0;
	posSubMenu = 0;
	posSubItem = 0;
};