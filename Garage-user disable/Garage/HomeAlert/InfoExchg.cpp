#include "InfoExchg.h"

extern ModuleSettings g_ModuleSettings;

InfoExchg::InfoExchg() {
	for (int i=0; i<MAX_NEIGHBOR; i++) {
	  nb[i]=NULL;
	  delay(0);
	}
	nextUpdate = 0;
}

InfoExchg::~InfoExchg() {
	for (int i=0; i<MAX_NEIGHBOR; i++) {
	  delay(0);
	  if (nb[i]) delete nb[i];
    }
}

int InfoExchg::findNeighbor(uint32_t ip) {
	for (int i=0; i<MAX_NEIGHBOR; i++) {
		delay(0);
		if (nb[i]!=NULL && nb[i]->ip==ip) return i;
	}
	return -1;
}

int InfoExchg::createNeighbor(const Neighbor* data) {
	for (int i=0; i<MAX_NEIGHBOR; i++) {
		delay(0);
		if (nb[i]==NULL) {
			nb[i] = new Neighbor;
			if ( nb[i]!=NULL ) {
				*(nb[i]) = *data;
				return i;
			}
			else return -1;
		}
	}
	return -1;
}

bool InfoExchg::UpdateNeighbor(Neighbor* data) {
	if ( !data ) return false;
	int idx = findNeighbor(data->ip);
	if ( idx<0 ) return false;
	*(nb[idx]) = *data;
	nb[idx]->sec = now();
	return true;
}

bool InfoExchg::Update() {
	time_t t = now();
	Neighbor me;
	bool bAgeOut = false;
	
	if ( read(&me) ) {
    TRACE("Received UDP");
		if ( t<1472638685 && me.sec>1472638685 ) {
      TRACE("Update time from neighbor");
			setTime(me.sec);
		}
		if  ( !UpdateNeighbor(&me) ) {
			bAgeOut = true;
			ageOut(t);
			createNeighbor(&me);
		}
	}
	
	if ( t>=nextUpdate ) {
		if ( !bAgeOut ) ageOut(t);
		
    nextUpdate = (t+NEIGHBOR_UPDATE_TIME);
		
		me.category = g_ModuleSettings.data.Category;
		me.port = g_ModuleSettings.data.port;
		me.sec = t;
		me.ip = (uint32_t)(WiFi.localIP());
		strncpy(me.name, g_ModuleSettings.data.ModuleName, MODULE_NAME_MAX);
		
		if ( !UpdateNeighbor(&me) ) createNeighbor(&me);
		
		return send(&me);
	}
	return true;
}

void InfoExchg::ageOut(time_t t) {
	for (int i=0; i<MAX_NEIGHBOR; i++) {
	  delay(0);
	  if (nb[i]) {
		if ( nb[i]->sec+NEIGHBOR_TTL<t ) {
			delete nb[i];
			nb[i] = NULL;
		}
	  }
    }
}

bool InfoExchg::begin() {
	//IPAddress broadcastIp = ~WiFi.subnetMask() | WiFi.gatewayIP();
	if ( !udp.begin(UDP_EXCHG_PORT) ) {
	  TRACE("UDP Listen Failed");
	  return false;
	}

  TRACE("UDP Listen succesffully");
	return true;
}

int InfoExchg::read(Neighbor* buffer) {
	if ( buffer==NULL ) return 0;

	int cb = udp.parsePacket();
	if ( cb >=UDP_EXCHG_PACKET_SIZE ) {
		return udp.read((char*)buffer,UDP_EXCHG_PACKET_SIZE)==UDP_EXCHG_PACKET_SIZE;
	}
	return 0;
}

int InfoExchg::send(const Neighbor* buffer) {
	IPAddress broadcastIp = ~WiFi.subnetMask() | WiFi.gatewayIP();

  TRACE("Send UDP");
	udp.beginPacket(broadcastIp, UDP_EXCHG_PORT);
	int rsl = udp.write((char*)buffer, UDP_EXCHG_PACKET_SIZE);
	udp.endPacket();
	return rsl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// receive: {cat: 1}
// return: [{"name":"Living Room","ip":"192.168.1.1","port":8080},{name:"Bed Room",ip:"192.168.1.2",port:8080}]
//////////////////////////////////////////////////////////////////////////////////////////////////
void InfoExchg::ParseData(ESP8266WebServerEx* server, String& buffer) {
  int cat = atoi(server->arg("cat").c_str());
  int i;
  int count = 0;
  String localAddress = server->GetHost();
  bool bRemote = false;

  i = localAddress.indexOf(":");
  if ( i>0 ) localAddress = localAddress.substring(0,i);
  TRACE2("ParseData local IP: ",localAddress);
  IPAddress localIp;

  if ( WiFi.hostByName(localAddress.c_str(), localIp)!=1 ) localIp = WiFi.localIP();
  if ( localIp != WiFi.localIP() ) bRemote = true;
  
  buffer = "[";
  for (i=0; i<MAX_NEIGHBOR; i++) {
  	delay(0);
    if ( nb[i] && nb[i]->category==cat ) {
      if ( count>0 ) buffer += ",";
      buffer += "{\"name\":\"";
      buffer += nb[i]->name;
      buffer += "\",\"ip\":\"";
      if ( bRemote ) {
        buffer += localIp.toString();
      } else buffer += IPAddress(nb[i]->ip).toString();      
      buffer +="\",\"port\":";
      buffer += nb[i]->port;
      buffer += "}";
      count++;
    }
  }
  buffer += "]";
}

