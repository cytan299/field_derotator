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

#ifndef DEROTATORCONFIG_HPP
#define DEROTATORCONFIG_HPP

#include <string>
#include <FL/Fl_Preferences.H>

/**********************************************************************
NAME

	DeRotatorConfig - handles reading and writing the derot
			  configuration.


SYNOPSIS
	DeRotatorConfig takes care of reading and writing of the derot
	configuration.


CONSTRUCTOR
   	DeRotatorConfig(
	  derotatoUI		- pointer to the derotator user interface
		)

INTERFACE

	Load(			- load the configuration 
	  filename		- from the given file name	
	)			- returns 0 on success

	Save(			- save the configuration
	  filename		- to the given filename
	)			- returns 0 on success

	IsGotFilename()		- returns true if a previous Load()/Save()
				  has been called and a filename has been set.
  
AUTHOR
	C.Y. Tan

SEE ALSO

**********************************************************************/
using namespace std;

class DeRotatorUI;

class DeRotatorConfig
{
public:
  DeRotatorConfig(DeRotatorUI* const derotatorUI);
  ~DeRotatorConfig();

public:
  int Load(const char* filename = NULL);
  int Save(const char* filename = NULL);

  bool IsGotFilename() const;

  char* GetUserConfigPath();

private:
  string _filename;
  char _path[FL_PATH_MAX];
  
  DeRotatorUI* _derotatorUI;
  Fl_Preferences* _prefs;
};


#endif
