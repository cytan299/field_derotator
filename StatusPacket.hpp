/* $Id$ */

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
  /*
    I have to add this padding because
    with or without this padding,
	sizeof(StatusPacket) = 88 on Mac
    but
	sizeof(StatusPacket) = 86 without this padding on Arduino
	sizeof(StatusPacket) = 88 with this padding on Arduino
   */
  int16_t _padding2; 
};
		   
			

#endif
