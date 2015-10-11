/* $Id$*/
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
#include <math.h>

#ifdef __APPLE__
#include <CoreFoundation/CFURL.h>
#include <CoreFoundation/CFBUNDLE.h>
#endif

/* general system header files (use "" for make depend) */
#include "boost/filesystem.hpp"

/* local include files (use "") */

#include "DeRotatorUI.h"
#include "DeRotatorGraphics.hpp"
#include "logging.hpp"


/**********************************************************************
NAME
        DeRotatorGraphics - wrapper class for socket calls to a tcp server.

SYNOPSIS
	See DeRotatorGraphics.hpp

                                                
PROTECTED FUNCTIONS

PRIVATE FUNCTIONS


LOCAL TYPES AND CLASSES

AUTHOR

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/

using namespace std;

double atan2a(const double y, const double x)
{
  /*
	atan2() quadrants:
		1st quadrant: 0 to 90 deg
		2nd quadrant: 90 to 180
		3rd quadrant:-90 to -180
		4th quadrant: 0 to -90

In this case, I am shifting the cut from 180 deg to -90 deg, i.e. by
rotating the quadrants by 90 deg

	atan2a() quadrants:
		1st quadrant: 0 to -90 deg
		2nd quadrant: 0 to 90 deg
		3rd quadrant: 90 to 180	deg
		4th quadrant:-90 to -180 deg
		
  */
  double angle = atan2(y,x);

  if((angle >= -M_PI/2) && (angle <= M_PI)){
    angle -= M_PI/2;
  }
  else {
    angle += 3*M_PI/2;	
  }

  return angle;  
}

DeRotatorGraphics::DeRotatorGraphics(int x,int y,int w,int h,const char *l)
            : Fl_Gl_Window(x,y,w,h,l)
{
  /*
	All the png files have been compressed with
		convert -depth 8 a.png a3.png
   */
  _camera_fname = "camera_back3.png";
  _camera_outline_fname = "camera_outline3.png";
  _derotator_fname = "derotator_back3.png";
  
  _home_arrow_fname = "home_arrow3.png";
  _home_fname = "home_pos3.png";

  _cw_arrow_fname = "cw_arrow3.png";
  _cw_fname = "cw_pos3.png";

  _ccw_arrow_fname = "ccw_arrow3.png";
  _ccw_fname = "ccw_pos3.png";  

  _z_angle = 0.0;
  _z_outline_angle = 0.0;
  _z_camera_angle = 0.0;

  _is_limits_enabled = false;
  
  _size = 10.0;
  _mouse_angle0 = 0.0;

  _z_hall_angle = 0.0;
  _z_home_angle = 0.0;
  _z_max_cw_angle = -90;
  _z_max_ccw_angle = 90;
  _z_display_camera_angle = 90;

  _drawing_mode = CAMERA_OUTLINE_MODE;
  _z_tmp_home_angle = 0;
  _z_tmp_cw_angle = -90;
  _z_tmp_ccw_angle = 90;
}


void DeRotatorGraphics::ZAngle(const double angle)
{
  _z_angle = angle;
}

double DeRotatorGraphics::ZAngle() const
{
  return _z_angle;
}


void DeRotatorGraphics::ZOutlineAngle(double angle)
{
  _z_outline_angle = angle;
}

double DeRotatorGraphics::ZOutlineAngle() const
{
  return _z_outline_angle;
}

void DeRotatorGraphics::ZCameraAngle(double angle)
{
  _z_camera_angle = angle;
}

double DeRotatorGraphics::ZCameraAngle() const
{
  return _z_camera_angle;
}

void DeRotatorGraphics::SetDeRotatorUI(DeRotatorUI* const derotatorUI)
{
  _derotatorUI = derotatorUI;
}

void DeRotatorGraphics::SetHallAngle(const double angle)
{
  _z_hall_angle = angle;
}

double DeRotatorGraphics::GetHallAngle() const
{
  return _z_hall_angle;
}

void DeRotatorGraphics::SetDisplayCameraAngle(const double angle)
{
  _z_display_camera_angle = angle;
}

double DeRotatorGraphics::GetDisplayCameraAngle() const
{
  return _z_display_camera_angle;
}


void DeRotatorGraphics::SetHomeAngle(const double angle)
{
  _z_home_angle = angle;
}

double DeRotatorGraphics::GetHomeAngle() const
{
  return _z_home_angle;
}

void DeRotatorGraphics::SetMaxCWAngle(const double angle)
{
  _z_max_cw_angle = angle;
}

