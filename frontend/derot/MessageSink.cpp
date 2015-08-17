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
/* operating system header files (use <> for make depend) */
#include <iostream>

using namespace std;

/* general system header files (use "" for make depend) */

/* local include files (use "") */
#include "MessageSink.hpp"

/* file global variables */

/**********************************************************************
NAME
        MessageSink - struct to allow the Message window of the
		      DeRotator frontend to show the boost logger
		      messages. 

SYNOPSIS
	See MessageSink.hpp

                                                
PROTECTED FUNCTIONS

PRIVATE FUNCTIONS


LOCAL TYPES AND CLASSES

AUTHOR

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/

Fl_Text_Display* MessageSink::_display= NULL;
Fl_Text_Buffer* MessageSink::_message = NULL;
Fl_Text_Buffer* MessageSink::_style = NULL;

// Style table
Fl_Text_Display::Style_Table_Entry MessageSink::_stable[] = {
  // FONT COLOR      FONT FACE     FONT SIZE
  // --------------- -----------   --------------
  {FL_RED,         FL_HELVETICA,   10 }, // A - Red
  {FL_DARK_RED,    FL_HELVETICA,   10 }, // B - Dark Red
  {FL_DARK_GREEN,  FL_HELVETICA,   10 }, // C - Green
  {FL_BLUE,        FL_HELVETICA,   10 }, // D - Blue
  {FL_MAGENTA,     FL_HELVETICA,   10 }, // E - Magenta
  {FL_CYAN,        FL_HELVETICA,   10 }, // F - CYAN
  {FL_BLACK,       FL_HELVETICA,   10 }, // G - Black
};

void MessageSink::consume(const logging::record_view& rec, const string& str) {
  using logging::trivial::severity_level;      
  auto severity = rec.attribute_values()[logging::aux::default_attribute_names::severity()].extract<severity_level>();

  string s;
  char color;
  switch(severity.get()){
    case logging::trivial::trace:
        color = 'D'; // blue
        s = "trace";
    break;
    case logging::trivial::debug:
        color = 'C'; // green
        s = "debug";
    break;
    case logging::trivial::info:
        color = 'G'; // black
        s = "info";
    break;
    case logging::trivial::warning:
        color = 'E'; // magenta
        s = "warning";
    break;
    case logging::trivial::error:
      color = 'A'; // red
      s = "error";
    break;
    case logging::trivial::fatal:
      color = 'B'; // dark red
      s = "fatal";
    break;
  }

  using namespace boost;
  posix_time::ptime t = posix_time::second_clock::local_time();
  string out = to_simple_string(t.date()) + " "
    + to_simple_string(t.time_of_day())
    + ": <" 
    + s
    + "> "
    + str
    + "\n";

  string out_style;
  for(int i=0; i< out.size(); i++){
    out_style += color;
  }
      
  if(_message && _style){
    _message->append(out.c_str());
    _style->append(out_style.c_str());

    //always scroll so that there is one blank line in the Message window
    // if the number of lines in message > number of rows in Message window

    /*
      Unfortunately there is no smart way of determining the number of
      display rows and so I have to hardcode the number here.
    */	    
    const int num_display_rows = 4; 
    const int num_message_rows = _message->count_lines(0, _message->length());

    if(num_message_rows >= num_display_rows){
      _display->scroll(num_message_rows - 2,0);
    }
  }
  else {
    cerr << out;
  }
}

void MessageSink::Init(Fl_Text_Display* const display,
		       Fl_Text_Buffer* const message)
{
  _display = display;
  _message = message;

  _style = new Fl_Text_Buffer();
  _display->highlight_data(_style, _stable, sizeof(_stable)/sizeof(_stable[0]), 'A', 0, 0);

}

void MessageSink::Write(string& message)
{
  string style;
  for(int i=0; i< message.size(); i++){
    style += 'G'; // black
  }
  
  _message->text(message.c_str());
  _style->text(style.c_str());
  
}
