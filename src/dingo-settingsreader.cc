/* 
 * dingo-settingsreader.cc
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

#include "dingo-settingsreader.h"
#include "dingo-utilities.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

//CONSTRUCTORS - DESTRUCTOR
Dingo::SettingsReader::SettingsReader() {
  /* CHECK EMPTINESS */
  //<repeat> values
  d_got_repeat = false;
      
  //<playorder> values
  d_got_playorder = false;
      
  //<volume> values
  d_got_volume = false;
      
  //<sort> values
  d_got_sort_column_id = false;
  d_got_sort_type = false;
      
  //<subtitle> values
  d_got_subtitle_show = false;
  d_got_font_desc = false;
      
  //<visible> values
  d_got_playlistbrowser_visible = false;
  d_got_trackbrowser_visible = false;
  d_got_playbackstatus_visible = false;
  d_got_tracknumber_visible = false;
  d_got_tracktitle_visible = false;
  d_got_trackgenre_visible = false;
  d_got_trackartist_visible = false;
  d_got_trackalbum_visible = false;
  d_got_trackyear_visible = false;
  d_got_tracktime_visible = false;
  d_got_trackfileextension_visible = false;
  d_got_trackbitrate_visible = false;
  d_got_genrefilter_visible = false;
  d_got_artistfilter_visible = false;
  d_got_yearfilter_visible = false;
  d_got_albumfilter_visible = false;
      
  //<active> values
  d_got_home_active = false;
  d_got_nowplaying_active = false;
  d_got_playqueue_active = false;
  d_got_audioonly_active = false;
  d_got_videoonly_active = false;
      
  //<width> values
  d_got_topwindow_width = false;
  d_got_playlisthpaned_width = false;
  d_got_genrehpaned_width = false;
  d_got_artisthpaned_width = false;
  d_got_yearhpaned_width = false;
  d_got_tracktitle_width = false;
  d_got_trackgenre_width = false;
  d_got_trackartist_width = false;
  d_got_trackalbum_width = false;
  d_got_trackyear_width = false;
      
  //<height> values
  d_got_topwindow_height = false;
  d_got_topvpaned_height = false;
}

Dingo::SettingsReader::~SettingsReader() {

}
      
