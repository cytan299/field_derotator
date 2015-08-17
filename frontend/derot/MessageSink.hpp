/*$Id$*/
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

#ifndef MESSAGESINK_HPP
#define MESSAGESINK_HPP

/**********************************************************************
NAME
	MessageSink - struct to allow the Message window of the
		      DeRotator frontend to show the boost logger
		      messages. 


SYNOPSIS
	MessageSink is a struct that allows the boost logger to print
	out messages to the Message window of the DeRotator frontend.

	Nearly everything in this struct is static.


CONSTRUCTOR
   	MessageSink(
		 text_display	- pointer to the Text_Display window
			   	  that will display the boost logger messages.	
		)

INTERFACE

	Init(			- initialize the static variables
		display		- pointer to Message text display window	
		message		- pointer to Message text buffer 
	)

	Write(			- write this message to the Message window
	  message		- Use this so that the colours don't get messed
	)			  up with subsequent LOG_*
				  writes. Note: Write() does not write
				  to the log file.


AUTHOR
	C.Y. Tan

SEE ALSO

**********************************************************************/
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include "logging.hpp"

namespace sinks = boost::log::sinks;

struct MessageSink:
  public sinks::basic_formatted_sink_backend<char, sinks::concurrent_feeding> {
  
  void consume(const logging::record_view& rec, const string& str);

  static void Init(Fl_Text_Display* const display,
		   Fl_Text_Buffer* const message);

  static void Write(string& message);

  static Fl_Text_Display* _display;
  static Fl_Text_Buffer* _message;
  static Fl_Text_Display::Style_Table_Entry _stable[];

  static Fl_Text_Buffer* _style;
};

#endif