double DeRotatorGraphics::GetMaxCWAngle() const
{
  return _z_max_cw_angle;
}

void DeRotatorGraphics::SetMaxCCWAngle(const double angle)
{
  _z_max_ccw_angle = angle;
}

double DeRotatorGraphics::GetMaxCCWAngle() const
{
  return _z_max_ccw_angle;
}

void DeRotatorGraphics::SetTmpHomeAngle(const double angle)
{
  _z_tmp_home_angle = angle;
}

void DeRotatorGraphics::SetTmpCWAngle(const double angle)
{
  _z_tmp_cw_angle = angle;
}

void DeRotatorGraphics::SetTmpCCWAngle(const double angle)
{
  _z_tmp_ccw_angle = angle;
}

void DeRotatorGraphics::SetDrawingMode(const drawingMode drawing_mode)
{
  _z_tmp_home_angle = _z_home_angle;
  _z_tmp_cw_angle = _z_max_cw_angle;
  _z_tmp_ccw_angle = _z_max_ccw_angle;
  
  _drawing_mode = drawing_mode;
}

DeRotatorGraphics::drawingMode DeRotatorGraphics::GetDrawingMode() const
{
  return _drawing_mode;
}

void DeRotatorGraphics::EnableLimits()
{
  _is_limits_enabled = true;
}

void DeRotatorGraphics::DisableLimits()
{
  _is_limits_enabled = false;
}

bool DeRotatorGraphics::IsEnableLimits()
{
  return _is_limits_enabled;
}



