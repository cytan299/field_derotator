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

#ifndef DEROTATORGRAPHICS_HPP
#define DEROTATORGRAPHICS_HPP

#include <config.h>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <FL/Fl_Shared_Image.H>	// fl_register_images()

#include <stdlib.h>

/**********************************************************************
NAME

	DeRotatorGraphics - draws rectangles to represent the position
			    of the actual mechanical derotator


SYNOPSIS
	DeRotatorGraphics draws rectangles to represent the position
	of the actual mechanical derotator, maximum and minimum
	positions etc.


CONSTRUCTOR
   	DeRotatorGraphics(
		)

INTERFACE

	ZAngle(			- set the correction angle of the derotator.
				- this is the zeta in the theory.
		angle		- angle in degrees
	)

	ZAngle()		- returns zeta in degrees

	ZOutlineAngle(		- set the outline of the camera
		angle		- to this angle in degrees w.r.t. the
				  graphics y-axis

	)

	ZOutlineAngle()		- returns the camera outline angle in degrees
				  w.r.t. the graphics y-axis

	ZCameraAngle(		- set the camera
		angle		- to this angle in degrees w.r.t. the graphics y-axis
	)

	ZCameraAngle()		- returns the camera angle in degrees w.r.t. the 
				  graphics y-axis

	SetDeRotatorUI(		- set the pointer to the derotator user interface.
		derotatorUI	
	)			

	SetHallAngle(		- set the angle of the Hall sensor w.r.t. the vertical
		angle		- to this angle in degrees
	)

	GetDisplayCameraAngle()	- get the display angle of the camera seen on the GUI.

	SetDisplayCameraAngle(	- set the display angle of the camera seen on the GUI
		angle		- to this angle in degrees
	)

	GetHallAngle()		- get the Hall sensor w.r.t. the vertical in degrees

	SetHomeAngle(		- set the angle of the Home angle
				  w.r.t. graphics y-axis
		angle		- to this angle in degrees
	)

	GetHomeAngle()		- get the Home angle w.r.t. the
				  graphics y-axis in degrees

	SetMaxCWAngle(		- set the cw angle
				  w.r.t. graphics y-axis
		angle		- to this angle in degrees
	)

	GetMaxCWAngle()		- get the max cw angle w.r.t. the
				  graphics y-axis in degrees

	SetMaxCCWAngle(		- set the ccw angle
				  w.r.t. graphics y-axis
		angle		- to this angle in degrees
	)

	GetMaxCCWAngle()	- get the max ccw angle w.r.t. the
				  graphics y-axis in degrees


	SetTmpHomeAngle(	- set the temporary home angle	
		angle		- to this value
	)

	SetTmpCWAngle(		- set the temporary cw angle	
		angle		- to this value
	)

	SetTmpCCWAngle(		- set the temporary ccw angle	
		angle		- to this value
	)

	SetDrawingMode(		- set the drawing mode to
		drawing_mode	- to this value. This activates which graphics to draw.
	)

	GetDrawingMode()	- returns the current drawing mode.

	EnableLimits()		- enable the CW and CCW limits
	DisableLimits()		- disable the CW and CCW limits
	IsEnableLimits()	- return the state of the limits

MACROS for managing angles

	There can be an inconsistent definition of the angle here in
	DeRotatorGraphics and the actual derotator hardware depending
	on how it is mounted.

	In order to keep everything consistent,
		For the DeRotatorGraphics:
			(a) +angle is always anti-clockwise w.r.t. the
		            user home vector
		        (b) angles are always measured in degrees
			    w.r.t. the user home vector, i.e. 0 degrees
			    is when the camera is at the same angle as
			    the user home vector.
			(c) the home angle is measured w.r.t. the Hall
			    home vector. Again a positive angle means
			    that the user home vector is clockwise
	       		    w.r.t. the Hall home vector.


		For the derotator hardware:
			(a) +angle means that the stepper motor is
			    stepping clockwise.
			(b) However, the gearing and installed
			    position of the derotator means that a
			    +angle may mean that the derotator
			    actually spins anti-clockwise.

	In order to keep everything straight, the conversion between
	the two angular systems is done HERE and not in the hardware
	system. In order to accomplish this as transparently as
	possible, the macros defined below are used.

	If you mount the hardware in a different manner than what I
	do, then the sign can be easily changed in the macros to
	accomomoate this change.

	**** The way that I have mounted the hardware, I have
	     +angle in user interface is anti-clockwise
	     which also spins the derotator in the anti-clockwise
	**** direction.

For the graphics, e.g. camera_outline etc. the angles are measured in
degrees w.r.t. the graphics y-axis. +angle is
anti-clockwise. Therefore, to draw the graphics correctly, the
graphics angle is always given by

	graphics angle = FA2HA(hardware angle) + home_angle

graphics angle is the argument to ZCameraAngle(), ZOutlineAngle() etc.

AUTHOR
	C.Y. Tan

SEE ALSO

**********************************************************************/

