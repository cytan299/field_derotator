/*$Id$*/
/*
    derot is the controller code for the Arduino MEGA2560
    Copyright (C) 2015  C.Y. Tan
    Contact: cytan299@yahoo.com

    This file is part of derot

    derot is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    derot is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with derot.  If not, see <http://www.gnu.org/licenses/>.

*/
/* operating system header files (use <> for make depend) */
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <Arduino.h>
#include <stdlib.h>
#include <math.h>
#include "utility/debug.h"

/* general system header files (use "" for make depend) */

/* local include files (use "") */
#include "TCPServer.h"
#include "RequestPacket.hpp"
#include "ReplyPacket.hpp"
#include "StatusPacket.hpp"

// These are the interrupt and control pins
#define CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define CC3000_VBAT  5
#define CC3000_CS    10

/**********************************************************************
NAME
        TCPServer - This class sets up Wifi to listen to user commands
		    sent over TCP

SYNOPSIS
	See TCPServer.hpp

                                                
PROTECTED FUNCTIONS

PRIVATE FUNCTIONS

	display_connection_details() - send the Wifi details back to the user via Serial.


LOCAL TYPES AND CLASSES

AUTHOR

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/


TCPServer::TCPServer(UserIO* userio, DeRotator* derotator)
  :BaseServer(userio, derotator),
  _cc3000(CC3000_CS, CC3000_IRQ, CC3000_VBAT, SPI_CLOCK_DIVIDER),
   _server(5001)
{
  _ssid[0] = '\0';
  _pass[0] = '\0';
  _secmode = WLAN_SEC_UNSEC;
}

TCPServer::~TCPServer()
{
  _cc3000.disconnect();
}

int TCPServer::Connect(const char* ssid, const char* pass, uint8_t secmode)
{
  Serial.println(F("\nTCPServer::Connect: Initialising the CC3000 ..."));
  Serial.flush();
  if (!_cc3000.begin()) {
    Serial.println(F("TCPServer::Connect: cannot initialize Wifi shield"));
    while(1);
  }

  Serial.println(F("\nDeleting old connection profiles"));
  Serial.flush();  
  if (!_cc3000.deleteProfiles()) {
    Serial.println(F("TCPServer:::Connect(): Failed!"));
    return -1;
  }

  Serial.print(F("\nTCPServer: Attempting to connect to ")); Serial.println(ssid);
  Serial.flush();  
  if (!_cc3000.connectToAP(ssid, pass, secmode, 10)) {
    Serial.println(F("TCPServer::Connect(): Failed!"));
    return -1;
  }
  
  Serial.println(F("TCPServer::Connect: Connected!"));
  Serial.flush();  
  /* Wait for DHCP to complete */
  Serial.println(F("TCPServer::Connect(): Request DHCP"));
  Serial.flush();  
  while (!_cc3000.checkDHCP())
  {
    if(_userio){
      _userio->PrintProgressWheel();
    }
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  /* Display the IP address DNS, Gateway, etc. */  
  while (display_connection_details() != 0) {
    delay(1000);
  }

  /* make a copy of the ssid, pass and security mode */
  if(strlen(ssid) < WIFI_MAX_STR_LEN-1){
    for(int i=0; i< WIFI_MAX_STR_LEN-1; i++){
      _ssid[i] = ssid[i];
    }
    _ssid[WIFI_MAX_STR_LEN-1] = '\0';
  }
  else {
    Serial.println(F("TCPServer:::Connect(): ssid too long"));      
    return -1;
  }

  if(strlen(pass) < WIFI_MAX_STR_LEN-1){
    for(int i=0; i< WIFI_MAX_STR_LEN-1; i++){
      _pass[i] = pass[i];
    }
    _pass[WIFI_MAX_STR_LEN-1] = '\0';
  }
  else {
    Serial.println(F("TCPServer:::Connect(): password too long"));      
    return -1;
  }

  _secmode = secmode;

  _server.begin();
  Serial.println(F("Listening for connections"));

  Serial.flush();
  
  return 0;
}

int TCPServer::Disconnect()
{
  return _cc3000.disconnect()? 0:-1;
}

int TCPServer::ServiceLoop()
{
  RequestPacket rq;
  ReplyPacket rp;
  StatusPacket sp;
  
  int sz;
  
  Adafruit_CC3000_ClientRef client = _server.available();
  if (client) {
    // Check if there is data available to read.
    if (client.available() > 0) {
      if(sz = client.read(static_cast<void*>(&rq), sizeof(RequestPacket))){
	#ifdef AAAAAA
	Serial.print("rq._command = ");
	Serial.println(rq._command);
	#endif
      }

      if(ServiceRequests(&rq, &rp, &sp) == 0){
	if(rq._command != CMD_QUERY_STATE){

	  char* packet_ptr = (char*)&rp;
	  char* packet_end = packet_ptr + sizeof(ReplyPacket);
	  int sz = sizeof(ReplyPacket);
	  int sent_sz;	  
	  do{
	    sent_sz += client.write(static_cast<void*>(packet_ptr), sz);
	    packet_ptr += sent_sz;
	    sz -= sent_sz;
	  } while(sz > 0);
	  
	}
	else {
	  char* packet_ptr = (char*)&sp;
	  char* packet_end = packet_ptr + sizeof(StatusPacket);
	  int sz = sizeof(StatusPacket);
	  int sent_sz;
	  do{
	    sent_sz += client.write(static_cast<void*>(packet_ptr), sz);
	    packet_ptr += sent_sz;
	    sz -= sent_sz;
	  } while(sz > 0);
	}
      } // ServiceRequests
      else {
	Serial.println(F("TCPServer::ServiceLoop: ServiceRequests() failed"));
	return -1;
      }
    } // client.available()
  } // client

  return 0;
}



int TCPServer::GetIPAddress(uint32_t* ip_address)
{
  uint32_t netmask, gateway, dhcpserv, dnsserv;
  
  if(!_cc3000.getIPAddress(ip_address, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("TCPServer: Unable to retrieve the IP Address!\r\n"));
    return -1;
  }
  return 0;
}


void TCPServer::GetSSID(char* ssid) const
{
  for(int i=0; i<WIFI_MAX_STR_LEN; i++){
    ssid[i] = _ssid[i];
  }
}

void TCPServer::SetSSID(char* ssid)
{
  strcpy(_ssid, ssid);
  _ssid[strlen(ssid)] = '\0';
}

void TCPServer::GetPass(char* pass) const
{
  for(int i=0; i<WIFI_MAX_STR_LEN; i++){
    pass[i] = _pass[i];
  }
}

void TCPServer::SetPass(char* pass)
{
  strcpy(_pass, pass);
  _pass[strlen(pass)] = '\0';  
}

uint8_t TCPServer::GetSecurity() const
{
  return _secmode;
}

void TCPServer::SetSecurity(uint8_t secmode)
{
  _secmode = secmode;
}


int TCPServer::display_connection_details()
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!_cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("TCPServer: Unable to retrieve the IP Address!\r\n"));
    return -1;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); _cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); _cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); _cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); _cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); _cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    Serial.flush();
    return 0;
  }
}

