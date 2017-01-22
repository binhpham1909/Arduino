// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

#ifndef __BMacros_h__
#define __BMacros_h__

// ----------------------------------------------------------------------------

#include <Arduino.h>

// ----------------------------------------------------------------------------
#define BAT_RELAY(X) digitalWrite(X, LOW)  
#define TAT_RELAY(X) digitalWrite(X, HIGH) 



// Ham noi suy tuyen tinh theo 2 diem gan nhat
double map(double x, double ca[10]);

// extraxt key value from String type {key}={val}
int deValue(String& inPut, String& outPut);

int getMemoryFree();
// ----------------------------------------------------------------------------

#endif // __BMacros_h__
