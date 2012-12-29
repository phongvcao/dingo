/* 
 * dingo-settings.cc
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

#include "dingo-settings.h"
#include "dingo-settingsreader.h"
#include "dingo-settingswriter.h"
#include "dingo-settingsactionobserver.h"
#include "dingo-utilities.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

//CONSTRUCTORS - DESTRUCTOR
Dingo::Settings::Settings() {
  assignDataDefaultValues();
}

Dingo::Settings::~Settings() {

}

//OBSERVER'S METHODS
void Dingo::Settings::registerSettingsActionObserver(Dingo::SettingsActionObserver* observer) {
  action_obs_vector.push_back(observer);
}

void Dingo::Settings::removeSettingsActionObserver(Dingo::SettingsActionObserver* observer) {
  std::vector<Dingo::SettingsActionObserver*>::iterator iter = std::find(action_obs_vector.begin(), action_obs_vector.end(), observer);
  
  if (iter != action_obs_vector.end()) {
    action_obs_vector.erase(iter);
  }
}

void Dingo::Settings::notifySettingsActionObserver(Dingo::SettingsSignals signal_type) {
  switch (signal_type) {
    case Dingo::SETTINGS_READ: {
      for (std::vector<Dingo::SettingsActionObserver*>::iterator iter = action_obs_vector.begin(); iter != action_obs_vector.end(); ++iter) {
        (*iter)->updateSettingsRead();
      }
      break;
    }
    
    case Dingo::SETTINGS_WRITE: {
      for (std::vector<Dingo::SettingsActionObserver*>::iterator iter = action_obs_vector.begin(); iter != action_obs_vector.end(); ++iter) {
        (*iter)->updateSettingsWrite();     
      }
      break;
    }
    
    default: {
      break;
    }
  }
}

//PUBLIC METHODS
void Dingo::Settings::saveToDefault() {
  Dingo::Utilities::createSettingsFolders();

  saveToFile(Glib::filename_to_uri(Glib::get_home_dir() + "/.config/dingo-0.5/settings/dingo-default.dgs"));
}

void Dingo::Settings::readFromDefault() {
  if (!readFromFile(Glib::filename_to_uri(Glib::get_home_dir() + "/.config/dingo-0.5/settings/dingo-default.dgs"))) {
    Dingo::Utilities::createSettingsFolders();

    saveToFile(Glib::filename_to_uri(Glib::get_home_dir() + "/.config/dingo-0.5/settings/dingo-default.dgs"));
  }
}
      
void Dingo::Settings::saveToFile(Glib::ustring file_uri) {
  Dingo::Utilities::createSettingsFolders();

  Dingo::SettingsWriter settingswriter;
  
  settingswriter.open(Dingo::Utilities::addFileExtension(file_uri, ".dgs"), "default");
  
  settingswriter.createSettings();
  
  settingswriter.setRepeat(d_read_repeat);
  settingswriter.setPlayOrder(d_read_playorder);
  settingswriter.setVolume(d_read_volume);
  settingswriter.setSortColumnID(d_read_sort_column_id);
  settingswriter.setSortType(d_read_sort_type);
  settingswriter.setSubtitleShow(d_read_subtitle_show);
  settingswriter.setSubtitleFontDesc(d_read_font_desc);
  
  settingswriter.setVisible("playlistbrowser", d_read_playlistbrowser_visible);
  settingswriter.setVisible("trackbrowser", d_read_trackbrowser_visible);
  settingswriter.setVisible("genrefilter", d_read_genrefilter_visible);
  settingswriter.setVisible("artistfilter", d_read_artistfilter_visible);
  settingswriter.setVisible("yearfilter", d_read_yearfilter_visible);
  settingswriter.setVisible("albumfilter", d_read_albumfilter_visible);
  settingswriter.setVisible("playbackstatus", d_read_playbackstatus_visible);
  settingswriter.setVisible("tracknumber", d_read_tracknumber_visible);
  settingswriter.setVisible("tracktitle", d_read_tracktitle_visible);
  settingswriter.setVisible("trackgenre", d_read_trackgenre_visible);
  settingswriter.setVisible("trackartist", d_read_trackartist_visible);
  settingswriter.setVisible("trackalbum", d_read_trackalbum_visible);
  settingswriter.setVisible("trackyear", d_read_trackyear_visible);
  settingswriter.setVisible("tracktime", d_read_tracktime_visible);
  settingswriter.setVisible("trackfileextension", d_read_trackfileextension_visible);
  settingswriter.setVisible("trackbitrate", d_read_trackbitrate_visible);
  
  settingswriter.setActive("home", d_read_home_active);
  settingswriter.setActive("nowplaying", d_read_nowplaying_active);
  settingswriter.setActive("playqueue", d_read_playqueue_active);
  settingswriter.setActive("audioonly", d_read_audioonly_active);
  settingswriter.setActive("videoonly", d_read_videoonly_active);
  
  settingswriter.setWidth("topwindow", d_read_topwindow_width);
  settingswriter.setWidth("playlisthpaned", d_read_playlisthpaned_width);
  settingswriter.setWidth("genrehpaned", d_read_genrehpaned_width);
  settingswriter.setWidth("artisthpaned", d_read_artisthpaned_width);
  settingswriter.setWidth("yearhpaned", d_read_yearhpaned_width);
  settingswriter.setWidth("tracktitle", d_read_tracktitle_width);
  settingswriter.setWidth("trackgenre", d_read_trackgenre_width);
  settingswriter.setWidth("trackartist", d_read_trackartist_width);
  settingswriter.setWidth("trackalbum", d_read_trackalbum_width);
  settingswriter.setWidth("trackyear", d_read_trackyear_width);
  
  settingswriter.setHeight("topwindow", d_read_topwindow_height);
  settingswriter.setHeight("topvpaned", d_read_topvpaned_height);
  
  //PUBLIC SAVER/WRITER
  settingswriter.saveSettings();
  settingswriter.writeToFile();
}

bool Dingo::Settings::readFromFile(Glib::ustring file_uri) {
  Dingo::SettingsReader settingsreader;
  
  Glib::RefPtr<Gio::File> settings_file = Gio::File::create_for_uri(file_uri);
  
  if (settings_file->query_exists()) {
    if (settingsreader.open(file_uri)) {
      d_read_repeat = settingsreader.getRepeat();
      d_read_playorder = settingsreader.getPlayOrder();
      d_read_volume = settingsreader.getVolume();
      d_read_sort_column_id = settingsreader.getSortColumnID();
      d_read_sort_type = settingsreader.getSortType();
      d_read_subtitle_show = settingsreader.getSubtitleShow();
      d_read_font_desc = settingsreader.getSubtitleFontDesc();
  
      //multiple calls
      d_read_playlistbrowser_visible = settingsreader.getVisible("playlistbrowser");
      d_read_trackbrowser_visible = settingsreader.getVisible("trackbrowser");
      d_read_playbackstatus_visible = settingsreader.getVisible("playbackstatus");
      d_read_tracknumber_visible = settingsreader.getVisible("tracknumber");
      d_read_tracktitle_visible = settingsreader.getVisible("tracktitle");
      d_read_trackgenre_visible = settingsreader.getVisible("trackgenre");
      d_read_trackartist_visible = settingsreader.getVisible("trackartist");
      d_read_trackalbum_visible = settingsreader.getVisible("trackalbum");
      d_read_trackyear_visible = settingsreader.getVisible("trackyear");
      d_read_tracktime_visible = settingsreader.getVisible("tracktime");
      d_read_trackfileextension_visible = settingsreader.getVisible("trackfileextension");
      d_read_trackbitrate_visible = settingsreader.getVisible("trackbitrate");
      d_read_genrefilter_visible = settingsreader.getVisible("genrefilter");
      d_read_artistfilter_visible = settingsreader.getVisible("artistfilter");
      d_read_yearfilter_visible = settingsreader.getVisible("yearfilter");
      d_read_albumfilter_visible = settingsreader.getVisible("albumfilter");
  
      //multiple calls
      d_read_home_active = settingsreader.getActive("home");
      d_read_nowplaying_active = settingsreader.getActive("nowplaying");
      d_read_playqueue_active = settingsreader.getActive("playqueue");
      d_read_audioonly_active = settingsreader.getActive("audioonly");
      d_read_videoonly_active = settingsreader.getActive("videoonly");
  
      //multiple calls
      d_read_topwindow_width = settingsreader.getWidth("topwindow");
      d_read_playlisthpaned_width = settingsreader.getWidth("playlisthpaned");
      d_read_genrehpaned_width = settingsreader.getWidth("genrehpaned");
      d_read_artisthpaned_width = settingsreader.getWidth("artisthpaned");
      d_read_yearhpaned_width = settingsreader.getWidth("yearhpaned");
      d_read_tracktitle_width = settingsreader.getWidth("tracktitle");
      d_read_trackgenre_width = settingsreader.getWidth("trackgenre");
      d_read_trackartist_width = settingsreader.getWidth("trackartist");
      d_read_trackalbum_width = settingsreader.getWidth("trackalbum");
      d_read_trackyear_width = settingsreader.getWidth("trackyear");
  
      //multiple calls
      d_read_topwindow_height = settingsreader.getHeight("topwindow");
      d_read_topvpaned_height = settingsreader.getHeight("topvpaned");
    
      return true;
    }
    
    else {
      return false;
    }
  }
  
  else {
    return false;
  }
}

//PUBLIC ACCESSORS
Dingo::RepeatMode Dingo::Settings::getRepeat() {
  return d_read_repeat;
}

Dingo::PlayOrder Dingo::Settings::getPlayOrder() {
  return d_read_playorder;
}

double Dingo::Settings::getVolume() {
  return d_read_volume;
}

int Dingo::Settings::getSortColumnID() {
  return d_read_sort_column_id;
}

Gtk::SortType Dingo::Settings::getSortType() {
  return d_read_sort_type;
}

bool Dingo::Settings::getSubtitleShow() {
  return d_read_subtitle_show;
}

Glib::ustring Dingo::Settings::getSubtitleFontDesc() {
  return d_read_font_desc;
}

bool Dingo::Settings::getVisible(Glib::ustring widget_name) {
  if (widget_name == "playlistbrowser") {
    return d_read_playlistbrowser_visible;
  }
  
  else if (widget_name == "trackbrowser") {
    return d_read_trackbrowser_visible;
  }
  
  else if (widget_name == "playbackstatus") {
    return d_read_playbackstatus_visible;
  }
  
  else if (widget_name == "tracknumber") {
    return d_read_tracknumber_visible;
  }
  
  else if (widget_name == "tracktitle") {
    return d_read_tracktitle_visible;
  }
  
  else if (widget_name == "trackgenre") {
    return d_read_trackgenre_visible;
  }
  
  else if (widget_name == "trackartist") {
    return d_read_trackartist_visible;
  }
  
  else if (widget_name == "trackalbum") {
    return d_read_trackalbum_visible;
  }
  
  else if (widget_name == "trackyear") {
    return d_read_trackyear_visible;
  }
  
  else if (widget_name == "tracktime") {
    return d_read_tracktime_visible;
  }
  
  else if (widget_name == "trackfileextension") {
    return d_read_trackfileextension_visible;
  }
  
  else if (widget_name == "trackbitrate") {
    return d_read_trackbitrate_visible;
  }
  
  else if (widget_name == "genrefilter") {
    return d_read_genrefilter_visible;
  }
  
  else if (widget_name == "artistfilter") {
    return d_read_artistfilter_visible;
  }
  
  else if (widget_name == "yearfilter") {
    return d_read_yearfilter_visible;
  }
  
  else if (widget_name == "albumfilter") {
    return d_read_albumfilter_visible;
  }
}

bool Dingo::Settings::getActive(Glib::ustring toggle_name) {
  if (toggle_name == "home") {
    return d_read_home_active;
  }
  
  else if (toggle_name == "nowplaying") {
    return d_read_nowplaying_active;
  }
  
  else if (toggle_name == "playqueue") {
    return d_read_playqueue_active;
  }
  
  else if (toggle_name == "audioonly") {
    return d_read_audioonly_active;
  }
  
  else if (toggle_name == "videoonly") {
    return d_read_videoonly_active;
  }
}

int Dingo::Settings::getWidth(Glib::ustring widget_name) {
  if (widget_name == "topwindow") {
    return d_read_topwindow_width;
  }
  
  else if (widget_name == "playlisthpaned") {
    return d_read_playlisthpaned_width;
  }
  
  else if (widget_name == "genrehpaned") {
    return d_read_genrehpaned_width;
  }
  
  else if (widget_name == "artisthpaned") {
    return d_read_artisthpaned_width;
  }
  
  else if (widget_name == "yearhpaned") {
    return d_read_yearhpaned_width;
  }
  
  else if (widget_name == "tracktitle") {
    return d_read_tracktitle_width;
  }
  
  else if (widget_name == "trackgenre") {
    return d_read_trackgenre_width;
  }
  
  else if (widget_name == "trackartist") {
    return d_read_trackartist_width;
  }
  
  else if (widget_name == "trackalbum") {
    return d_read_trackalbum_width;
  }
  
  else if (widget_name == "trackyear") {
    return d_read_trackyear_width;
  }
}

int Dingo::Settings::getHeight(Glib::ustring widget_name) {
  if (widget_name == "topwindow") {
    return d_read_topwindow_height;
  }
  
  else if (widget_name == "topvpaned") {
    return d_read_topvpaned_height;
  }
}

void Dingo::Settings::setRepeat(Dingo::RepeatMode repeat_mode) {
  d_read_repeat = repeat_mode;
}

void Dingo::Settings::setPlayOrder(Dingo::PlayOrder play_order) {
  d_read_playorder = play_order;
}

void Dingo::Settings::setVolume(double volume) {
  d_read_volume = volume;
}

void Dingo::Settings::setSortColumnID(int column_id) {
  d_read_sort_column_id = column_id;
}

void Dingo::Settings::setSortType(Gtk::SortType sort_type) {
  d_read_sort_type = sort_type;
}

void Dingo::Settings::setSubtitleShow(bool is_show) {
  d_read_subtitle_show = is_show;
}

void Dingo::Settings::setSubtitleFontDesc(Glib::ustring font_desc) {
  d_read_font_desc = font_desc;
}

void Dingo::Settings::setVisible(Glib::ustring widget_name, bool is_visible) {
  if (widget_name == "playlistbrowser") {
    d_read_playlistbrowser_visible = is_visible;
  }
  
  else if (widget_name == "trackbrowser") {
    d_read_trackbrowser_visible = is_visible;
  }
  
  else if (widget_name == "playbackstatus") {
    d_read_playbackstatus_visible = is_visible;
  }
  
  else if (widget_name == "tracknumber") {
    d_read_tracknumber_visible = is_visible;
  }
  
  else if (widget_name == "tracktitle") {
    d_read_tracktitle_visible = is_visible;
  }
  
  else if (widget_name == "trackgenre") {
    d_read_trackgenre_visible = is_visible;
  }
  
  else if (widget_name == "trackartist") {
    d_read_trackartist_visible = is_visible;
  }
  
  else if (widget_name == "trackalbum") {
    d_read_trackalbum_visible = is_visible;
  }
  
  else if (widget_name == "trackyear") {
    d_read_trackyear_visible = is_visible;
  }
  
  else if (widget_name == "tracktime") {
    d_read_tracktime_visible = is_visible;
  }
  
  else if (widget_name == "trackfileextension") {
    d_read_trackfileextension_visible = is_visible;
  }
  
  else if (widget_name == "trackbitrate") {
    d_read_trackbitrate_visible = is_visible;
  }
  
  else if (widget_name == "genrefilter") {
    d_read_genrefilter_visible = is_visible;
  }
  
  else if (widget_name == "artistfilter") {
    d_read_artistfilter_visible = is_visible;
  }
  
  else if (widget_name == "yearfilter") {
    d_read_yearfilter_visible = is_visible;
  }
  
  else if (widget_name == "albumfilter") {
    d_read_albumfilter_visible = is_visible;
  }
}

void Dingo::Settings::setActive(Glib::ustring toggle_name, bool is_active) {
  if (toggle_name == "home") {
    d_read_home_active = is_active;
  }
  
  else if (toggle_name == "nowplaying") {
    d_read_nowplaying_active = is_active;
  }
  
  else if (toggle_name == "playqueue") {
    d_read_playqueue_active = is_active;
  }
  
  else if (toggle_name == "audioonly") {
    d_read_audioonly_active = is_active;
  }
  
  else if (toggle_name == "videoonly") {
    d_read_videoonly_active = is_active; 
  }
}

void Dingo::Settings::setWidth(Glib::ustring widget_name, int width) {
  if (widget_name == "topwindow") {
    d_read_topwindow_width = width;
  }
  
  else if (widget_name == "playlisthpaned") {
    d_read_playlisthpaned_width = width;
  }
  
  else if (widget_name == "genrehpaned") {
    d_read_genrehpaned_width = width;
  }
  
  else if (widget_name == "artisthpaned") {
    d_read_artisthpaned_width = width;
  }
  
  else if (widget_name == "yearhpaned") {
    d_read_yearhpaned_width = width;
  }
  
  else if (widget_name == "tracktitle") {
    d_read_tracktitle_width = width;
  }
  
  else if (widget_name == "trackgenre") {
    d_read_trackgenre_width = width;
  }
  
  else if (widget_name == "trackartist") {
    d_read_trackartist_width = width;
  }
  
  else if (widget_name == "trackalbum") {
    d_read_trackalbum_width = width;
  }
  
  else if (widget_name == "trackyear") {
    d_read_trackyear_width = width;
  }
}

void Dingo::Settings::setHeight(Glib::ustring widget_name, int height) {
  if (widget_name == "topwindow") {
    d_read_topwindow_height = height;
  }
  
  else if (widget_name == "topvpaned") {
    d_read_topvpaned_height = height;
  }
}

//UTILITIES
void Dingo::Settings::assignDataDefaultValues() {
  d_read_repeat = Dingo::REPEAT_OFF;
  d_read_playorder = Dingo::PLAYORDER_NORMAL;
  d_read_volume = 0.75;
  d_read_sort_column_id = 3;
  d_read_sort_type = Gtk::SORT_ASCENDING;
  d_read_subtitle_show = true;
  d_read_font_desc = "";
  
  d_read_playlistbrowser_visible = true;
  d_read_trackbrowser_visible = true;
  d_read_genrefilter_visible = true;
  d_read_artistfilter_visible = true;
  d_read_yearfilter_visible = true;
  d_read_albumfilter_visible = true;
  d_read_playbackstatus_visible = true;
  d_read_tracknumber_visible = true;
  d_read_tracktitle_visible = true;
  d_read_trackgenre_visible = true;
  d_read_trackartist_visible = true;
  d_read_trackalbum_visible = true;
  d_read_trackyear_visible = true;
  d_read_tracktime_visible = true;
  d_read_trackfileextension_visible = true;
  d_read_trackbitrate_visible = true;
  
  d_read_home_active = true;
  d_read_nowplaying_active = false;
  d_read_playqueue_active = false;
  d_read_audioonly_active = false;
  d_read_videoonly_active = false;
  
  d_read_topwindow_width = 1219;
  d_read_playlisthpaned_width = 256;
  d_read_genrehpaned_width = 323;
  d_read_artisthpaned_width = 349;
  d_read_yearhpaned_width = 175;
  d_read_tracktitle_width = 250;
  d_read_trackgenre_width = 125;
  d_read_trackartist_width = 125;
  d_read_trackalbum_width = 125;
  d_read_trackyear_width = 125;
  
  d_read_topwindow_height = 635;
  d_read_topvpaned_height = 220;
}
