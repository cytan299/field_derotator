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
/* operating system header files (use <> for make depend) */
#include <Arduino.h>
#include <stdlib.h>
#include <math.h>

/* general system header files (use "" for make depend) */

/* local include files (use "") */
#include "DeRotator.h"

/**********************************************************************
	Defines for the mechanical de-rotator
 **********************************************************************/

#define TIME_STEP_US	100000 // us = 100 ms. this is arbitrary. I am setting 100 ms
			    // to be the smallest time interval that
			    // the loop can be processed. So, if the
			    // next time step is less than this, we
			    // won't be in time to make the next
			    // correction.

#define DEG2RAD	M_PI/180.0
#define RAD2DEG 180.0/M_PI

#define OMEGA	7.2921150e-5 // rotation frequency of the Earth in rad/s. Number is from wikipedia
#define DT_FRACTION	0.25	  // Given the predicted time to get to 1 motor step,
				  // reduce it by this amount to check telescope position

/**********************************************************************
	Hall switch interrupt pin
 **********************************************************************/
#define INTERRUPT_SIGNAL_PIN 18  


/**********************************************************************
NAME
        DeRotator - This is the class calculates the amount of derotation
		    given the initial alt-az position of the star.

SYNOPSIS
	See DeRotator.hpp

                                                
PROTECTED FUNCTIONS

PRIVATE FUNCTIONS

	dzeta_dt(	- calculate the derotator angular velocity in rad/s.
	  latitude_rad	- at this latitude in radians
	  alt, az	- when the telescope is pointed to star at alt,az in degrees
	)		- returns the derotator angular velocity. Value can be NEGATIVE!

	predictor(	- predicts the time needed to achieve one motor step
	  latitude_rad	- at this latitude in radians
	  alt, az	- when the telescope is pointed to star at alt,az in degrees
	  angle_rad	- this is the angle of interest in radians
	)		- returns the time needed to get to the above angle

	step_motor(	- tells the stepper motor to increment by one step.
	  is_clockwise	- in the clockwise direction if true.
			  Otherwise anti-clockwise. Default: true.
	)		  Returns 0 on success.
			  Returns -1 if max_cw and mac_ccw limits are
			  reached.

	hall_interrupt_handler() - handle the interrupt from the Hall magnet	

				  


LOCAL TYPES AND CLASSES

AUTHOR

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/

volatile bool DeRotator::_is_stop_rotating = false;
volatile bool DeRotator::_is_searching_for_hall_home = false;

#define STEPPER_SPEED  100	// 100 steps/second
				// 200 steps required for 1 turn
				// therefore 0.5 Hz

#define MIN_STEPPER_TIME_US  static_cast<unsigned long>(1e6/STEPPER_SPEED) 			

DeRotator::DeRotator(Telescope* const telescope,
		     const double mechanical_stepsize,
		     const bool is_debug)
:
  _stepper(AccelStepper(AccelStepper::DRIVER, 6,7)),
  _telescope(telescope),
  _latitude_rad(0.0), // this will be initialized in Start() by querying Telescope
  _omega(OMEGA),
  _MECHANICAL_STEPSIZE_RAD(mechanical_stepsize*DEG2RAD),
  _is_debug(is_debug)
{
  _time_us = 0;
  _angle_rad = 0;
  _accumulated_angle_rad = 0;

  _stepper.setSpeed(STEPPER_SPEED);  
  _stepper.setMaxSpeed(STEPPER_SPEED);

  LoadLimits();

  // set up the hall switch interrupt to interrupt on falling edge
  pinMode(INTERRUPT_SIGNAL_PIN,INPUT);
  attachInterrupt(5, hall_interrupt_handler, FALLING);  
}


DeRotator::~DeRotator()
{
}


int DeRotator::Start(const double alt, const double az)
{
  // reset the last step time here and in AccelStepper()
  _last_step_time_us = micros();
  _stepper.resetTime();
  _latitude_rad = _telescope->GetLatitude()*DEG2RAD;  
  
  _time_us = _last_step_time_us; 
  _dt_us = predictor(_latitude_rad, alt, az, _MECHANICAL_STEPSIZE_RAD*DT_FRACTION)*1e6;

  _accumulated_angle_rad = 0.0;  
  _angle_rad = 0.0;
  _alt0 = alt;
  _az0 = az;
#ifdef AAAAAA
  Serial.print("last time step us= "); Serial.println(_last_step_time_us, DEC);
  Serial.print("time us = "); Serial.println(_time_us, DEC);
  Serial.print("dt us = "); Serial.println(_dt_us, 16);
#endif
  if(_dt_us > TIME_STEP_US){
    return 0;
  }
  else {
    return -1;
  }
}

int DeRotator::Continue()
{
  int status = 0;
  unsigned long time_us = micros(); // time since micro woke up in us. Note wraps in 1hr15 minutes!
  unsigned long time_ms = millis(); // time since micro woke up in ms. Note wraps in 50 days!
  unsigned long dtime_us = time_us - _time_us; // us

  /*
    check that both dtime and that the stepper will cause a step to
    take place. The reason is that a while() loop check in
    step_motor() screws up the correction timing.
   */
  if((dtime_us >= _dt_us) && ((time_us - _last_step_time_us) >= MIN_STEPPER_TIME_US)){
    // calculate the incremental angular change	
    double dangle_rad = dzeta_dt(_latitude_rad, _alt0, _az0)*dtime_us*1e-6; // rad

    // check whether we need to actually turn the de-rotator motor
    // Note since dzeta_dt can be NEGATIVE, the angle can be negative.
    // So must compare positive numbers

    double angle_rad = _angle_rad + dangle_rad;
    if(fabs(angle_rad) >= _MECHANICAL_STEPSIZE_RAD){
      if(step_motor(angle_rad > 0) == 0){

        // update the angles, and if there's some remainder that we
        // still need to correct, I'll leave in _angle_rad.

	// Depending on whether dangle_rad is positive or negative, I
        // either add or remove MECHANICAL_STEPSIZE_RAD from
        // _angle_rad and _accumulated_angle_rad
	
	if(dangle_rad > 0){
#ifdef AAAAA	  
	  Serial.print("angle_rad (>0) = "); Serial.println(angle_rad, DEC);
	  Serial.print("_angle_rad (>0) = "); Serial.println(_angle_rad, DEC);	  
#endif	  
	  _angle_rad = angle_rad - _MECHANICAL_STEPSIZE_RAD;
	  _accumulated_angle_rad += _MECHANICAL_STEPSIZE_RAD;	  
#ifdef AAAAAAA
	  Serial.print("dtime = "); Serial.println(dtime, DEC);
	  Serial.print("dangle = "); Serial.println(dangle_rad, DEC);
#endif	  
	}
	else {
	  _angle_rad = angle_rad + _MECHANICAL_STEPSIZE_RAD;
	  _accumulated_angle_rad -= _MECHANICAL_STEPSIZE_RAD;
#ifdef AAAAAA	  
	  Serial.print("_angle_rad (<0) = "); Serial.println(_angle_rad, DEC);
	  Serial.print("dtime = "); Serial.println(dtime,DEC);
#endif	  
	}

        // Now remember the current time and calculate the time increment of the next step
	_time_us = time_us;
	_telescope->GetAltAz(time_ms*1e-3, &_alt0, &_az0);
	_dt_us = predictor(_latitude_rad, _alt0, _az0, _MECHANICAL_STEPSIZE_RAD*DT_FRACTION)*1e6;

        // check that the next time step is not smaller than our sampling time
	if(_dt_us > TIME_STEP_US){
	  status = 1; // tell user that we have made stepper motor take one step and next
		      // predicted time step is ok.
#ifdef AAAAAA
	  Serial.print(_time_us, DEC); Serial.print(" "); Serial.println(_accumulated_angle_rad, 12);
#endif	  
        }
	else {
	  status = -1;      // else tell user the predicted time step is too small
	}
      } // step_motor
      else {
	// user limits reached!
	return -2;
      }
    } // angle_rad >= mechanical stepsize
    else { // I am not turning the motor, but I still need to update the _angle_rad
      _angle_rad += dangle_rad;
      // Now remember the current time and calculate the time increment of the next step
      _time_us = time_us; //us
      _telescope->GetAltAz(time_ms*1e-3, &_alt0, &_az0);
      _dt_us = predictor(_latitude_rad, _alt0, _az0, _MECHANICAL_STEPSIZE_RAD*DT_FRACTION)*1e6;
      // check that the next time step is not smaller than our sampling time
      if(_dt_us > TIME_STEP_US){
	status = 0; // tell user not to do anything yet, but everything is still ok
      }
      else {
	status = -1; // tell user that the predicted time step is too small     
      }
    }
  }

  return status;  
}

