/* 
 * dingo-processgrid.h
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

#ifndef INCLUDED_DINGO_PROCESSGRID_H
#define INCLUDED_DINGO_PROCESSGRID_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#include "dingo-header.h"

namespace Dingo {
  class ProcessGridObserver;

  class ProcessGrid : public Gtk::Grid {
    public:
      ProcessGrid();
      virtual ~ProcessGrid();
      
      //INHERITED OBSERVER'S METHODS
      void registerProcessGridObserver(ProcessGridObserver* observer);
      void removeProcessGridObserver(ProcessGridObserver* observer);
      void notifyProcessGridObserver(ProcessSignals signal_type);
      
      //PUBLIC ACCESSORS
      void setTotalFilesCount(int total_files_count);
      void setCurrentFilesCount(int current_files_count);
      void setCurrentFileName(Glib::ustring current_file_name);
      
      void setCurrentProcessID(ProcessID process_id);
      ProcessID getCurrentProcessID();
      
      void setPulse(bool is_pulse);
      
    private:
      //CREATORS
      void createWidgets();
      void createGrid();
      
      //SIGNAL HANDLERS
      void onCancelButtonClicked();
      bool onPulseTimeOut();
      bool onMainIterationTimeOut();
    
      //DATA MEMBERS
      Gtk::Label d_message_title_label;
      Gtk::Label d_file_name_label;
      Gtk::Button d_cancel_button;
      Gtk::ProgressBar d_progress_bar;
      
      Glib::ustring d_message_title;
      int d_total_files_count;
      sigc::connection d_pulse_connection;
      bool d_is_pulse;
      
      std::vector<ProcessGridObserver*> processgrid_obs_vector;
      ProcessID d_process_id;
  };
}

#endif //INCLUDED_DINGO_PROCESSGRID_H
