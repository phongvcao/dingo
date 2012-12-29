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
