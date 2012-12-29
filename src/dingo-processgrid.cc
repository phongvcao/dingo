#include "dingo-processgrid.h"
#include "dingo-processgridobserver.h"
#include "dingo-utilities.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

#ifndef INCLUDED_GLIBMM_I18N_H
#define INCLUDED_GLIBMM_I18N_H
#include <glibmm/i18n.h>
#endif //INCLUDED_GLIBMM_I18N_H

//CONSTRUCTORS - DESTRUCTOR
Dingo::ProcessGrid::ProcessGrid() :
  Gtk::Grid()
{
  createWidgets();
  createGrid();
  
  d_process_id = Dingo::PROCESS_IMPORTING_FILES;
  
  //set_size_request(265, 85);
}

Dingo::ProcessGrid::~ProcessGrid() {

}

//INHERITED OBSERVER'S METHODS
void Dingo::ProcessGrid::registerProcessGridObserver(Dingo::ProcessGridObserver* observer) {
  processgrid_obs_vector.push_back(observer);
}

void Dingo::ProcessGrid::removeProcessGridObserver(Dingo::ProcessGridObserver* observer) {
  std::vector<Dingo::ProcessGridObserver*>::iterator iter = std::find(processgrid_obs_vector.begin(), processgrid_obs_vector.end(), observer);
  
  if (iter != processgrid_obs_vector.end()) {
    processgrid_obs_vector.erase(iter);
  }
}

void Dingo::ProcessGrid::notifyProcessGridObserver(Dingo::ProcessSignals signal_type) {
  switch (signal_type) {
    case Dingo::PROCESS_FINISHED: {
      for (std::vector<Dingo::ProcessGridObserver*>::iterator iter = processgrid_obs_vector.begin(); iter != processgrid_obs_vector.end(); ++iter) {
        (*iter)->updateProcessFinished(d_process_id);
      }
      //std::cout << "Process Finished!" << std::endl;
      break;
    }
    
    default: {
      std::cerr << "Undefined Dingo::ProcessGridSignals!" << std::endl;
      break;
    }
  }
}
      
//PUBLIC ACCESSORS
void Dingo::ProcessGrid::setTotalFilesCount(int total_files_count) {
  d_total_files_count = total_files_count;
}

void Dingo::ProcessGrid::setCurrentFilesCount(int current_files_count) {
  setPulse(false);

  d_message_title_label.set_markup(Glib::ustring::compose(d_message_title, current_files_count, d_total_files_count));
  
  double double_current_files_count = current_files_count;
  double double_total_files_count = d_total_files_count;
  
  d_progress_bar.set_fraction(double_current_files_count / double_total_files_count);
}

void Dingo::ProcessGrid::setCurrentFileName(Glib::ustring current_file_name) {
  d_file_name_label.set_markup(Glib::ustring::compose("<i>" + Glib::ustring(_("File: %1")) + "</i>", current_file_name));
}
      
