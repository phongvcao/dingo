#include "dingo-statusbar.h"

//CONSTRUCTORS - DESTRUCTOR
Dingo::StatusBar::StatusBar() :
  Gtk::Statusbar()
{
  Gtk::Box* message_box = dynamic_cast<Gtk::Box*>(get_message_area());
  
  message_box->set_halign(Gtk::ALIGN_CENTER);
}

Dingo::StatusBar::~StatusBar() {

}
