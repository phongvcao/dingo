/* 
 * dingo-settings.h
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

#ifndef INCLUDED_DINGO_SETTINGS_H
#define INCLUDED_DINGO_SETTINGS_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#include "dingo-header.h"

namespace Dingo {
  class SettingsActionObserver;

  class Settings {
    public:
      Settings();
      virtual ~Settings();
      
      //OBSERVER'S METHODS
      void registerSettingsActionObserver(SettingsActionObserver* observer);
      void removeSettingsActionObserver(SettingsActionObserver* observer);
      void notifySettingsActionObserver(SettingsSignals signal_type);
      
      //PUBLIC METHODS
      void saveToDefault();
      void readFromDefault();
      
      void saveToFile(Glib::ustring file_uri);
      bool readFromFile(Glib::ustring file_uri);
      
      //PUBLIC ACCESSORS
      RepeatMode getRepeat();
      PlayOrder getPlayOrder();
      double getVolume();
      int getSortColumnID();
      Gtk::SortType getSortType();
      bool getSubtitleShow();
      Glib::ustring getSubtitleFontDesc();
      bool getVisible(Glib::ustring widget_name);
      bool getActive(Glib::ustring toggle_name);
      int getWidth(Glib::ustring widget_name);
      int getHeight(Glib::ustring widget_name);
      
      void setRepeat(Dingo::RepeatMode repeat_mode);
      void setPlayOrder(Dingo::PlayOrder play_order);
      void setVolume(double volume);
      void setSortColumnID(int column_id);
      void setSortType(Gtk::SortType sort_type);
      void setSubtitleShow(bool is_show);
      void setSubtitleFontDesc(Glib::ustring font_desc);
      void setVisible(Glib::ustring widget_name, bool is_visible);
      void setActive(Glib::ustring toggle_name, bool is_active);
      void setWidth(Glib::ustring widget_name, int width);
      void setHeight(Glib::ustring widget_name, int height);
     
    private:
      //UTILITIES
      void assignDataDefaultValues();
    
      /* ====================================== */
      /* DATA MEMBERS */      
      //<repeat> values
      RepeatMode d_read_repeat;  //notify Dingo::ControlBar - implemented
      
      //<playorder> values
      PlayOrder d_read_playorder;  //notify Dingo::ControlBar - implemented
      
      //<volume> values
      double d_read_volume;  //notify Dingo::ControlBar - implemented
      
      //<sort> values
      int d_read_sort_column_id;  //notify Dingo::TrackTreeView - implemented
      Gtk::SortType d_read_sort_type;  //notify Dingo::TrackTreeView - implemented
      
      //<subtitle> values
      bool d_read_subtitle_show;  //notify Dingo::MenuBar - implemented
      Glib::ustring d_read_font_desc;  //notify Dingo::MenuBar - implemented
      
      //<visible> values
      bool d_read_playlistbrowser_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_trackbrowser_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_playbackstatus_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_tracknumber_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_tracktitle_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_trackgenre_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_trackartist_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_trackalbum_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_trackyear_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_tracktime_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_trackfileextension_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_trackbitrate_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_genrefilter_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_artistfilter_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_yearfilter_visible;  //notify Dingo::MenuBar - implemented
      bool d_read_albumfilter_visible;  //notify Dingo::MenuBar - implemented
      
      //<active> values
      bool d_read_home_active;  //notify Dingo::ToolBar - implemented
      bool d_read_nowplaying_active;  //notify Dingo::ToolBar - implemented
      bool d_read_playqueue_active;  //notify Dingo::ToolBar - implemented
      bool d_read_audioonly_active;  //notify Dingo::ToolBar - implemented
      bool d_read_videoonly_active;  //notify Dingo::ToolBar - implemented
      
      //<width> values
      int d_read_topwindow_width;  //notify Dingo::MainWindow - implemented
      int d_read_playlisthpaned_width;  //notify Dingo::MainWindow - implemented
      int d_read_genrehpaned_width;  //notify Dingo::MainWindow - implemented
      int d_read_artisthpaned_width;  //notify Dingo::MainWindow - implemented
      int d_read_yearhpaned_width;  //notify Dingo::MainWindow - implemented
      int d_read_tracktitle_width;  //notify Dingo::TrackTreeView - implemented
      int d_read_trackgenre_width;  //notify Dingo::TrackTreeView - implemented
      int d_read_trackartist_width;  //notify Dingo::TrackTreeView - implemented
      int d_read_trackalbum_width;  //notify Dingo::TrackTreeView - implemented
      int d_read_trackyear_width;  //notify Dingo::TrackTreeView - implemented
      
      //<height> values
      int d_read_topwindow_height;  //notify Dingo::MainWindow - implemented
      int d_read_topvpaned_height;  //notify Dingo::MainWindow - implemented
      
      std::vector<SettingsActionObserver*> action_obs_vector;
  };
}

#endif //INCLUDED_DINGO_SETTINGS_H
