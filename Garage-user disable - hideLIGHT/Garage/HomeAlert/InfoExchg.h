#ifndef __INFO_EXCHG_H__
#define __INFO_EXCHG_H__

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include "ESP8266WebServerEx.h"
#include "SerialDebug.h"
#include "ModuleSettings.h"

#define UDP_EXCHG_PORT 5001

struct Neighbor {
	int category;
	uint32_t ip;
	uint16_t port;
	time_t sec;
	char name[MODULE_NAME_MAX+1];
};

#define UDP_EXCHG_PACKET_SIZE (sizeof(Neighbor))
#define MAX_NEIGHBOR 50
#define NEIGHBOR_TTL (60*5)
#define NEIGHBOR_UPDATE_TIME (60*1)

class InfoExchg {
protected:
	Neighbor* nb[MAX_NEIGHBOR];
	WiFiUDP udp;
	time_t nextUpdate;
	
	int findNeighbor(uint32_t ip);
	int createNeighbor(const Neighbor* data);
	void ageOut(time_t t);
	bool UpdateNeighbor(Neighbor* data);
	int read(Neighbor* buffer);
	int send(const Neighbor* buffer);
public:
	InfoExchg();
	virtual ~InfoExchg();
	bool Update();
	bool begin();
  void ParseData(ESP8266WebServerEx* server, String& buffer);
};
#endif