//PUBLIC METHODS - READ
bool Dingo::SettingsReader::open(Glib::ustring input_fileuri) {
  d_textreader.reset(new xmlpp::TextReader(input_fileuri));
  
  //read <dingo> & <playlist> tags
  while (d_textreader->read()) {
    if (d_textreader->get_name() == "dingo") {
      if (d_textreader->has_attributes()) {
        d_textreader->move_to_first_attribute();
        
        do {
          if (d_textreader->get_name() == "version") {
            if (d_textreader->get_value() != "0.5.x") {
              return false;
            }
          }
          
          else if (d_textreader->get_name() == "author") {
            if (d_textreader->get_value() != "Phong V. Cao") {
              return false;
            }
          }
          
          else if (d_textreader->get_name() == "email") {
            if (d_textreader->get_value() != "phongvcao@phongvcao.com") {
              return false;
            }
          }
          
          else if (d_textreader->get_name() == "website") {
            if (d_textreader->get_value() != "http://www.phongvcao.com/") {
              return false;
            }
          }
          
          else {
            return false;
          }
          
        } while (d_textreader->move_to_next_attribute());
      }
    }
    
    else if (d_textreader->get_name() == "settings") {
      if (d_textreader->has_attributes()) {
        d_textreader->move_to_first_attribute();
        
        do {
          if (d_textreader->get_name() == "name") {
            
          }
          
          else if (d_textreader->get_name() == "type") {
            
          }
          
          else {
            return false;
          }
          
        } while (d_textreader->move_to_next_attribute());
      }
    }
    
    else if (d_textreader->get_name() == "repeat") {
      d_textreader->read();
      
      if (!d_got_repeat) {
        Glib::ustring temp_repeat = d_textreader->get_value();
      
        if (temp_repeat == "off") {
          d_read_repeat = Dingo::REPEAT_OFF;
        }
        
        else if (temp_repeat == "playlist") {
          d_read_repeat = Dingo::REPEAT_PLAYLIST;
        }
        
        else if (temp_repeat == "single") {
          d_read_repeat = Dingo::REPEAT_SINGLE;
        }
        
        d_got_repeat = true;
      }
    }
    
    else if (d_textreader->get_name() == "playorder") {
      d_textreader->read();
      
      if (!d_got_playorder) {
        Glib::ustring temp_playorder = d_textreader->get_value();
        
        if (temp_playorder == "normal") {
          d_read_playorder = Dingo::PLAYORDER_NORMAL;
        }
        
        else if (temp_playorder == "shuffle") {
          d_read_playorder = Dingo::PLAYORDER_SHUFFLE;
        }
        
        d_got_playorder = true;
      }
    }
    
    else if (d_textreader->get_name() == "volume") {
      d_textreader->read();
      
      if (!d_got_volume) {
        d_read_volume = Dingo::Utilities::stringToDouble(d_textreader->get_value());
        
        d_got_volume = true;
      }
    }
    
    else if (d_textreader->get_name() == "sort") {
      while (d_textreader->read()) {
        if (d_textreader->get_name() == "columnname") {
          d_textreader->read();
        
          if (!d_got_sort_column_id) {
            Glib::ustring temp_columnname = d_textreader->get_value();
        
            if (temp_columnname == "tracktitle") {
              d_read_sort_column_id = 3;
            }
        
            else if (temp_columnname == "trackgenre") {
              d_read_sort_column_id = 4;
            }
        
            else if (temp_columnname == "trackartist") {
              d_read_sort_column_id = 5;        
            }
        
            else if (temp_columnname == "trackalbum") {
              d_read_sort_column_id = 6;
            }
        
            else if (temp_columnname == "trackyear") {
              d_read_sort_column_id = 7;
            }
        
            else if (temp_columnname == "trackfileextension") {
              d_read_sort_column_id = 9;
            }
        
            else if (temp_columnname == "trackbitrate") {
              d_read_sort_column_id = 10;
            }
        
            d_got_sort_column_id = true;
          }
        }
      
        else if (d_textreader->get_name() == "type") {
          d_textreader->read();
        
          if (!d_got_sort_type) {
            Glib::ustring temp_type = d_textreader->get_value();
        
            if (temp_type == "ascending") {
              d_read_sort_type = Gtk::SORT_ASCENDING;
            }
        
            else if (temp_type == "descending") {
              d_read_sort_type = Gtk::SORT_DESCENDING;
            }
        
            d_got_sort_type = true;
          }
        }
        
        else if (d_textreader->get_name() == "#text") {
          continue;
        }
        
        else {
          break;
        }
      }
    }
    
    else if (d_textreader->get_name() == "subtitle") {      
      while (d_textreader->read()) {
        if (d_textreader->get_name() == "show") {
          d_textreader->read();
        
          if (!d_got_subtitle_show) {
            d_read_subtitle_show = Dingo::Utilities::stringToBool(d_textreader->get_value());
          
            d_got_subtitle_show = true;
          }
        }
        
        else if (d_textreader->get_name() == "font") {
          d_textreader->read();
        
          if (!d_got_font_desc) {
            d_read_font_desc = d_textreader->get_value();
          
            d_got_font_desc = true;
          }
        }
        
        else if (d_textreader->get_name() == "#text") {
          continue;
        }
        
        else {
          break;
        }
      }
    }
    
    else if (d_textreader->get_name() == "visible") {
      while (d_textreader->read()) {
        Glib::ustring tag_name = d_textreader->get_name();
      
        if (tag_name == "playlistbrowser") {
          d_textreader->read();
          
          if (!d_got_playlistbrowser_visible) {
            d_read_playlistbrowser_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_playlistbrowser_visible = true;
          }
        }
        
        else if (tag_name == "trackbrowser") {
          d_textreader->read();
          
          if (!d_got_trackbrowser_visible) {
            d_read_trackbrowser_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_trackbrowser_visible = true;
          }
        }
        
        else if (tag_name == "playbackstatus") {
          d_textreader->read();
          
          if (!d_got_playbackstatus_visible) {
            d_read_playbackstatus_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_playbackstatus_visible = true;
          }
        }
        
        else if (tag_name == "tracknumber") {
          d_textreader->read();
          
          if (!d_got_tracknumber_visible) {
            d_read_tracknumber_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_tracknumber_visible = true;
          }
        }
        
        else if (tag_name == "tracktitle") {
          d_textreader->read();
          
          if (!d_got_tracktitle_visible) {
            d_read_tracktitle_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_tracktitle_visible = true;
          }
        }
        
        else if (tag_name == "trackgenre") {
          d_textreader->read();
          
          if (!d_got_trackgenre_visible) {
            d_read_trackgenre_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_trackgenre_visible = true;
          }
        }
        
        else if (tag_name == "trackartist") {
          d_textreader->read();
          
          if (!d_got_trackartist_visible) {
            d_read_trackartist_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_trackartist_visible = true;
          }
        }
        
        else if (tag_name == "trackalbum") {
          d_textreader->read();
          
          if (!d_got_trackalbum_visible) {
            d_read_trackalbum_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_trackalbum_visible = true;
          }
        }
        
        else if (tag_name == "trackyear") {
          d_textreader->read();
          
          if (!d_got_trackyear_visible) {
            d_read_trackyear_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_trackyear_visible = true;
          }
        }
        
        else if (tag_name == "tracktime") {
          d_textreader->read();
          
          if (!d_got_tracktime_visible) {
            d_read_tracktime_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_tracktime_visible = true;
          }
        }
        
        else if (tag_name == "trackfileextension") {
          d_textreader->read();
          
          if (!d_got_trackfileextension_visible) {
            d_read_trackfileextension_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_trackfileextension_visible = true;
          }
        }
        
        else if (tag_name == "trackbitrate") {
          d_textreader->read();
          
          if (!d_got_trackbitrate_visible) {
            d_read_trackbitrate_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_trackbitrate_visible = true;
          }
        }
        
        else if (tag_name == "genrefilter") {
          d_textreader->read();
          
          if (!d_got_genrefilter_visible) {
            d_read_genrefilter_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_genrefilter_visible = true;
          }
        }
        
        else if (tag_name == "artistfilter") {
          d_textreader->read();
          
          if (!d_got_artistfilter_visible) {
            d_read_artistfilter_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_artistfilter_visible = true;
          }
        }
        
        else if (tag_name == "yearfilter") {
          d_textreader->read();
          
          if (!d_got_yearfilter_visible) {
            d_read_yearfilter_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_yearfilter_visible = true;
          }
        }
        
        else if (tag_name == "albumfilter") {
          d_textreader->read();
          
          if (!d_got_albumfilter_visible) {
            d_read_albumfilter_visible = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_albumfilter_visible = true;
          }
        }
        
        else if (tag_name == "#text") {
          continue;
        }
        
        else {
          break;
        }
      }
    }
    
    else if (d_textreader->get_name() == "active") {
      while (d_textreader->read()) {
        Glib::ustring tag_name = d_textreader->get_name();
        
        if (tag_name == "home") {
          d_textreader->read();
        
          if (!d_got_home_active) {
            d_read_home_active = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_home_active = true;
          }
        }
        
        else if (tag_name == "nowplaying") {
          d_textreader->read();
        
          if (!d_got_nowplaying_active) {
            d_read_nowplaying_active = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_nowplaying_active = true;
          }
        }
        
        else if (tag_name == "playqueue") {
          d_textreader->read();
        
          if (!d_got_playqueue_active) {
            d_read_playqueue_active = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_playqueue_active = true;
          }
        }
        
        else if (tag_name == "audioonly") {
          d_textreader->read();
        
          if (!d_got_audioonly_active) {
            d_read_audioonly_active = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_audioonly_active = true;
          }
        }
        
        else if (tag_name == "videoonly") {
          d_textreader->read();
        
          if (!d_got_videoonly_active) {
            d_read_videoonly_active = Dingo::Utilities::stringToBool(d_textreader->get_value());
            
            d_got_videoonly_active = true;
          }
        }
        
        else if (tag_name == "#text") {
          continue;
        }
        
        else {
          break;
        }
      }
    }
    
    else if (d_textreader->get_name() == "width") {
      while (d_textreader->read()) {
        Glib::ustring tag_name = d_textreader->get_name();
        
        if (tag_name == "topwindow") {
          d_textreader->read();
          
          if (!d_got_topwindow_width) {
            d_read_topwindow_width = Dingo::Utilities::stringToInt(d_textreader->get_value());
            
            d_got_topwindow_width = true;
          }
        }
        
        else if (tag_name == "playlisthpaned") {
          d_textreader->read();
          
          if (!d_got_playlisthpaned_width) {
            d_read_playlisthpaned_width = Dingo::Utilities::stringToInt(d_textreader->get_value());
            
            d_got_playlisthpaned_width = true;
          }
        }
        
        else if (tag_name == "genrehpaned") {
          d_textreader->read();
          
          if (!d_got_genrehpaned_width) {
            d_read_genrehpaned_width = Dingo::Utilities::stringToInt(d_textreader->get_value());
            
            d_got_genrehpaned_width = true;
          }
        }
        
        else if (tag_name == "artisthpaned") {
          d_textreader->read();
          
          if (!d_got_artisthpaned_width) {
            d_read_artisthpaned_width = Dingo::Utilities::stringToInt(d_textreader->get_value());
            
            d_got_artisthpaned_width = true;
          }
        }
        
        else if (tag_name == "yearhpaned") {
          d_textreader->read();
          
          if (!d_got_yearhpaned_width) {
            d_read_yearhpaned_width = Dingo::Utilities::stringToInt(d_textreader->get_value());
            
            d_got_yearhpaned_width = true;
          }
        }
        
        else if (tag_name == "tracktitle") {
          d_textreader->read();
          
          if (!d_got_tracktitle_width) {
            d_read_tracktitle_width = Dingo::Utilities::stringToInt(d_textreader->get_value());
            
            d_got_tracktitle_width = true;
          }
        }
        
        else if (tag_name == "trackgenre") {
          d_textreader->read();
          
          if (!d_got_trackgenre_width) {
            d_read_trackgenre_width = Dingo::Utilities::stringToInt(d_textreader->get_value());
            
            d_got_trackgenre_width = true;
          }
        }
        
        else if (tag_name == "trackartist") {
          d_textreader->read();
          
          if (!d_got_trackartist_width) {
            d_read_trackartist_width = Dingo::Utilities::stringToInt(d_textreader->get_value());
            
            d_got_trackartist_width = true;
          }
        }
        
        else if (tag_name == "trackalbum") {
          d_textreader->read();
          
          if (!d_got_trackalbum_width) {
            d_read_trackalbum_width = Dingo::Utilities::stringToInt(d_textreader->get_value());
            
            d_got_trackalbum_width = true;
          }
        }
        
        else if (tag_name == "trackyear") {
          d_textreader->read();
        
          if (!d_got_trackyear_width) {
            d_read_trackyear_width = Dingo::Utilities::stringToInt(d_textreader->get_value());
            
            d_got_trackyear_width = true;
          }
        }
        
        else if (tag_name == "#text") {
          continue;
        }
        
        else {
          break;
        }
      }
    }
    
    else if (d_textreader->get_name() == "height") {
      d_textreader->read();
      
      while (d_textreader->read()) {
        Glib::ustring tag_name = d_textreader->get_name();
        
        if (tag_name == "topwindow") {
          d_textreader->read();
          
          if (!d_got_topwindow_height) {
            d_read_topwindow_height = Dingo::Utilities::stringToInt(d_textreader->get_value());
            
            d_got_topwindow_height = true;
          }
        }
        
        else if (tag_name == "topvpaned") {
          d_textreader->read();
        
          if (!d_got_topvpaned_height) {
            d_read_topvpaned_height = Dingo::Utilities::stringToInt(d_textreader->get_value());
            
            d_got_topvpaned_height = true;
          }
          
          break;
        }
      }
    }
    
    /* else {
      std::cout << "Missed tag: " << d_textreader->get_name() << std::endl;
      std::cout << "Value: " << d_textreader->get_value() << std::endl;
    } */
  }
  
  return true;
}
      
