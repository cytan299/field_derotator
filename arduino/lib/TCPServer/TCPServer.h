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

#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include "Adafruit_CC3000.h"
#include "BaseServer.h"
#include "UserIO.h"
#include "DeRotator.h"

/**********************************************************************
NAME

        TCPServer - This class sets up Wifi to listen to user commands
		    sent over TCP. It is a derived class of Server

SYNOPSIS
	TCPServer sets up the Wifi ADAFRUIT Wifi Shield as a TCP
	server to listen to user commands that are sent over TCP.

CONSTRUCTOR

        TCPServer(		- constructor
	  userio		- userio for printing to LCD.
	  derotator		- the derotator
	)	

        
INTERFACE
	Connect(		- connect to the Wifi network
	  ssid			- and connect to this ssid network
	  key			- with this password
	  secmode		- with this security mode
	)			- returns 0 on success

	Disconnect()		- disconnect from wifi
				  network. Returns 0 on success

	ServiceLoop()		- listen for client data packets

	GetIPAddress(		- get the wifi address
	  ipaddress		- and put it into ipaddress
	)			- returns 0 on success

	GetIPAddress(		- get the current Wifi
	  ipaddress		- and put it into ipaddress
	)			- returns 0 on success

	GetSSID(		- get the ssid of the current
	  ssid			- Wifi
	)

	SetSSID(		- set the ssid to
	  ssid			- this string
	)

	GetPass(		- get the current password used
	  pass			- to access the ssid.
	)

	SetPass(		- set the password
	  pass			- to this singt to access the ssid
	)

	GetSecurity()		- return the security type of the ssid

	SetSecurity(		- set the secruity of the ssid
	   sec			- to this value
	)


AUTHOR                                          

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/

// length of the wifi ssid and password limited to 32 bytes
#define WIFI_MAX_STR_LEN	32

using namespace std;

class UserIO;



class TCPServer : public BaseServer
{
public:
  TCPServer(UserIO* userio, DeRotator* derotator);
  ~TCPServer();

 public:
  int Connect(const char* ssid,
	      const char* key,
	      uint8_t secmode
	      );


  int Disconnect();

  int ServiceLoop();

  int GetIPAddress(uint32_t* ipaddress);
  
  void GetSSID(char* ssid) const;
  void SetSSID(char* ssid);
  
  void GetPass(char* pass) const;
  void SetPass(char* pass);
  
  uint8_t GetSecurity() const;
  void SetSecurity(uint8_t sec);

private:
  int display_connection_details();
  
private:
  Adafruit_CC3000 _cc3000;
  Adafruit_CC3000_Server _server;

private:
  char _ssid[WIFI_MAX_STR_LEN];
  char _pass[WIFI_MAX_STR_LEN];
  uint8_t _secmode;

};

#endif