int DeRotator::Stop()
{
  _is_stop_rotating = true;
  return 0;
}

int DeRotator::IsStop() const
{
  return _is_stop_rotating? 1:0;
}

int DeRotator::Turn(const DIRECTION dir)
{

  /*
    IMPORTANT!!!!! Clockwise and anti-clockwise is defined w.r.t. the
    stepper motor and NOT to the camera rotation
    direction.
    +STEPPER_SPEED is always defined to be clockwise and
    -STEPPER_SPEED is always defined to be anti-clockwise.
   */
  int err = 0;
  long dpos = _stepper.currentPosition() - _home_pos;
  switch(dir){
    case CW:
      if(_is_enable_limits){
	if(dpos >= _max_cw){
	  err = -1;
	}
      }

      _stepper.setSpeed(STEPPER_SPEED);
      break;
      
    case CCW:
      if(_is_enable_limits){
	if(dpos <= _max_ccw){
	  err = -2;
	}
      }

      _stepper.setSpeed(-STEPPER_SPEED);
      break;
  }
  
  _stepper.runSpeed();

  return err;
}


int DeRotator::StartGoingToHallHome()
{
  _is_stop_rotating = false;
  _is_searching_for_hall_home = true;
  
  _stepper.resetTime();
  _stepper.setSpeed(STEPPER_SPEED);

  _start_hall_search_pos = _stepper.currentPosition();

  return 0;
}

