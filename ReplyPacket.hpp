/* $Id$ */

#ifndef REPLYPACKET_HPP
#define REPLYPACKET_HPP

#include <stdint.h>

/**********************************************************************
NAME
	ReplyPacket - The data packet that is sent from the server. 

SYNOPSIS

	The data packet that is sent from the server. 			  

AUTHOR
	C.Y. Tan

SEE ALSO


**********************************************************************/

#define REPLY_OK	0
#define REPLY_INT16	1
#define REPLY_FLOAT	2



struct ReplyPacket
{
  int16_t _reply;
  int16_t _ivalue;
  float _fvalue[4];
};
		   
			

#endif
