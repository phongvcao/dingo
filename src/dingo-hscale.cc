/* 
 * dingo-hscale.cc
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA."
 *
 */

#include "dingo-hscale.h"

//CONSTRUCTORS - DESTRUCTOR
Dingo::HScale::HScale() :
  Gtk::HScale(0.00, 1.00, 0.01)
{

}

Dingo::HScale::~HScale() {

}

//SIGNAL HANDLERS
bool Dingo::HScale::on_button_press_event(GdkEventButton* event) {
  bool is_left_clicked = false;

  if (event->button == 1) {
    is_left_clicked = true;
  }
  
  if (is_left_clicked) {
    event->button = 2;
    
    Gtk::HScale::on_button_press_event(event);
    
    event->button = 3;
    
    Gtk::HScale::on_button_press_event(event);
  }
  
  return true;
}

bool Dingo::HScale::on_button_release_event(GdkEventButton* event) {
  bool is_left_clicked = false;

  if (event->button == 1) {
    is_left_clicked = true;
  }
  
  if (is_left_clicked) {
    event->button = 2;
    
    Gtk::HScale::on_button_release_event(event);
    
    event->button = 3;
    
    Gtk::HScale::on_button_release_event(event);
  }
  
  return true;
}