void Dingo::ProcessGrid::setCurrentProcessID(Dingo::ProcessID process_id) {
  d_process_id = process_id;
  
  setPulse(true);
  
  show_all();
  
  switch (d_process_id) {
    case Dingo::PROCESS_IMPORTING_FILES: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Importing Tracks")));
      d_message_title = Dingo::Utilities::highlightString(_("Importing Track %1 of %2"));
      
      break;
    }
    
    case Dingo::PROCESS_IMPORTING_FOLDERS: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Importing Folders")));
      d_message_title = Dingo::Utilities::highlightString(_("Importing Track %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_IMPORTING_PLAYLISTS: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Importing Playlists")));
      d_message_title = Dingo::Utilities::highlightString(_("Importing Track %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_EXPORTING_PLAYLIST: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Exporting Playlist")));
      d_message_title = Dingo::Utilities::highlightString(_("Exporting Track %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_CUTTING_TRACKS: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Cutting Track(s)")));
      d_message_title = Dingo::Utilities::highlightString(_("Cutting Track %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_COPYING_TRACKS: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Copying Track(s)")));
      d_message_title = Dingo::Utilities::highlightString(_("Copying Track %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_PASTING_TRACKS: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Pasting Track(s)")));
      d_message_title = Dingo::Utilities::highlightString(_("Pasting Track %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_ADDING_TO_PLAYQUEUE: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Adding Track(s) to Play Queue")));
      d_message_title = Dingo::Utilities::highlightString(_("Adding Track %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_ADDING_TO_PLAYLISTS: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Adding Track(s) to Playlists")));
      d_message_title = Dingo::Utilities::highlightString(_("Adding Track %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_DELETING_FROM_DATABASE: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Removing Track(s) from Database")));
      d_message_title = Dingo::Utilities::highlightString(_("Removing Track %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_DELETING_FROM_PLAYQUEUE: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Removing Track(s) from Play Queue")));
      d_message_title = Dingo::Utilities::highlightString(_("Removing Track %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_DELETING_FROM_HARDDRIVE: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Deleting Track(s) from Hard Drive")));
      d_message_title = Dingo::Utilities::highlightString(_("Deleting Track %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_DELETING_FROM_PLAYLISTS: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Removing Track(s) from Playlists")));
      d_message_title = Dingo::Utilities::highlightString(_("Removing Track %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_DELETING_FROM_CURRENT_PLAYLIST: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Removing Track(s) from Current Playlist")));
      
      d_message_title = Dingo::Utilities::highlightString(_("Removing Track %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_EDITING_SINGLE_TRACK: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Saving Track's Info")));
      d_message_title = Dingo::Utilities::highlightString(_("Saving Info Track %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_EDITING_MULTIPLE_TRACKS: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Saving Tracks' Info")));
      d_message_title = Dingo::Utilities::highlightString(_("Saving Info Track %1 of %2"));
      
      break;
    }
    
    case Dingo::PROCESS_SETTINGS_LOADING_FROM_FILE: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Loading Settings from File")));
      d_message_title = Dingo::Utilities::highlightString(_("Loading Setting %1 of %2"));
      
      break;
    }
    
    case Dingo::PROCESS_SETTINGS_SAVING_TO_FILE: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Saving Settings to File")));
      d_message_title = Dingo::Utilities::highlightString(_("Saving Setting %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_SETTINGS_SAVING_CURRENT_AS_DEFAULT: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Saving Current Settings as Default")));
      d_message_title = Dingo::Utilities::highlightString(_("Saving Setting %1 of %2"));
    
      break;
    }
    
    case Dingo::PROCESS_SETTINGS_RESTORING_TO_DEFAULT: {
      d_message_title_label.set_markup(Dingo::Utilities::highlightString(_("Restoring to Default Settings")));
      d_message_title = Dingo::Utilities::highlightString(_("Restoring Setting %1 of %2"));
    
      break;
    }
    
    default: {
      hide();
      setPulse(false);
    
      break;
    }
  }
}

Dingo::ProcessID Dingo::ProcessGrid::getCurrentProcessID() {
  return d_process_id;
}

void Dingo::ProcessGrid::setPulse(bool is_pulse) {
  d_is_pulse = is_pulse;

  if (d_is_pulse) {
    if (d_pulse_connection.connected()) {
      d_pulse_connection.disconnect();
    }
  
    d_pulse_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &Dingo::ProcessGrid::onPulseTimeOut), 50);
  }
}

//CREATORS
void Dingo::ProcessGrid::createWidgets() {
  d_cancel_button.set_image(*(Gtk::manage(new Gtk::Image(Gtk::Stock::CANCEL, Gtk::ICON_SIZE_MENU))));
  d_cancel_button.set_size_request(20, 20);
  d_cancel_button.signal_clicked().connect(sigc::mem_fun(*this, &Dingo::ProcessGrid::onCancelButtonClicked));
  
  d_message_title_label.set_ellipsize(Pango::ELLIPSIZE_END);
  d_file_name_label.set_ellipsize(Pango::ELLIPSIZE_END);
}

void Dingo::ProcessGrid::createGrid() {
  attach(d_message_title_label, 0, 0, 2, 1);
  attach(d_progress_bar, 0, 1, 1, 1);
  attach(d_cancel_button, 1, 1, 1, 1);
  attach(d_file_name_label, 0, 2, 2, 1);
  
  set_hexpand(true);
  
  set_row_spacing(3);
  set_column_spacing(5);
  set_border_width(5);
  
  d_message_title_label.set_hexpand(true);
  d_progress_bar.set_hexpand(true);
  d_file_name_label.set_hexpand(true);
  
  d_message_title_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_file_name_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_progress_bar.set_pulse_step(0.1);
}

//SIGNAL HANDLERS
void Dingo::ProcessGrid::onCancelButtonClicked() {
  notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
}

bool Dingo::ProcessGrid::onPulseTimeOut() {
  if (d_is_pulse) {
    d_progress_bar.pulse();
  }
  
  return d_is_pulse;
}