void DeRotatorGraphics::draw()
{
  using namespace logging::trivial;
  src::severity_logger< severity_level > lg;
  
  static bool first_time = true;
  
  if (!valid()) {
    valid(1);
    if(first_time){
      first_time = false;
      if(gl_init() != 0){
	LOG_ERROR << "DeRotatorGraphics::draw(): gl_init() error\n";
      }

      // set up the deg and steps boxes in the interface
      _derotatorUI->steps->value("  0");        
      _derotatorUI->deg->value("0.00");  
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0,0,w(),h());
    float aspect = (float)w() / (float)h()*_size;
    glOrtho(-aspect, aspect, -_size, _size, 0, 1);
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

  glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, _derotator_texture_id);  
    glBegin(GL_QUADS);
    {
      // length =4.75, breadth=3.25      
      double dx = -4.875;
      double dy = -7.125 - 2.1;
      glTexCoord2f(0.0, 0.0);  glVertex3f (0.0 + dx, 14.25 + dy, 0.0);
      glTexCoord2f (1.0, 0.0); glVertex3f (9.75 + dx, 14.25 + dy, 0.0);
      glTexCoord2f (1.0, 1.0); glVertex3f (9.75 + dx, 0.0 + dy, 0.0);
      glTexCoord2f (0.0, 1.0); glVertex3f(0.0 + dx, 0.0 + dy, 0.0);
    }
    glEnd();
  glPopMatrix();

  glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, _camera_texture_id);
    glRotatef((GLfloat)(_z_camera_angle + _z_hall_angle + _z_display_camera_angle) ,0,0,1.0);    
    glBegin(GL_QUADS);
    {
      // length = 4.96, breadth =3.95	
      double dx = -5.925;
      double dy = -7.44 + 0.025;
      glTexCoord2f(0.0, 0.0);  glVertex3f (0.0 + dx, 14.88 + dy, 0.0);
      glTexCoord2f (1.0, 0.0); glVertex3f (11.85 + dx, 14.88 + dy, 0.0);
      glTexCoord2f (1.0, 1.0); glVertex3f (11.85 + dx, 0.0 + dy, 0.0);
      glTexCoord2f (0.0, 1.0); glVertex3f(0.0 + dx, 0.0 + dy, 0.0);
    }
    glEnd();
  glPopMatrix();

  glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, _camera_outline_texture_id);
    glRotatef((GLfloat)(_z_outline_angle + _z_hall_angle + _z_display_camera_angle),0,0,1.0);    
    glBegin(GL_QUADS);
    {
      // length = 4.96, breadth =3.95	
      double dx = -5.925;
      double dy = -7.44 + 0.025;
      glTexCoord2f(0.0, 0.0);  glVertex3f (0.0 + dx, 14.88 + dy, 0.0);
      glTexCoord2f (1.0, 0.0); glVertex3f (11.85 + dx, 14.88 + dy, 0.0);
      glTexCoord2f (1.0, 1.0); glVertex3f (11.85 + dx, 0.0 + dy, 0.0);
      glTexCoord2f (0.0, 1.0); glVertex3f(0.0 + dx, 0.0 + dy, 0.0);
    }
    glEnd();
  glPopMatrix();    
  

  glPushMatrix();
    switch(_drawing_mode){
      case CAMERA_OUTLINE_MODE:
	glBindTexture(GL_TEXTURE_2D, _home_arrow_texture_id);
        glRotatef((GLfloat)(_z_home_angle + _z_hall_angle + _z_display_camera_angle),0,0,1.0);

      break;

      case HOME_MODE:
	glBindTexture(GL_TEXTURE_2D, _home_texture_id);
        glRotatef((GLfloat)(_z_tmp_home_angle + _z_hall_angle + _z_display_camera_angle),0,0,1.0);   	
      break;

      case CW_POS_MODE:
	glBindTexture(GL_TEXTURE_2D, _cw_texture_id);
        glRotatef((GLfloat)(_z_tmp_cw_angle),0,0,1.0);   	
      break;

      case CCW_POS_MODE:
	glBindTexture(GL_TEXTURE_2D, _ccw_texture_id);
        glRotatef((GLfloat)(_z_tmp_ccw_angle),0,0,1.0);   	
      break;      
    }
    

    glBegin(GL_QUADS);
    {
      double dx = -7.45;
      double dy = -7.45;
      glTexCoord2f(0.0, 0.0);  glVertex3f (0.0 + dx, 14.90 + dy, 0.0);
      glTexCoord2f (1.0, 0.0); glVertex3f (14.90 + dx, 14.90 + dy, 0.0);
      glTexCoord2f (1.0, 1.0); glVertex3f (14.90 + dx, 0.0 + dy, 0.0);
      glTexCoord2f (0.0, 1.0); glVertex3f(0.0 + dx, 0.0 + dy, 0.0);
    }
    glEnd();
  glPopMatrix();

  if(_is_limits_enabled){
    if(_drawing_mode == CAMERA_OUTLINE_MODE){
      glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, _cw_arrow_texture_id);
        glRotatef((GLfloat)(_z_max_cw_angle),0,0,1.0);
        glBegin(GL_QUADS);
        {
	  double dx = -7.45;
	  double dy = -7.45;
	  glTexCoord2f(0.0, 0.0);  glVertex3f (0.0 + dx, 14.90 + dy, 0.0);
	  glTexCoord2f (1.0, 0.0); glVertex3f (14.90 + dx, 14.90 + dy, 0.0);
	  glTexCoord2f (1.0, 1.0); glVertex3f (14.90 + dx, 0.0 + dy, 0.0);
	  glTexCoord2f (0.0, 1.0); glVertex3f(0.0 + dx, 0.0 + dy, 0.0);
        }
        glEnd();
      glPopMatrix();

      glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, _ccw_arrow_texture_id);
        glRotatef((GLfloat)(_z_max_ccw_angle),0,0,1.0);
        glBegin(GL_QUADS);
        {
          double dx = -7.45;
          double dy = -7.45;
          glTexCoord2f(0.0, 0.0);  glVertex3f (0.0 + dx, 14.90 + dy, 0.0);
          glTexCoord2f (1.0, 0.0); glVertex3f (14.90 + dx, 14.90 + dy, 0.0);
          glTexCoord2f (1.0, 1.0); glVertex3f (14.90 + dx, 0.0 + dy, 0.0);
          glTexCoord2f (0.0, 1.0); glVertex3f(0.0 + dx, 0.0 + dy, 0.0);
        }
        glEnd();
      glPopMatrix();
    }  
  }
  
  // create the label that tells us the angle of the derotator
  glDisable(GL_DEPTH_TEST);
    gl_color(FL_GRAY);    
    char buf[32];
    sprintf(buf, "\u03B6 = % 7.2f deg    ", _z_angle);
    gl_draw(buf, -14.0f, -9.5f );
  glEnable(GL_DEPTH_TEST);    
}

