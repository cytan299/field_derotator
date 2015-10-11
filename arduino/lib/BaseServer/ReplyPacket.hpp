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

/*
  Derotator continue status is sent back in ReplyPacket::_reply
*/

#define REPLY_DEROTATOR_STEPSIZE_ERR	-1
#define REPLY_DEROTATOR_LIMITS_REACHED	-2


struct ReplyPacket
{
  int16_t _reply;
  int16_t _ivalue;
  float _fvalue[4];
};
		   
			

#endif
