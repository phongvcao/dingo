/* 
 * dingo-settingswriter.cc
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

#include "dingo-settingswriter.h"
#include "dingo-utilities.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

#ifndef INCLUDED_FSTREAM
#define INCLUDED_FSTREAM
#include <fstream>
#endif //INCLUDED_FSTREAM

//CONSTRUCTORS - DESTRUCTOR
Dingo::SettingsWriter::SettingsWriter() {

}

Dingo::SettingsWriter::~SettingsWriter() {

}
      
//PUBLIC METHODS - WRITE
void Dingo::SettingsWriter::open(Glib::ustring output_fileuri, Glib::ustring settings_name) {
  d_output_fileuri = output_fileuri;
  
  d_whole_file_str = "<dingo version='0.5.x' author='Phong V. Cao' email='phongvcao@phongvcao.com' website='http://www.phongvcao.com/'>\n";
  
  d_whole_file_str += "  <settings name='" + settings_name + "' type='basic'>\n";
}
      
//PUBLIC CREATORS
void Dingo::SettingsWriter::createSettings() {
  //reset old data
  d_write_repeat = "";
  d_write_playorder = "";
  d_write_volume = "";
  d_write_sort = "";
  d_write_subtitle = "";
  d_write_visible = "";
  d_write_active = "";
  d_write_width_paneds_wins = "";
  d_write_width_columns = "";
  d_write_height = "";
}
      
//PUBLIC ACCESSORS
void Dingo::SettingsWriter::setRepeat(Dingo::RepeatMode repeat_mode) {
  switch (repeat_mode) {
    case Dingo::REPEAT_OFF: {
      d_write_repeat = "    <repeat>off</repeat>\n";
      
      break;
    }
    
    case Dingo::REPEAT_SINGLE: {
      d_write_repeat = "    <repeat>single</repeat>\n";
      
      break;
    }
    
    case Dingo::REPEAT_PLAYLIST: {
      d_write_repeat = "    <repeat>playlist</repeat>\n";
      
      break;
    }
    
    default: {
      d_write_repeat = "    <repeat>off</repeat>\n";
      
      break;
    }
  }
}

void Dingo::SettingsWriter::setPlayOrder(Dingo::PlayOrder play_order) {
  switch (play_order) {
    case Dingo::PLAYORDER_NORMAL: {
      d_write_playorder = "    <playorder>normal</playorder>\n";
      
      break;
    }
    
    case Dingo::PLAYORDER_SHUFFLE: {
      d_write_playorder = "    <playorder>shuffle</playorder>\n";
      
      break;
    }
    
    default: {
      break;
    }
  }
}

void Dingo::SettingsWriter::setVolume(double volume) {
  d_write_volume = "    <volume>" + Dingo::Utilities::doubleToString(volume) + "</volume>\n";
}

void Dingo::SettingsWriter::setSortColumnID(int column_id) {
  d_write_sort = "    <sort>\n";

  switch (column_id) {
    case 3: {
      d_write_sort += "      <columnname>tracktitle</columnname>\n";
    
      break;
    }
    
    case 4: {
      d_write_sort += "      <columnname>trackgenre</columnname>\n";
    
      break;
    }
    
    case 5: {
      d_write_sort += "      <columnname>trackartist</columnname>\n";
    
      break;
    }
    
    case 6: {
      d_write_sort += "      <columnname>trackalbum</columnname>\n";
    
      break;
    }
    
    case 7: {
      d_write_sort += "      <columnname>trackyear</columnname>\n";
    
      break;
    }
    
    case 9: {
      d_write_sort += "      <columnname>trackfileextension</columnname>\n";
    
      break;
    }
    
    case 10: {
      d_write_sort += "      <columnname>trackbitrate</columnname>\n";
    
      break;
    }
    
    default: {
      d_write_sort += "      <columnname>tracktitle</columnname>\n";
    
      break;
    }
  }
}

void Dingo::SettingsWriter::setSortType(Gtk::SortType sort_type) {
  switch (sort_type) {
    case Gtk::SORT_ASCENDING: {
      d_write_sort += "      <type>ascending</type>\n";
      
      break;
    }
    
    case Gtk::SORT_DESCENDING: {
      d_write_sort += "      <type>descending</type>\n";
      
      break;
    }
    
    default: {
      d_write_sort += "      <type>ascending</type>\n";
      
      break;
    }
  }
  
  d_write_sort += "    </sort>\n";
}

void Dingo::SettingsWriter::setSubtitleShow(bool is_show) {
  d_write_subtitle = "    <subtitle>\n";
  
  d_write_subtitle += "      <show>" + Dingo::Utilities::boolToString(is_show) + "</show>\n";
}

void Dingo::SettingsWriter::setSubtitleFontDesc(Glib::ustring font_desc) {
  d_write_subtitle += "      <font>" + font_desc + "</font>\n";
  
  d_write_subtitle += "    </subtitle>\n";
}

void Dingo::SettingsWriter::setVisible(Glib::ustring widget_name, bool is_visible) {
  if (widget_name == "playlistbrowser") {
    d_write_visible = "    <visible>\n";
    d_write_visible += "      <playlistbrowser>" + Dingo::Utilities::boolToString(is_visible) + "</playlistbrowser>\n";
  }
  
  else if (widget_name == "trackbitrate") {
    d_write_visible += "      <trackbitrate>" + Dingo::Utilities::boolToString(is_visible) + "</trackbitrate>\n";
    d_write_visible += "    </visible>\n";
  }
  
  else {
    d_write_visible += "      <" + widget_name + ">" + Dingo::Utilities::boolToString(is_visible) + "</" + widget_name + ">\n";
  }
}

void Dingo::SettingsWriter::setActive(Glib::ustring toggle_name, bool is_active) {
  if (toggle_name == "home") {
    d_write_active = "    <active>\n";
    d_write_active += "      <home>" + Dingo::Utilities::boolToString(is_active) + "</home>\n";
  }
  
  else if (toggle_name == "videoonly") {
    d_write_active += "      <videoonly>" + Dingo::Utilities::boolToString(is_active) + "</videoonly>\n";
    d_write_active += "    </active>\n";
  }
  
  else {
    d_write_active += "      <" + toggle_name + ">" + Dingo::Utilities::boolToString(is_active) + "</" + toggle_name + ">\n";
  }
}

void Dingo::SettingsWriter::setWidth(Glib::ustring widget_name, int width) {
  if (widget_name == "topwindow") {
    d_write_width_paneds_wins = "    <width>\n";
    d_write_width_paneds_wins += "      <topwindow>" + Dingo::Utilities::intToString(width) + "</topwindow>\n";
  }
  
  else if (widget_name == "trackyear") {
    d_write_width_columns += "      <trackyear>" + Dingo::Utilities::intToString(width) + "</trackyear>\n";
    d_write_width_columns += "    </width>\n";
  }
  
  else {
    Glib::ustring::size_type paned_pos = widget_name.find("paned");
    
    if (paned_pos != Glib::ustring::npos) {
      d_write_width_paneds_wins += "      <" + widget_name + ">" + Dingo::Utilities::intToString(width) + "</" + widget_name + ">\n";
    }
    
    else {
      d_write_width_columns += "      <" + widget_name + ">" + Dingo::Utilities::intToString(width) + "</" + widget_name + ">\n";
    }
  }
}

void Dingo::SettingsWriter::setHeight(Glib::ustring widget_name, int height) {
  if (widget_name == "topwindow") {
    d_write_height = "    <height>\n";
    d_write_height += "      <topwindow>" + Dingo::Utilities::intToString(height) + "</topwindow>\n";
  }
  
  else if (widget_name == "topvpaned") {
    d_write_height += "      <topvpaned>" + Dingo::Utilities::intToString(height) + "</topvpaned>\n";
    d_write_height += "    </height>\n";
  }
  
  else {
    d_write_height += "      <" + widget_name + ">" + Dingo::Utilities::intToString(height) + "</" + widget_name + ">\n";
  }
}
      
//PUBLIC SAVER/WRITER
void Dingo::SettingsWriter::saveSettings() {
  //concatenate settings      
  d_whole_file_str += d_write_repeat + d_write_playorder + d_write_volume + d_write_sort + d_write_subtitle + d_write_visible + d_write_active + d_write_width_paneds_wins + d_write_width_columns + d_write_height;
}

void Dingo::SettingsWriter::writeToFile() {
  d_whole_file_str += "  </settings>\n";
  d_whole_file_str += "</dingo>";
  
  Glib::ustring d_output_filepath = Glib::filename_from_uri(d_output_fileuri);
  
  std::ofstream outfile(d_output_filepath.c_str());
  
  if (outfile) {
    outfile << d_whole_file_str;
  }
  
  outfile.close();
}
