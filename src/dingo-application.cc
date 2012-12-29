#include "dingo-application.h"

#ifndef INCLUDED_GSTREAMERMM_H
#define INCLUDED_GSTREAMERMM_H
#include <gstreamermm.h>
#endif //INCLUDED_GSTREAMERMM_H

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

//CONSTRUCTORS - DESTRUCTOR
Dingo::Application::Application(int argc, char** argv) :
  Gtk::Application(argc, argv, "manager.media.dingo", Gio::APPLICATION_HANDLES_COMMAND_LINE | Gio::APPLICATION_HANDLES_OPEN)
{
  Gtk::Application::hold();
  
  notify_init(Glib::ustring("Dingo").c_str());
  
  d_programoptions.reset(new Dingo::ProgramOptions(argc, argv));
  
  Gst::init(argc, argv);
}
   
Dingo::Application::~Application() {
  
}

//INHERITED PUBLIC METHODS
int Dingo::Application::run() {
  int return_result = 0;

  if (!d_programoptions->getShouldDingoQuit()) {
    d_dbmanager.reset(new Dingo::DBManager(Glib::get_home_dir() + "/.config/dingo-0.5/database/dingo.db"));
    d_playerengine.reset(new Dingo::PlayerEngine());
    d_settings.reset(new Dingo::Settings());
    d_mpriscommon.reset(new Dingo::MPRISCommon(d_dbmanager.get(), d_playerengine.get(), d_settings.get(), this));
  
    d_mainwindow.reset(new Dingo::MainWindow(d_dbmanager.get(), d_playerengine.get(), d_settings.get(), this, d_mpriscommon.get(), d_programoptions.get()));
  
    Dingo::MPRISv2::createMPRISv2(d_mpriscommon.get());
  
    d_programoptions->run(d_dbmanager.get(), d_settings.get());
    
    return_result = Gtk::Application::run(*d_mainwindow);
  }
  
  Gtk::Application::release();
  
  return return_result;
}

//SIGNAL HANDLERS
int Dingo::Application::on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line) {
  Gtk::Application::activate();
}