int DeRotatorGraphics::handle(int event) 
{
  float x; float y;
  double angle;

  switch(event) {
    case FL_PUSH: //... mouse down event ...

      x = Fl::event_x() - w()/2.0;
      // the difference is this way because y pixels INCREASES going down      
      y = h()/2.0 - Fl::event_y();

      switch(_drawing_mode){
        case CAMERA_OUTLINE_MODE:
	  // the mouse angle is calculated w.r.t. what is in _z_outline_angle
	  _mouse_angle0 = atan2a(y,x)*180.0/M_PI - _z_outline_angle;
	break;

        case HOME_MODE:
          _mouse_angle0 = atan2a(y,x)*180.0/M_PI - _z_tmp_home_angle;
	break;

        case CW_POS_MODE:
          _mouse_angle0 = atan2a(y,x)*180.0/M_PI - _z_tmp_cw_angle;
	break;

        case CCW_POS_MODE:
          _mouse_angle0 = atan2a(y,x)*180.0/M_PI - _z_tmp_ccw_angle;
	break;  	
      }
      return 1;
      
    case FL_DRAG: // ... mouse moved while down event ...
      x = Fl::event_x() - w()/2.0;
      y = h()/2.0 - Fl::event_y(); 
      angle = atan2a(y,x)*180.0/M_PI;

      double new_angle;
      char buf[32];
      switch(_drawing_mode){
        case CAMERA_OUTLINE_MODE:
	  new_angle = angle - _mouse_angle0;	  
	  // check whether z_outline_angle is within limits 
	  if(_is_limits_enabled){
	    if((((new_angle + _z_hall_angle) <= _z_max_ccw_angle)) &&
	       ((new_angle + _z_hall_angle) >= _z_max_cw_angle)){
	      _z_outline_angle = new_angle;
	    }
	  }
	  else {
	    // limits disabled, so do no checks
	    _z_outline_angle = new_angle;
	  }
	  
          redraw();
	  
          sprintf(buf, "%4.2f", _z_outline_angle - _z_home_angle);
          _derotatorUI->deg->value(buf);

          sprintf(buf, "%4d", static_cast<int>((_z_outline_angle- _z_home_angle)/MECHANICAL_STEPSIZE));
          _derotatorUI->steps->value(buf);
	break;
	
        case HOME_MODE:
	  _z_tmp_home_angle = angle - _mouse_angle0;
	  redraw();

	  sprintf(buf, "%4.2f", _z_tmp_home_angle);
          _derotatorUI->deg->value(buf);

          sprintf(buf, "%4d", static_cast<int>(_z_tmp_home_angle/MECHANICAL_STEPSIZE));
          _derotatorUI->steps->value(buf);
	break;


        case CW_POS_MODE:

	  // make sure that CW is always between -180 and 0 degrees
	  new_angle = angle - _mouse_angle0;
	  if((new_angle >= -180) && (new_angle <= 0)){
	    _z_tmp_cw_angle = new_angle;
	  } // else do nothing
	  
	  redraw();

	  sprintf(buf, "%4.2f", _z_tmp_cw_angle);
          _derotatorUI->deg->value(buf);

          sprintf(buf, "%4d", static_cast<int>(_z_tmp_cw_angle/MECHANICAL_STEPSIZE));
          _derotatorUI->steps->value(buf);
	break;


        case CCW_POS_MODE:
	  new_angle = angle - _mouse_angle0;

	  if((new_angle >= 0) && (new_angle <= 180)){
	    _z_tmp_ccw_angle = new_angle;
	  } // else do nothing
	  
	  redraw();

	  sprintf(buf, "%4.2f", _z_tmp_ccw_angle);
          _derotatorUI->deg->value(buf);

          sprintf(buf, "%4d", static_cast<int>(_z_tmp_ccw_angle/MECHANICAL_STEPSIZE));
          _derotatorUI->steps->value(buf);	  
	break;	
      }
      return 1;
      
    case FL_RELEASE: //... mouse up event ...
      return 1;
      
    case FL_FOCUS :
    case FL_UNFOCUS : //... Return 1 if you want keyboard events, 0 otherwise
    
      return 0;
    
    case FL_KEYBOARD:
    /*
    ... keypress, key is in Fl::event_key(), ascii in Fl::event_text()
    ... Return 1 if you understand/use the keyboard event, 0 otherwise...
    */
      return 0;
      
    case FL_SHORTCUT:
      /*... shortcut, key is in Fl::event_key(), ascii in Fl::event_text()
        ... Return 1 if you understand/use the shortcut event, 0 otherwise...
      */
      return 0;
    
    default:
    // pass other events to the base class...
      return Fl_Gl_Window::handle(event);
  }  
}