// converting steps to angle
#define MECHANICAL_STEPSIZE	0.05970731707 // deg/step

// The way I've mounted the hardware derotator, +angle
// spins the hardware in the anti-clockwise direction.


// Frontend angle to hardware angle
#define FA2HA(x)	x
// Frontend angle to hardware steps
#define FA2HS(x)	x*MECHANICAL_STEPSIZE

// Hardware angle to frontend angle
#define HA2FA(x)	x
// Hardware steps to frontend angle
#define HS2FA(x)	x*MECHANICAL_STEPSIZE
// Hardware angle to frontend steps
#define HA2FS(x)	x/MECHANICAL_STEPSIZE

class DeRotatorUI;

class DeRotatorGraphics : public Fl_Gl_Window {
public:
  typedef enum {CAMERA_OUTLINE_MODE, HOME_MODE, CW_POS_MODE, CCW_POS_MODE} drawingMode;
  
public:
  DeRotatorGraphics(int x,int y,int w,int h,const char *l=0);

  void ZAngle(const double angle);
  double ZAngle() const;

  void ZOutlineAngle(const double angle);
  double ZOutlineAngle() const;

  void ZCameraAngle(const double angle);
  double ZCameraAngle() const;

  void SetDeRotatorUI(DeRotatorUI* const derotatorUI);

public:
  void SetHallAngle(const double angle);
  double GetHallAngle() const;

  void SetDisplayCameraAngle(const double angle);
  double GetDisplayCameraAngle() const;
  
  void SetHomeAngle(const double angle);
  double GetHomeAngle() const;

  void SetMaxCWAngle(const double angle);
  double GetMaxCWAngle() const;

  void SetMaxCCWAngle(const double angle);
  double GetMaxCCWAngle() const;

  void SetTmpHomeAngle(const double angle);
  void SetTmpCWAngle(const double angle);
  void SetTmpCCWAngle(const double angle);  

  void SetDrawingMode(const DeRotatorGraphics::drawingMode drawing_mode);
  DeRotatorGraphics::drawingMode GetDrawingMode() const;

  void EnableLimits();
  void DisableLimits();
  bool IsEnableLimits();

public:
  // these functions must be defined for derived classes of FL_Gl_Window
  void draw();
  int handle(int event);
  
private:
  int load_texture(const char* fname, GLuint& tex_id);
  int gl_init();

private:
  double _z_angle;
  double _z_outline_angle;
  double _z_camera_angle;
  double _size;

  double _mouse_angle0;

private:
  /*
    This is the Hall angle w.r.t. the vertical. Zero angle is defined
    to be the vertical as found by a plumb.

    By default the angle is set to zero,
    unless
	(a) it is already saved in the config file
	(b) the user sets this angle by hand.
  */
  double _z_hall_angle;

  // the user sets the home, cw and ccw angles
  double _z_home_angle;
  double _z_max_cw_angle;
  double _z_max_ccw_angle;

  // the user also sets how the camera and outlines are displayed
  double _z_display_camera_angle;

  // user enables for disables the cw and ccw limits
  bool _is_limits_enabled;

private:
  DeRotatorUI* _derotatorUI;

private:
  const char* _camera_fname;
  GLuint _camera_texture_id;

  const char* _camera_outline_fname;
  GLuint _camera_outline_texture_id;  

  const char* _derotator_fname;
  GLuint _derotator_texture_id;

  const char* _home_fname;
  const char* _home_arrow_fname;
  GLuint _home_texture_id;
  GLuint _home_arrow_texture_id;  

  const char* _cw_fname;
  const char* _cw_arrow_fname;  
  GLuint _cw_texture_id;
  GLuint _cw_arrow_texture_id;    

  const char* _ccw_fname;
  const char* _ccw_arrow_fname;
  GLuint _ccw_texture_id;  
  GLuint _ccw_arrow_texture_id;

private:
  drawingMode _drawing_mode;
  double _z_tmp_home_angle;
  double _z_tmp_cw_angle;
  double _z_tmp_ccw_angle;

};
#endif
