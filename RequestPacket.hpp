/* $Id$ */

#ifndef REQUESTPACKET_HPP
#define REQUESTPACKET_HPP

#include <stdint.h>

/**********************************************************************
NAME
	RequestPacket - The data packet that is sent to the server.

SYNOPSIS
	The following fields have the following usage:

	
INTERFACE


AUTHOR
	C.Y. Tan

SEE ALSO
	DataPacket

**********************************************************************/

/*
	Derotator commands are all between 10 to 19
*/
#define DEROTATOR_START			10
#define DEROTATOR_STOP			11
#define DEROTATOR_GOTO_HALL_HOME	12
#define DEROTATOR_GOTO_USER_HOME	13


/*
	Setup commands are all between 20 to 29
*/
#define SETUP_SET_USER_HOME	20
#define SETUP_MAX_CW		21
#define SETUP_MAX_CCW		22
#define SETUP_ENABLE_LIMITS	23
#define SETUP_IS_CLOCKWISE	24
#define SETUP_SAVE_SETTINGS	25
#define SETUP_LOAD_SETTINGS	26
#define SETUP_DEF_SETTINGS	27



/*
	Commands that are not on the Derotator menu
	are all > 100
*/

#define CMD_GET_ALTAZ_ZETA	100
#define CMD_GET_THETA		101
#define CMD_GOTO_THETA		102
#define CMD_QUERY_STATE		103
#define CMD_GET_USER_HOME_POS	104
#define CMD_GET_MAX_CW_POS	105
#define CMD_GET_MAX_CCW_POS	106
#define CMD_SET_WLAN_SSID	107
#define CMD_SET_WLAN_PASS	108
#define CMD_SET_WLAN_SECURITY	109


struct RequestPacket
{
  int16_t _command;
  int16_t _ivalue;
  float _fvalue[3];
  char _buf[32];
};
		   
#endif