int DeRotatorGraphics::load_texture(const char* fname, GLuint& tex_id)
{
  using namespace logging::trivial;
  src::severity_logger< severity_level > lg;
  
 // initialize the texture map
  glGenTextures(1, &tex_id);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  Fl_Shared_Image *image;  
  // check that the fname exists
  using namespace boost::filesystem;
  path p(fname);
  if(exists(p)){
    image = Fl_Shared_Image::get(fname);
  }
  else {
    // fname does not exist in the current directory.
    // Check whether this file exists in the Resources directory of
    // a Mac bundle
    
    #ifdef __APPLE__
      unsigned short ufname[64];
      for(int i=0; i<strlen(fname); i++){
	ufname[i] = fname[i];
      }
      CFStringRef s = CFStringCreateWithCharacters(NULL, ufname, strlen(fname));
      CFURLRef appUrlRef;
      appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), s, NULL, NULL);

      CFStringRef filePathRef = CFURLCopyPath(appUrlRef);
      const char* filePath = CFStringGetCStringPtr(filePathRef, kCFStringEncodingUTF8);

      path p1(filePath);
      if(exists(p1)){
	image = Fl_Shared_Image::get(filePath);
	CFRelease(filePathRef);
	CFRelease(appUrlRef);
      }
      else {
	LOG_ERROR << "DeRotator::load_texture(): " << filePath
		  << " does not exist\n";
	CFRelease(filePathRef);
	CFRelease(appUrlRef);	
	return -1;	
      }
      
    #endif
  }

  if(image == NULL){
    LOG_ERROR << "DeRotator::load_texture(): " << fname
	      << " has an unknown format\n";
    return -1;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->w(), image->h(), 0,
	       GL_RGBA, GL_UNSIGNED_BYTE, image->data()[0]);
  image->release();

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glEnable(GL_TEXTURE_2D);

  return 0;
}

int DeRotatorGraphics::gl_init()
{
  using namespace logging::trivial;
  src::severity_logger< severity_level > lg;

  fl_register_images();

  // load in the camera texture
  if(load_texture(_camera_fname, _camera_texture_id) != 0){
    LOG_ERROR << "DeRotatorGraphics::gl_init(): problem with loading_texture() "
	      << _camera_fname << "\n";      
    return -1;
  }


  // load in the camera shell texture
  if(load_texture(_camera_outline_fname, _camera_outline_texture_id) != 0){
    LOG_ERROR << "DeRotatorGraphics::gl_init(): problem with loading_texture() "
	      << _camera_outline_fname << "\n";      
    return -1;
  }  

  // load in the derotator texture
  if(load_texture(_derotator_fname, _derotator_texture_id) != 0){
    LOG_ERROR << "DeRotatorGraphics::gl_init(): problem with loading_texture() "
	      << _derotator_fname << "\n";
    return -1;
  }

  // load in the home pointer textures

  if(load_texture(_home_arrow_fname, _home_arrow_texture_id) != 0){
    LOG_ERROR << "DeRotatorGraphics::gl_init(): problem with loading_texture() "
	      << _home_arrow_fname << "\n";
    return -1;
  }
  
  if(load_texture(_home_fname, _home_texture_id) != 0){
    LOG_ERROR << "DeRotatorGraphics::gl_init(): problem with loading_texture() "
	      << _home_fname << "\n";
    return -1;
  }

  // load in the cw pointer texture
  if(load_texture(_cw_arrow_fname, _cw_arrow_texture_id) != 0){
    LOG_ERROR << "DeRotatorGraphics::gl_init(): problem with loading_texture() "
	      << _cw_arrow_fname << "\n";
    return -1;
  }
  
  if(load_texture(_cw_fname, _cw_texture_id) != 0){
    LOG_ERROR << "DeRotatorGraphics::gl_init(): problem with loading_texture() "
	      << _cw_fname << "\n";
    return -1;
  }
  
  
  // load in the ccw pointer texture
  if(load_texture(_ccw_arrow_fname, _ccw_arrow_texture_id) != 0){
    LOG_ERROR << "DeRotatorGraphics::gl_init(): problem with loading_texture() "
	      << _ccw_arrow_fname << "\n";
    return -1;
  }
  
  if(load_texture(_ccw_fname, _ccw_texture_id) != 0){
    LOG_ERROR << "DeRotatorGraphics::gl_init(): problem with loading_texture() "
	      << _ccw_fname << "\n";
    return -1;
  }

#ifdef AAAAAA
  // enable lighting
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glShadeModel(GL_SMOOTH);
#endif  
  // hidden surface
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  return 0;
}
