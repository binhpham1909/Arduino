#include <TimeLib.h>
#include "NTPClient.h"

CNTPClient::CNTPClient() {
	ntpServerName = "pool.ntp.org";
	packetBuffer = (byte*)malloc(NTP_PACKET_SIZE);
  lastSync = 0;
  resyncPeriod = 0;
}
CNTPClient::~CNTPClient() {
	if ( packetBuffer ) free(packetBuffer);
}

void CNTPClient::SetNTPServer(const char *server) {
  if ( server==NULL || !server[0]) return;
  ntpServerName = server;
}

bool CNTPClient::AutoSync(time_t ifSyncOK, time_t ifSyncFailed){
  if ( now() > resyncPeriod ) {
    if ( Sync() ) {
      resyncPeriod = now() + ifSyncOK;
    }
    else {
      resyncPeriod = now() + ifSyncFailed;
      return false;
    }
  }
  return true;
}

bool CNTPClient::Sync() {
  WiFiUDP udp;
  
  if ( !udp.begin(NTP_LOCAL_PORT) ) {
    TRACE("Cannot listen for UDP");
    return false;
  }
  
  // discard any previously received packets
  while (udp.parsePacket() > 0) delay(0); 
  
  if ( !sendNTPpacket(udp) ) {
    udp.stop();
    TRACE("Cannot send NTP packet");
    return false;
  }

  uint32_t beginWait = millis();
  int size = 0;
  while (millis() - beginWait < 1500) {
    
    size = udp.parsePacket();
    
    if (size >= NTP_PACKET_SIZE) {
      TRACE("-- Receive NTP Response");
      udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      break;
    }
    delay(1);
  }
  udp.stop();

  if (size < NTP_PACKET_SIZE) {
    TRACE("no packet yet");
    return false;
  }

  TRACE2("packet received, length=",size);

  //the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, esxtract the two words:

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  TRACE2("Seconds since Jan 1 1900 = ", secsSince1900);

  // now convert NTP time into everyday time:
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:

  lastSync = secsSince1900 - seventyYears;

  setTime(lastSync);
  
  // print Unix time:
  TRACE2("Unix time = ",lastSync);
  return true;
}

// send an NTP request to the time server at the given address
bool CNTPClient::sendNTPpacket(WiFiUDP& udp)
{
  if ( !packetBuffer ) {
  	TRACE("Error: Cannot allocate memory");
    return false;
  }

  // resolve NTP server name.
  IPAddress address;
  if ( WiFi.hostByName(ntpServerName.c_str(), address)!=1 ) {
    TRACE2("Cannot resolve host: ",ntpServerName);
    return false;
  }
  		
  TRACE2("sending NTP packet...",ntpServerName);
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();

  return true;
}
