#ifndef ___SERIAL_DEBUG__H__
#define ___SERIAL_DEBUG__H__

#define _DEBUG_

#ifdef _DEBUG_

#define INIT_SERIAL_DEBUG() Serial.begin(115200)
#define TRACE0(x) Serial.print(x)
#define TRACE(x) Serial.println(x)
#define TRACE2(x,y) { Serial.print(x); Serial.println(y); }

#else

#define INIT_SERIAL_DEBUG()
#define TRACE0(x) 
#define TRACE(x) 
#define TRACE2(x,y)

#endif

#endif

