/* 
 * dingo-playlistreader.h
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

#ifndef INCLUDED_DINGO_PLAYLISTREADER_H
#define INCLUDED_DINGO_PLAYLISTREADER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_LIBXMLPP_H
#define INCLUDED_LIBXMLPP_H
#include <libxml++/libxml++.h>
#endif //INCLUDED_LIBXMLPP_H

#ifndef INCLUDED_SQLITE3_H
#define INCLUDED_SQLITE3_H
#include <sqlite3.h>
#endif //INCLUDED_SQLITE3_H

namespace Dingo {
  class PlaylistReader {
    public:
      PlaylistReader();
      virtual ~PlaylistReader();
      
      //PUBLIC METHODS - READ
      bool open(Glib::ustring input_fileuri);
      
      bool moveToNextTrack();
      
      //PUBLIC ACCESSORS
      Glib::ustring getPlaylistName();
      int getPlaylistSize();
      
      Glib::ustring getTrackArtist();
      Glib::ustring getTrackAlbum();
      int getTrackNumber();
      Glib::ustring getTrackTitle();
      Glib::ustring getTrackGenre();
      sqlite3_int64 getTrackDuration();
      sqlite3_int64 getTrackSize();
      Glib::ustring getTrackFileName();
      Glib::ustring getTrackFileExtension();
      Glib::ustring getTrackURI();
      Glib::ustring getTrackSubtitleURI();
      Glib::ustring getTrackYear();
      int getTrackBitrate();
      Glib::ustring getTrackMIMEType();
      Glib::ustring getTrackDateAdded();
      Glib::ustring getTrackComment();
      Glib::ustring getTrackLyrics();
    
    private:
      //UTILITIES
      void resetData();
    
      //DATA MEMBERS
      std::auto_ptr<xmlpp::TextReader> d_textreader;
      
      //DATA MEMBERS - READ
      Glib::ustring d_read_trackartist;
      Glib::ustring d_read_trackalbum;
      int d_read_tracknumber;
      Glib::ustring d_read_tracktitle;
      Glib::ustring d_read_trackgenre;
      sqlite3_int64 d_read_trackduration;
      sqlite3_int64 d_read_tracksize;
      Glib::ustring d_read_trackfilename;
      Glib::ustring d_read_trackfileextension;
      Glib::ustring d_read_trackuri;
      Glib::ustring d_read_tracksubtitleuri;
      Glib::ustring d_read_trackyear;
      int d_read_trackbitrate;
      Glib::ustring d_read_trackmimetype;
      Glib::ustring d_read_trackdateadded;
      Glib::ustring d_read_trackcomment;
      Glib::ustring d_read_tracklyrics;
      
      Glib::ustring d_read_playlistname;
      int d_read_playlistsize;
      
      bool d_got_trackartist;
      bool d_got_trackalbum;
      bool d_got_tracknumber;
      bool d_got_tracktitle;
      bool d_got_trackgenre;
      bool d_got_trackduration;
      bool d_got_tracksize;
      bool d_got_trackfilename;
      bool d_got_trackfileextension;
      bool d_got_trackuri;
      bool d_got_tracksubtitleuri;
      bool d_got_trackyear;
      bool d_got_trackbitrate;
      bool d_got_trackmimetype;
      bool d_got_trackdateadded;
      bool d_got_trackcomment;
      bool d_got_tracklyrics;
  };
}

#endif //INCLUDED_DINGO_PLAYLISTREADER_H
