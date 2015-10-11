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
#include <Arduino.h>
#include <stdlib.h>
#include <math.h>

/* general system header files (use "" for make depend) */

/* local include files (use "") */
#include "SerialServer.h"
#include "RequestPacket.hpp"
#include "ReplyPacket.hpp"
#include "StatusPacket.hpp"

/**********************************************************************
NAME
        SerialServer - This class sets up serial port 0 to listen to
		       user commands

SYNOPSIS
	See SerialServer.hpp

                                                
PROTECTED FUNCTIONS

PRIVATE FUNCTIONS


LOCAL TYPES AND CLASSES

AUTHOR

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/


SerialServer::SerialServer(UserIO* userio, DeRotator* derotator)
  : BaseServer(userio, derotator)
{
  Serial.begin(115200);
}

SerialServer::~SerialServer()
{
  //  Serial.close();
}

int SerialServer::ServiceLoop()
{
  RequestPacket rq;
  ReplyPacket rp;
  StatusPacket sp;
  
  int sz;

  // Check if there is data available to read.
  if (Serial.available() > 0) {
    if(sz = Serial.readBytes((char*)(&rq), sizeof(RequestPacket))){
      // for debugging
    }

    if(ServiceRequests(&rq, &rp, &sp) == 0){
      if(rq._command != CMD_QUERY_STATE){

	if(Serial.write((char*)(&rp),
			sizeof(ReplyPacket)) != sizeof(ReplyPacket)){
	  Serial.println(F("SerialServer::ServiceLoop: did not write the entire reply packet"));
	  return -1;
	}	  
      }
      else {
	// Serial only supports sending packets in 64 byte chunks
	// without editing the default
	// hardware/arduino/cores/arduino/HardwareSerial.cpp file.	
	// #define SERIAL_BUFFER_SIZE	64
	// Thus I'm going to send the StatusPacket back in chunks.
	#define SERIAL_BUFFER_SIZE	64
	
	char* packet_ptr = (char*)&sp;
	char* packet_end = packet_ptr + sizeof(StatusPacket);
	while((packet_ptr + SERIAL_BUFFER_SIZE) < packet_end){
	  if(Serial.write(packet_ptr,
			  SERIAL_BUFFER_SIZE) != SERIAL_BUFFER_SIZE){
	    Serial.println(F("SerialServer::ServiceLoop: did not write the entire reply packet"));
	    return -1;
	  }
	  packet_ptr += SERIAL_BUFFER_SIZE;
	}
        // now send off the remainder
	if(packet_ptr < packet_end){
	  int sz = packet_end - packet_ptr;
	  int sz1;

	  if(Serial.write(packet_ptr, sz) != sz){
	    Serial.println(F("SerialServer::ServiceLoop: did not write the entire reply packet"));
	    return -1;
	  }
	}
      }
    } // ServiceRequests
    else {
      Serial.println(F("SerialServer::ServiceLoop: ServiceRequests() failed"));
      return -1;
    }
  }  // Serial.available()
   
  Serial.flush();

  return 0;
}





