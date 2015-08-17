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

#ifndef DEROTATOR_HPP
#define DEROTATOR_HPP

#include "AccelStepper.h"
#include "Telescope.h"

/**********************************************************************
NAME

        DeRotator - This is the class calculates the amount of derotation
		    given the initial alt-az position of the star.

SYNOPSIS
	DeRotator calculates the amount of derotation given the
	initial alt-az position of the star.

	DeRotator does the following:
		(a) accumulates phase from start to finish.
		(b) dynamically corrects the field de-rotation by
		querying the alt-az position of the telescope.
		(c) dynamically calculates the query/update rate from
		the mechanical step size of the correction.

CONSTRUCTOR

        DeRotator(		- constructor
	  telescope		- pointer to the user constructed Telescope object
	  mechanical_stepsize	- the mechanical step size of the
				  system in degrees/step
	  is_debug		- enable debugging if true. Default: true
	)	

        
INTERFACE
	Start(			- start de-rotation using these initial
	  alt			- alt-az coordinates in degrees
	  az
	)			- returns 0 on success.
				  returns -1 if the (alt,az)
			     	  coordinates show that the de-rotator
				  will not have time to turn.

	Continue()		- continue with the de-rotation.

				  returns +1 if one stepper motor step
				  has been applied. Everything is ok.

				  returns 0 if there is nothing for
				  the caller to do, i.e. continue to
				  wait to accumulate enough phase to
				  turn the motor. This is an ok state.

				  returns -1 if the next *time* step is
				  so small that we will not be able to
				  increment the motor in time.

				  returns -2 if the user set max ccw
				  or cw have been reached

	Stop()			- stop de-rotation. Also resets the
				  accumulated angle. Returns 0 on
				  success.

	IsStop()		- returns whether the derotator
				  has stopped rotating

	Turn(			- turn the stepper motor
	  dir			- in the given direction: CW or CCW.
	)			- returns

	StartGoingToHallHome()	- start going to the home position defined
				  by the Hall switch

	ContinueFindingHallHome() - continue looking for the Hall home
				    position, i.e. Hall interrupt is asserted.
				    Rotation is confined to +/- 10 deg
				    so that the camera does not hit an
				    obstacle.
				    Returns 0 if rotation is stopped
					    1 if rotation is continuing

	StopFindingHallHome()	- found the Hall home position and stop the
				  rotation.

	StartGoingToUserHome()	- start rotating towards the user
				  defined HOME position.

	ContinueFindingUserHome() - continue rotating until the user
				   home is found. Returns 0 if found.
				   Returns 1 if rotation has to be
				   continued.

	SetUserHome()		- set the current stepper position as
				  home.

	SetUserHome(		- set the stepper home position to
	  home_pos		- this home position	
	)

	GetUserHome()		- return the user home position

	SetMaxCW()		- set the current stepper position as
				  the maximum CW position w.r.t. home pos

	SetMaxCW(		- set the max cw position to
	  max_cw		- this user given position	
	)			- this position *must* be w.r.t. home pos

	GetMaxCW()		- return the max cw position w.r.t. home pos

	SetMaxCCW()		- set the current stepper position as
				  the maximum CCW position w.r.t. home pos

	SetMaxCCW(		- set the max ccw position to
	  max_ccw		- this user given position.	
	)			- this position *must* be w.r.t. home pos	

	GetMaxCCW()		- return the max ccw position w.r.t. home pos

	EnableLimits()		- enable checking of the derotator
				  cw and ccw limits

	DisableLimits()		- disable checking of the derotator
				  cw and ccw limits

	IsEnableLimits()	- return whether the limits are enabled
				  or disabled.

	SetCorrectionDirection(	- tell the derotator which way to
				  turn to apply the de-rotation
		is_clockwise	- true if clockwise is required.
	)

	GetCorrectionDirection()- get the derotator correction
				  direction. Returns true if clockwise.
	
	GetAccumulatedAngle()	- returns the accumulated angle in
				  degrees since the last Reset()

	GetAltAz(		- returns the alt, az coordinates
		alt, az		- that the derotator thinks the telescope is pointing at
	)			- returns 0 on success

	GetAngle()		- return the current angle in degrees of the derotator
				  w.r.t. USER Home position

	StartGoingToUserAngle(	- start moving towards the user given angle
		angle		- angle in degrees w.r.t. the USER Home position
	)			- returns 0 on success

	ContinueFindingUserAngle() - continue rotating until the user
				   angle is found. Returns 0 if found.
				   Returns 1 if rotation has to be
				   continued.

	LoadLimits(		- load the limits of
	  home_pos			- home. Default = 0 
	  max_cw		- max_cw. Default = +90 deg
	  max_ccw		- max_ccw. Default = -90 deg
	  is_enable_limits	- should max_cw and max_ccw be enabled? Default: false
	)

AUTHOR                                          

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/

using namespace std;

class DeRotator
{
public:
  DeRotator(Telescope* const telescope,
	    const double mechanical_stepsize,
	    const bool is_debug = true);
  
  ~DeRotator();

public:
  /*
    The clockwise and anti-clockwise directions are defined w.r.t. the
    stepper motor direction and not the direction the camera rotates.
    +stepper motor speed is clockwise.
    -stepper motor speed is anti-clockwise.
   */
  enum DIRECTION {CW, CCW};
  
public:
  int Start(const double alt, const double az);
  int Continue();
  int Stop();
  int IsStop() const;

  int Turn(const DIRECTION dir);

  int StartGoingToHallHome();
  int ContinueFindingHallHome();
  void StopFindingHallHome();
  
  int StartGoingToUserHome();  
  int ContinueFindingUserHome();
  
  void SetUserHome();
  void SetUserHome(const long home_pos);
  long GetUserHome() const;
  
  void SetMaxCW();
  void SetMaxCW(const long max_cw);
  long GetMaxCW() const;
  
  void SetMaxCCW();
  void SetMaxCCW(const long max_ccw);
  long GetMaxCCW() const;

  void EnableLimits();
  void DisableLimits();
  bool IsEnableLimits() const;

  void SetCorrectionDirection(const bool is_clockwise);
  bool GetCorrectionDirection() const;
  
  double GetAccumulatedAngle() const;
  void GetAltAz(double* alt, double* az) const;

  double GetAngle();
  int StartGoingToUserAngle(const double angle);
  int ContinueFindingUserAngle();
  long _user_abs_angle_pos;
  
public:
  void LoadLimits(const long home_pos = 0,
		  const float max_cw = 90,
		  const float max_ccw = -90,
		  const bool is_enable_limits = false);

private:
  double dzeta_dt(const double latitude,
		  const double alt,
		  const double az);

  double predictor(const double latitude,
		   const double alt,
		   const double az,
		   const double angle_rad);

  int step_motor(const bool is_clockwise = true);

private:
  AccelStepper _stepper;  
  Telescope* _telescope;

private:
  double _alt0, _az0;  
  double _angle_rad;
  double _accumulated_angle_rad;
  double _time;
  double _dt;

  double _last_step_time_us;

  bool _is_clockwise_correction;

private:
  double _latitude_rad;
  const double _MECHANICAL_STEPSIZE_RAD; // in rad/step
  const bool _is_debug;

private:
  long _home_pos;
  long _max_cw, _max_ccw;
  bool _is_enable_limits;

public:
  static void hall_interrupt_handler();
  static volatile bool _is_stop_rotating;
  static volatile bool _is_searching_for_hall_home;  
  long _start_hall_search_pos;

  
};
#endif
