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

#ifndef TELESCOPE_HPP
#define TELESCOPE_HPP

/**********************************************************************
NAME

        Telescope - class that allows the user to communicate with the
		    LX200.


SYNOPSIS
	Telescope is the class that allows to user to communicate with
	the LX200.

CONSTRUCTOR

        Telescope(		- constructor
	)			
        
INTERFACE
	Connect()		- connect to the telescope.
				- automatically detects whether an LX200
				- is connected to the derotator.
				- If no LX200 is found, the Telescope goes into debug mode.
				- Returns 0 on success

	Init(			- initialize the telescope instance
				  by reading the LX200 alt-az position
				  and other housekeeping duties.
	)			- returns 0 on success

	GetAltAz(		- get the alt-az position of the
			          telescope
	  time			- at this time
	  alt, az		- the returned alt, az in degrees
	)			- returns 0 on success

AUTHOR                                          

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/

#define CHICAGO_LATITUDE	41.8369 // degrees

using namespace std;

class Telescope
{
public:
  Telescope();
  
  ~Telescope();

public:
  int Connect();
  int Init();
  int GetAltAz(const double time, double* alt,  double* az);
  double GetLatitude() const;
 
private:
  int send(const __FlashStringHelper* cmd) const;
  int receive(int* const data) const;

  int get_altaz(double* alt, double* az) const;
  
  double convert2alt(const int* deg_min_s) const;
  double convert2az(const int* deg_min_s) const;

private:
  double _alt0_rad;
  double _az0_rad;

  double _X0, _Y0, _Z0;

  double _start_time;

private:
  double _latitude_rad;

private:  
  bool _is_debug;
  
};
#endif
