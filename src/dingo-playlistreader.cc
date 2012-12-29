/* 
 * dingo-playlistreader.cc
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

#include "dingo-playlistreader.h"
#include "dingo-utilities.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

//CONSTRUCTORS - DESTRUCTOR
Dingo::PlaylistReader::PlaylistReader() {

}

Dingo::PlaylistReader::~PlaylistReader() {

}
      
//PUBLIC METHODS - READ
bool Dingo::PlaylistReader::open(Glib::ustring input_fileuri) {
  d_textreader.reset(new xmlpp::TextReader(input_fileuri));
  
  //read <dingo> & <playlist> tags
  while (d_textreader->read()) {
    if (d_textreader->get_name() == "dingo") {
      if (d_textreader->has_attributes()) {
        d_textreader->move_to_first_attribute();
        
        do {
          if (d_textreader->get_name() == "version") {
            //std::cout << "Dingo Version: " << d_textreader->get_value() << std::endl;
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
    
    else if (d_textreader->get_name() == "playlist") {
      if (d_textreader->has_attributes()) {
        d_textreader->move_to_first_attribute();
        
        do {
          if (d_textreader->get_name() == "name") {
            d_read_playlistname = Dingo::Utilities::decodeToEntityString(d_textreader->get_value());
          }
          
          else if (d_textreader->get_name() == "type") {
            //std::cout << "Dingo Playlist Type: " << d_textreader->get_value() << std::endl;
          }
          
          else if (d_textreader->get_name() == "size") {
            d_read_playlistsize = Dingo::Utilities::stringToInt(d_textreader->get_value());
          }
          
          else {
            return false;
          }
          
        } while (d_textreader->move_to_next_attribute());
      }
    }
    
    else if (d_textreader->get_name() == "#text") {
      //std::cout << "#text found!" << std::endl;
      continue;
    }
    
    else {
      return false;
    }
  }
}

bool Dingo::PlaylistReader::moveToNextTrack() {
  resetData();
  
  bool return_value = d_textreader->read();

  if (return_value) {
    while (d_textreader->read()) {
      if (d_textreader->get_name() == "trackartist") {
        d_textreader->read();
      
        if (!d_got_trackartist) {
          d_read_trackartist = Dingo::Utilities::decodeToEntityString(d_textreader->get_value());
          d_got_trackartist = true;
        }
      }
    
      else if (d_textreader->get_name() == "trackalbum") {
        d_textreader->read();
      
        if (!d_got_trackalbum) {
          d_read_trackalbum = Dingo::Utilities::decodeToEntityString(d_textreader->get_value());
          d_got_trackalbum = true;
        }
      }
    
      else if (d_textreader->get_name() == "tracknumber") {
        d_textreader->read();
      
        if (!d_got_tracknumber) {
          d_read_tracknumber = Dingo::Utilities::stringToInt(d_textreader->get_value());
          d_got_tracknumber = true;
        }
      }
    
      else if (d_textreader->get_name() == "tracktitle") {
        d_textreader->read();
      
        if (!d_got_tracktitle) {
          d_read_tracktitle = Dingo::Utilities::decodeToEntityString(d_textreader->get_value());
          d_got_tracktitle = true;
        }
      }
    
      else if (d_textreader->get_name() == "trackgenre") {
        d_textreader->read();
      
        if (!d_got_trackgenre) {
          d_read_trackgenre = Dingo::Utilities::decodeToEntityString(d_textreader->get_value());
          d_got_trackgenre = true;
        }
      }
    
      else if (d_textreader->get_name() == "trackduration") {
        d_textreader->read();
      
        if (!d_got_trackduration) {
          d_read_trackduration = Dingo::Utilities::stringToSQLite3Int64(d_textreader->get_value());
          d_got_trackduration = true;
        }
      }
    
      else if (d_textreader->get_name() == "tracksize") {
        d_textreader->read();
      
        if (!d_got_tracksize) {
          d_read_tracksize = Dingo::Utilities::stringToSQLite3Int64(d_textreader->get_value());
          d_got_tracksize = true;
        }
      }
    
      else if (d_textreader->get_name() == "trackfilename") {
        d_textreader->read();
      
        if (!d_got_trackfilename) {
          d_read_trackfilename = Dingo::Utilities::decodeToEntityString(d_textreader->get_value());
          d_got_trackfilename = true;
        }
      }
    
      else if (d_textreader->get_name() == "trackfileextension") {
        d_textreader->read();
      
        if (!d_got_trackfileextension) {
          d_read_trackfileextension = d_textreader->get_value();
          d_got_trackfileextension = true;
        }
      }
    
      else if (d_textreader->get_name() == "trackuri") {
        d_textreader->read();
      
        if (!d_got_trackuri) {
          d_read_trackuri = Dingo::Utilities::decodeToEntityString(d_textreader->get_value());          
          d_got_trackuri = true;
        }
      }
      
      else if (d_textreader->get_name() == "tracksubtitleuri") {
        d_textreader->read();
        
        if (!d_got_tracksubtitleuri) {
          d_read_tracksubtitleuri = Dingo::Utilities::decodeToEntityString(d_textreader->get_value());
          d_got_tracksubtitleuri = true;
        }
      }
    
      else if (d_textreader->get_name() == "trackyear") {
        d_textreader->read();
      
        if (!d_got_trackyear) {
          d_read_trackyear = d_textreader->get_value();
          d_got_trackyear = true;
        }
      }
    
      else if (d_textreader->get_name() == "trackbitrate") {
        d_textreader->read();
      
        if (!d_got_trackbitrate) {
          d_read_trackbitrate = Dingo::Utilities::stringToInt(d_textreader->get_value());
          d_got_trackbitrate = true;
        }
      }
    
      else if (d_textreader->get_name() == "trackmimetype") {
        d_textreader->read();
        
        if (!d_got_trackmimetype) {
          d_read_trackmimetype = d_textreader->get_value();
          d_got_trackmimetype = true;
        }
      }
      
      else if (d_textreader->get_name() == "trackdateadded") {
        d_textreader->read();
      
        if (!d_got_trackdateadded) {
          d_read_trackdateadded = d_textreader->get_value();
          d_got_trackdateadded = true;
        }
      }
      
      else if (d_textreader->get_name() == "trackcomment") {
        d_textreader->read();
        
        if (!d_got_trackcomment) {
          d_read_trackcomment = Dingo::Utilities::decodeToEntityString(d_textreader->get_value());
          d_got_trackcomment = true;
        }
      }
      
      else if (d_textreader->get_name() == "tracklyrics") {
        d_textreader->read();
        
        if (!d_got_tracklyrics) {
          d_read_tracklyrics = Dingo::Utilities::decodeToEntityString(d_textreader->get_value());
          d_got_tracklyrics = true; 
        }
        
        break;
      }
    }
  }
  
  return return_value;
}

//PUBLIC ACCESSORS
Glib::ustring Dingo::PlaylistReader::getPlaylistName() {
  return d_read_playlistname;
}

int Dingo::PlaylistReader::getPlaylistSize() {
  return d_read_playlistsize;
}

Glib::ustring Dingo::PlaylistReader::getTrackArtist() {
  return d_read_trackartist;
}

Glib::ustring Dingo::PlaylistReader::getTrackAlbum() {
  return d_read_trackalbum;
}

int Dingo::PlaylistReader::getTrackNumber() {
  return d_read_tracknumber;
}

Glib::ustring Dingo::PlaylistReader::getTrackTitle() {
  return d_read_tracktitle;
}

Glib::ustring Dingo::PlaylistReader::getTrackGenre() {
  return d_read_trackgenre;
}

sqlite3_int64 Dingo::PlaylistReader::getTrackDuration() {
  return d_read_trackduration;
}

sqlite3_int64 Dingo::PlaylistReader::getTrackSize() {
  return d_read_tracksize;
}

Glib::ustring Dingo::PlaylistReader::getTrackFileName() {
  return d_read_trackfilename;
}

Glib::ustring Dingo::PlaylistReader::getTrackFileExtension() {
  return d_read_trackfileextension;
}

Glib::ustring Dingo::PlaylistReader::getTrackURI() {
  return d_read_trackuri;
}

Glib::ustring Dingo::PlaylistReader::getTrackSubtitleURI() {
  return d_read_tracksubtitleuri;
}

Glib::ustring Dingo::PlaylistReader::getTrackYear() {
  return d_read_trackyear;
}

int Dingo::PlaylistReader::getTrackBitrate() {
  return d_read_trackbitrate;
}

Glib::ustring Dingo::PlaylistReader::getTrackMIMEType() {
  return d_read_trackmimetype;
}

Glib::ustring Dingo::PlaylistReader::getTrackDateAdded() {
  return d_read_trackdateadded;
}

Glib::ustring Dingo::PlaylistReader::getTrackComment() {
  return d_read_trackcomment;
}

Glib::ustring Dingo::PlaylistReader::getTrackLyrics() {
  return d_read_tracklyrics;
}

//UTILITIES
void Dingo::PlaylistReader::resetData() {
  d_got_trackartist = false;
  d_got_trackalbum = false;
  d_got_tracknumber = false;
  d_got_tracktitle = false;
  d_got_trackgenre = false;
  d_got_trackduration = false;
  d_got_tracksize = false;
  d_got_trackfilename = false;
  d_got_trackfileextension = false;
  d_got_trackuri = false;
  d_got_trackyear = false;
  d_got_trackbitrate = false;
  d_got_trackmimetype = false;
  d_got_trackdateadded = false;
  d_got_trackcomment = false;
  d_got_tracklyrics = false;
  
  d_read_trackartist = "";
  d_read_trackalbum = "";
  d_read_tracknumber = 0;
  d_read_tracktitle = "";
  d_read_trackgenre = "";
  d_read_trackduration = 0;
  d_read_tracksize = 0;
  d_read_trackfilename = "";
  d_read_trackfileextension = "";
  d_read_trackuri = "";
  d_read_trackyear = "";
  d_read_trackbitrate = 0;
  d_read_trackmimetype = "";
  d_read_trackdateadded = "";
  d_read_trackcomment = "";
  d_read_tracklyrics = "";
}