//PUBLIC ACCESSORS
Dingo::RepeatMode Dingo::SettingsReader::getRepeat() {
  //<repeat> values
  return d_read_repeat;
}

Dingo::PlayOrder Dingo::SettingsReader::getPlayOrder() {
  //<playorder> values
  return d_read_playorder;

}

double Dingo::SettingsReader::getVolume() {
  //<volume> values
  return d_read_volume;

}

int Dingo::SettingsReader::getSortColumnID() {
  //<sort> values
  return d_read_sort_column_id;
}

Gtk::SortType Dingo::SettingsReader::getSortType() {
  //<sort> values
  return d_read_sort_type;
}

bool Dingo::SettingsReader::getSubtitleShow() {
  //<subtitle> values
  return d_read_subtitle_show;
}

Glib::ustring Dingo::SettingsReader::getSubtitleFontDesc() {
  //<subtitle> values
  return d_read_font_desc;
}

bool Dingo::SettingsReader::getVisible(Glib::ustring widget_name) {
  //<visible> values
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

bool Dingo::SettingsReader::getActive(Glib::ustring toggle_name) {
  //<active> values
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

int Dingo::SettingsReader::getWidth(Glib::ustring widget_name) {
  //<width> values
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

int Dingo::SettingsReader::getHeight(Glib::ustring widget_name) {
  //<height> values
  if (widget_name == "topwindow") {
    return d_read_topwindow_height;
  }
  
  else if (widget_name == "topvpaned") {
    return d_read_topvpaned_height;
  }
}
