/* 
 * dingo-videoarea.cc
 *
 *  Copyright (C) 2011-2013 Phong V. Cao <phongvcao@phongvcao.com>
 * 
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 3 of 
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA."
 *
 */

#include "dingo-videoarea.h"
#include "dingo-statusbar.h"
#include "dingo-dbmanager.h"
#include "dingo-playerengine.h"
#include "dingo-mousemotionobserver.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

//CONSTRUCTORS - DESTRUCTOR
Dingo::VideoArea::VideoArea(Dingo::DBManager* dbmanager, Dingo::PlayerEngine* playerengine, Dingo::StatusBar* statusbar) :
  temp_rgba("000000")
{
  d_dbmanager = dbmanager;
  d_playerengine = playerengine;
  d_statusbar = statusbar;
  
  d_cursor_movement_count = 0;
  
  createVideoArea();
}

Dingo::VideoArea::~VideoArea() {

}
      
//OBSERVER'S METHODS      
void Dingo::VideoArea::registerMouseMotionObserver(Dingo::MouseMotionObserver* observer) {
  mousemotion_obs_vector.push_back(observer);
}

void Dingo::VideoArea::removeMouseMotionObserver(Dingo::MouseMotionObserver* observer) {
  std::vector<Dingo::MouseMotionObserver*>::iterator iter = std::find(mousemotion_obs_vector.begin(), mousemotion_obs_vector.end(), observer);
  
  if (iter != mousemotion_obs_vector.end()) {
    mousemotion_obs_vector.erase(iter);
  }
}

void Dingo::VideoArea::notifyMouseMotionObserver(Dingo::VideoAreaSignals signal_type) {
  switch (signal_type) {
    case Dingo::MOUSE_HOVER_ON_FULLSCREEN: {
      for (std::vector<Dingo::MouseMotionObserver*>::iterator iter = mousemotion_obs_vector.begin(); iter != mousemotion_obs_vector.end(); ++iter) {
        (*iter)->updateMouseHoverOnFullscreen();
      }
      //std::cout << "Video Area: Mouse Motion Hovered!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Mouse Motion: Unknown Dingo::VideoAreaSignals!" << std::endl;
      break;
    }
  }
}

//INHERITED OBSERVER'S METHODS
void Dingo::VideoArea::updateFullscreenActivated() {
  get_window()->set_cursor(Gdk::Cursor::create(Gdk::BLANK_CURSOR));
}

void Dingo::VideoArea::updateUnFullscreenActivated() {
  if (d_cursor_hide_connection.connected()) {
    d_cursor_hide_connection.disconnect();
  }
  
  get_window()->set_cursor(Gdk::Cursor::create(Gdk::LEFT_PTR));
  
  d_cursor_movement_count = 0;
}

void Dingo::VideoArea::updateEnginePlaying() {
  if (d_draw_connection.connected()) {
    d_draw_connection.disconnect();
  }
  
  if (d_playerengine->getMIMEType() == "audio/*") {
    d_draw_connection = signal_draw().connect(sigc::mem_fun(*this, &Dingo::VideoArea::onVideoAreaDraw));
  }
}

void Dingo::VideoArea::updateEnginePaused() {

}

void Dingo::VideoArea::updateEngineStopped() {
  d_draw_connection = signal_draw().connect(sigc::mem_fun(*this, &Dingo::VideoArea::onVideoAreaDraw));
}

void Dingo::VideoArea::updateEngineNewPlaying() {
  if (d_draw_connection.connected()) {
    d_draw_connection.disconnect();
  }
  
  if (d_playerengine->getMIMEType() == "audio/*") {
    d_draw_connection = signal_draw().connect(sigc::mem_fun(*this, &Dingo::VideoArea::onVideoAreaDraw));
  }
}
    
//CREATORS
void Dingo::VideoArea::createVideoArea() {
  set_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK);
  
  override_background_color(temp_rgba, Gtk::STATE_FLAG_NORMAL);
  
  d_draw_connection = signal_draw().connect(sigc::mem_fun(*this, &Dingo::VideoArea::onVideoAreaDraw));
}
      
//SIGNAL HANDLERS
bool Dingo::VideoArea::onVideoAreaDraw(const Cairo::RefPtr<Cairo::Context>& cairo_context) {
  Glib::RefPtr<Gdk::Pixbuf> temp_pixbuf = Gdk::Pixbuf::create_from_file(DATADIR"/dingo/icons/128x128/dingo.png");
  
  Gtk::DrawingArea::on_draw(cairo_context);
  
  if (!temp_pixbuf) {
    return false;
    
    //std::cout << "Couldn't create pixbuf" << std::endl;
  }
  
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  
  Gdk::Cairo::set_source_pixbuf(cairo_context, temp_pixbuf, (width - temp_pixbuf->get_width()) / 2, (height - temp_pixbuf->get_height()) / 2);
  
  cairo_context->paint();
  
  return true;
}

bool Dingo::VideoArea::on_button_press_event(GdkEventButton* event) {
  if ((event->type == GDK_2BUTTON_PRESS) && (event->button == 1)) {
    if (!d_playerengine->getFullscreen()) {
      d_playerengine->setFullscreen(true);
    }
    
    else {
      d_playerengine->setFullscreen(false);
    }
    
    return true;
  }
  
  return false;
}

bool Dingo::VideoArea::on_motion_notify_event(GdkEventMotion* event) {
  if ((event->type == GDK_MOTION_NOTIFY) && (d_playerengine->getFullscreen())) {
    ++d_cursor_movement_count;
  
    if (d_cursor_movement_count == 50) {
      notifyMouseMotionObserver(Dingo::MOUSE_HOVER_ON_FULLSCREEN);
    
      get_window()->set_cursor(Gdk::Cursor::create(Gdk::LEFT_PTR));
    
      if (d_cursor_hide_connection.connected()) {
        d_cursor_hide_connection.disconnect();
      }
    
      d_cursor_hide_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &Dingo::VideoArea::onCursorHideTimeOut), 8000);
      
      d_cursor_movement_count = 0;
    
      return true;
    }
  }
  
  return false;
}

bool Dingo::VideoArea::onCursorHideTimeOut() {
  get_window()->set_cursor(Gdk::Cursor::create(Gdk::BLANK_CURSOR));
  
  d_cursor_movement_count = 0;

  return false;
}