int DeRotator::ContinueFindingHallHome()
{
  if(!_is_stop_rotating){
    // safety. Check that we only rotated by +/- 10 deg
    // number of steps = (10 deg)/(0.0597 deg/step) = 167
    long dpos = _stepper.currentPosition() - _start_hall_search_pos;
    if((dpos < -167) || (dpos > 167)){
      return -1;
    }
    _stepper.runSpeed();
  }
  else {
    // Set the current position to zero.
    _stepper.setCurrentPosition(0);
    // there is a bug in setCurrentPosition,
    // I have to set speed to the reverse and forward
    // so that the rotation in the CW direction can work
    // immediately after HALL Home is found
    _stepper.setSpeed(-STEPPER_SPEED);    
    _stepper.setSpeed(STEPPER_SPEED);

    _is_searching_for_hall_home = false;  
  }

  return !_is_stop_rotating? 1:0;
}

void DeRotator::StopFindingHallHome()
{
  _is_stop_rotating = true;
  _is_searching_for_hall_home = false;  
}

int DeRotator::StartGoingToUserHome()
{
  _is_stop_rotating = false;
  _stepper.resetTime();  
  _stepper.moveTo(_home_pos);

  long current_pos = _stepper.currentPosition();
  long dpos = current_pos - _home_pos;

  if(dpos > 0){
    _stepper.setSpeed(-STEPPER_SPEED);
  }
  else {
    _stepper.setSpeed(STEPPER_SPEED); 
  }

  return 0;
}
 
int DeRotator::ContinueFindingUserHome()
{

  if(!_is_stop_rotating){  
    _stepper.runSpeed();

    long current_pos = _stepper.currentPosition();
  
    if(current_pos != _home_pos){
      return 1;
    }
    else {
      return 0;
    }
  }
  else {
    // STOP because user has emergency stopped the rotation
    return 0;
  }
}

void DeRotator::SetUserHome()
{
  _home_pos = _stepper.currentPosition();
}

void DeRotator::SetUserHome(const long home_pos)
{
  _home_pos = home_pos;
}

long DeRotator::GetUserHome() const
{
  return _home_pos;
}

void DeRotator::SetMaxCW()
{
  _max_cw = _stepper.currentPosition() - _home_pos;
}

void DeRotator::SetMaxCW(const long max_cw)
{
  _max_cw = max_cw;
}

long DeRotator::GetMaxCW() const
{
  return _max_cw;
}

void DeRotator::SetMaxCCW()
{
  _max_ccw = _stepper.currentPosition() - _home_pos;
}

void DeRotator::SetMaxCCW(const long max_ccw)
{
  _max_ccw = max_ccw;
}

long DeRotator::GetMaxCCW() const
{
  return _max_ccw;
}

void DeRotator::EnableLimits()
{
  _is_enable_limits = true;
}

void DeRotator::DisableLimits()
{
  _is_enable_limits = false;
}

bool DeRotator::IsEnableLimits() const
{
  return _is_enable_limits;
}

void DeRotator::SetCorrectionDirection(const bool is_clockwise)
{
  _is_clockwise_correction = is_clockwise;
}

bool DeRotator::GetCorrectionDirection() const
{
  return _is_clockwise_correction;
}

double DeRotator::GetAccumulatedAngle() const
{
  return _accumulated_angle_rad*RAD2DEG;
}

void DeRotator::GetAltAz(double* alt, double* az) const
{
  *alt = _alt0;
  *az = _az0;
}

