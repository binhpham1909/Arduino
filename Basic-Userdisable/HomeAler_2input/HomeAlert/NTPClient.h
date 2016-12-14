#ifndef _NTP_CLIENT_H_
#define _NTP_CLIENT_H_

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "SerialDebug.h"

#define NTP_LOCAL_PORT 2395      // local port to listen for UDP packets
#define NTP_PACKET_SIZE 48 // NTP time stamp is in the first 48 bytes of the message

class CNTPClient {
protected:
	String ntpServerName;
	byte* packetBuffer;
  time_t lastSync;
  time_t resyncPeriod;
public:
  bool AutoSync(time_t ifSyncOK = 14400, time_t ifSyncFailed = 300);
  void SetNTPServer(const char *server);
	CNTPClient();
	~CNTPClient();
  bool Sync();
	
	bool sendNTPpacket(WiFiUDP& udp);
};

#endif

