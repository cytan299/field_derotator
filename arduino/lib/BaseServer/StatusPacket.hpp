/* $Id$ */
/*
    derot is the GUI frontend that controls the field derotator
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

#ifndef STATUSPACKET_HPP
#define STATUSPACKET_HPP

#include <stdint.h>

/**********************************************************************
NAME
	StatusPacket - The status packet that contains the current or
		       new status for the DeRotator hardware.

SYNOPSIS

	The status packet of the DeRotator hardware that is sent to/from
	the server and the computer control program

AUTHOR
	C.Y. Tan

SEE ALSO


**********************************************************************/

#define WLAN_UNSECURE	0
#define WLAN_WEP	1
#define WLAN_WPA	2
#define WLAN_WPA2	3

#define REPLY_IS_DEROTATING	10
#pragma pack(push, 1) // exact fit - no padding
struct StatusPacket
{
  int16_t _reply;
  // derotator data
  int16_t _is_clockwise_correction;
  int16_t _home_pos;
  int16_t _max_cw;
  int16_t _max_ccw;
  int16_t _is_enable_limits;
  // present position of camera
  float _angle;
  float _accumulated_angle;
  // WLAN data  
  char _WLAN_ssid[32];
  char _WLAN_password[32];
  int16_t _WLAN_security;

  // Earth's rotation rate
  float _omega;
};
		   
#pragma pack(pop) //back to whatever the previous packing mode was 
			

#endif