void DeRotator::LoadLimits(const long home_pos,
			   const float max_cw,
			   const float max_ccw,
			   const bool is_enable_limits)
{
  _home_pos = home_pos;

  float max_cw_rad = max_cw*DEG2RAD;
  float max_ccw_rad = max_ccw*DEG2RAD;
  
  _max_cw = static_cast<long>(max_cw_rad/_MECHANICAL_STEPSIZE_RAD);
  _max_ccw = static_cast<long>(max_ccw_rad/_MECHANICAL_STEPSIZE_RAD);

  _is_enable_limits = is_enable_limits;
}

double DeRotator::GetAngle() 
{
  long dpos = _stepper.currentPosition() - _home_pos;

  return dpos*_MECHANICAL_STEPSIZE_RAD*RAD2DEG;
}

int DeRotator::StartGoingToUserAngle(const double angle)
{
  _is_stop_rotating = false;
  _stepper.resetTime();  

  long current_pos = _stepper.currentPosition();
  long dpos0 = current_pos - _home_pos;

  // the user given angle is already w.r.t. User HOME position
  long new_pos = static_cast<long>(angle*DEG2RAD/_MECHANICAL_STEPSIZE_RAD);

  // calculate which way to rotate
  long dpos = new_pos - dpos0;
  if(dpos > 0){
    _stepper.setSpeed(STEPPER_SPEED);
  }
  else {
    _stepper.setSpeed(-STEPPER_SPEED); 
  }

  // and the absolute position is
  _user_abs_angle_pos = new_pos + _home_pos;
  return 0;  
}

int DeRotator::ContinueFindingUserAngle()
{
  if(!_is_stop_rotating){
    _stepper.runSpeed();

    long current_pos = _stepper.currentPosition();

    if(_is_enable_limits){
      long dpos = current_pos - _home_pos;    
      if((dpos >= _max_cw) || (dpos <= _max_ccw)){
        Serial.println("step_motor: max reached");    
        return -2;
      }
    }

    if(current_pos != _user_abs_angle_pos){
      return 1;
    }
    else {
      return 0;
    }
  }
  else {
    // STOP because user has emergency stopped the rotation
    return 0;
  }
}

void DeRotator::SetOmega(const double omega)
{
  _omega = omega;
}

double DeRotator::GetOmega() const
{
  return _omega;
}
   

double DeRotator::dzeta_dt(const double latitude_rad,
			   const double alt,
			   const double az)
{
  double alt_rad = alt*DEG2RAD;
  double az_rad = az*DEG2RAD;

  return _omega*cos(az_rad)*cos(latitude_rad)/cos(alt_rad);
}
  

double DeRotator::predictor(const double latitude_rad,
			    const double alt,
			    const double az,
			    const double angle_rad)
{
  double zeta_dot = dzeta_dt(latitude_rad, alt, az);

  // zeta_dot can be NEGATIVE! But predicted time must be positive
  return fabs(angle_rad/zeta_dot);
}



int DeRotator::step_motor(const bool is_clockwise) 
{
  if(is_clockwise){
    if(_is_clockwise_correction){
      _stepper.move(1);
      // I have to setspeed() again because move() screws up AccelStepper::_stepInterval value!
      _stepper.setSpeed(STEPPER_SPEED);      
    }
    else {
      _stepper.move(-1);
      _stepper.setSpeed(-STEPPER_SPEED);            
    }
  }
  else {
    if(_is_clockwise_correction){
      _stepper.move(-1);
      // I have to setspeed() again because move() screws up AccelStepper::_stepInterval value!
      _stepper.setSpeed(-STEPPER_SPEED);            
    }
    else {
      _stepper.move(1);
      _stepper.setSpeed(STEPPER_SPEED);            
    }
  }
  
  long dpos = _stepper.currentPosition() - _home_pos;
  if(_is_enable_limits){
    if((dpos >= _max_cw) || (dpos <= _max_ccw)){
      Serial.println("step_motor: max reached");    
      return -2;
    }
  }
  
  int i=0;
  while(_stepper.runSpeed() == false){
    /*
	This loop should NEVER be entered!  Empiricially, this would
	screw up the correction timing if stepper does not step when
	required. The Continue() function checks that the stepper is
	ready to step before calling this function.
     */
    _stepper.setSpeed(STEPPER_SPEED);    
    Serial.print("i = ");
    Serial.println(i++, DEC);
  }

  _last_step_time_us = micros(); // us

  return 0;
}
    
void DeRotator::hall_interrupt_handler()
{
  noInterrupts();
  if(_is_searching_for_hall_home)
    _is_stop_rotating = true;
  interrupts();
}
