/* 
 * dingo-header.h
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

#ifndef INCLUDED_DINGO_HEADER_H
#define INCLUDED_DINGO_HEADER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  enum SettingsSignals {
    SETTINGS_READ = 0,
    SETTINGS_WRITE
  };

  enum DBManSignals {
    CHANGES_IN_CORE_PLAYLISTS = 0,
    CHANGES_IN_CORE_GENRES,
    CHANGES_IN_CORE_ARTISTS,
    CHANGES_IN_CORE_YEARS,
    CHANGES_IN_CORE_ALBUMS,
    CHANGES_IN_CORE_TRACKS
  };
  
  enum ClipboardTracksAction {
    CLIPBOARD_CUT_TRACKS = 0,
    CLIPBOARD_COPY_TRACKS,
    CLIPBOARD_PASTE_TRACKS
  };
  
  enum ProcessSignals {
    PROCESS_FINISHED = 0
  };
  
  enum ProcessID {
    PROCESS_NONE = -1,
    PROCESS_IMPORTING_FILES = 0,
    PROCESS_IMPORTING_FOLDERS = 1,
    PROCESS_IMPORTING_PLAYLISTS = 2,
    PROCESS_EXPORTING_PLAYLIST = 3,
    PROCESS_CUTTING_TRACKS = 4,
    PROCESS_COPYING_TRACKS = 5,
    PROCESS_PASTING_TRACKS = 6,
    PROCESS_ADDING_TO_PLAYQUEUE = 7,
    PROCESS_ADDING_TO_PLAYLISTS = 8,
    PROCESS_DELETING_FROM_DATABASE = 9,
    PROCESS_DELETING_FROM_PLAYQUEUE = 10,
    PROCESS_DELETING_FROM_HARDDRIVE = 11,
    PROCESS_DELETING_FROM_PLAYLISTS = 12,
    PROCESS_DELETING_FROM_CURRENT_PLAYLIST = 13,
    PROCESS_EDITING_SINGLE_TRACK = 14,
    PROCESS_EDITING_MULTIPLE_TRACKS = 15,
    PROCESS_SETTINGS_LOADING_FROM_FILE = 16, //to be implemented...
    PROCESS_SETTINGS_SAVING_TO_FILE = 17, //to be implemented...
    PROCESS_SETTINGS_SAVING_CURRENT_AS_DEFAULT = 18, //to be implemented...
    PROCESS_SETTINGS_RESTORING_TO_DEFAULT = 19, //to be implemented...
  };
  
  enum ToolBarSignals {
    HOME_TOGGLED = 0,
    NOW_PLAYING_TOGGLED,
    PLAY_QUEUE_TOGGLED,
    AUDIO_FILTER_TOGGLED,
    VIDEO_FILTER_TOGGLED
  };
  
  enum SearchEntrySignals {
    SEARCH_ENTRY_CHANGED = 0
  };
  
  enum EngineStatus {
    ENGINE_PLAYING = 0,
    ENGINE_PAUSED,
    ENGINE_STOPPED,
    ENGINE_NEW_PLAYING
  };
  
  enum EngineNavigationSignals {
    ENGINE_TRACK_NEXT = 0,
    ENGINE_TRACK_PREVIOUS,
    ENGINE_TRACK_CURRENT
  };
  
  enum FullscreenSignals {
    FULLSCREEN_ACTIVATED = 0,
    UNFULLSCREEN_ACTIVATED
  };
  
  enum VideoAreaSignals {
    MOUSE_HOVER_ON_FULLSCREEN = 0
  };
  
  enum RepeatMode {
    REPEAT_OFF = 0,
    REPEAT_SINGLE,
    REPEAT_PLAYLIST,
    REPEAT_SECTION
  };
  
  enum PlayOrder {
    PLAYORDER_NORMAL = 0,
    PLAYORDER_SHUFFLE
  };
  
  enum TreeViewSignals {
    PLAYLIST_SELECTED = 0,
    GENRES_SELECTED,
    ARTISTS_SELECTED,
    YEARS_SELECTED,
    ALBUMS_SELECTED,
    TRACK_TREE_ROW_ACTIVATED,
    TRACK_TREE_ROW_DEACTIVATED
  };
  
  enum TrackPlaylistEditWindowSignals {
    TRACKPLAYLISTEDIT_INFO_SAVED = 0
  };
  
  enum TrackInfoWindowSignals {
    PREVIOUS_TRACK_INFO = 0,
    NEXT_TRACK_INFO
  };
  
  enum MultipleTracksInfoWindowSignals {
    SAVE_CLICKED = 0
  };
    
  class TrackColumnRecord : public Gtk::TreeModel::ColumnRecord {
    public:
      TrackColumnRecord() { add(trackStatus); add(trackID); add(trackNumber); add(trackTitle); add(trackGenre); add(trackArtist); add(trackAlbum); add(trackYear); add(trackTime); add(trackFileExtension); add(trackBitrate); add(trackMIMEType); add(trackURI); add(trackSubtitleURI); add(trackVisible); }
        
      Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > trackStatus;
      Gtk::TreeModelColumn<int> trackID;
      Gtk::TreeModelColumn<Glib::ustring> trackNumber;
      Gtk::TreeModelColumn<Glib::ustring> trackTitle;
      Gtk::TreeModelColumn<Glib::ustring> trackGenre;
      Gtk::TreeModelColumn<Glib::ustring> trackArtist;
      Gtk::TreeModelColumn<Glib::ustring> trackAlbum;
      Gtk::TreeModelColumn<Glib::ustring> trackYear;
      Gtk::TreeModelColumn<Glib::ustring> trackTime;
      Gtk::TreeModelColumn<Glib::ustring> trackFileExtension;
      Gtk::TreeModelColumn<Glib::ustring> trackBitrate;
      Gtk::TreeModelColumn<Glib::ustring> trackMIMEType;
      Gtk::TreeModelColumn<Glib::ustring> trackURI;
      Gtk::TreeModelColumn<Glib::ustring> trackSubtitleURI;
      Gtk::TreeModelColumn<bool> trackVisible;
  };
  
  class TrackPlaylistColumnRecord : public Gtk::TreeModel::ColumnRecord {
    public:
      TrackPlaylistColumnRecord() { add(trackplaylistPlaylistID); add(trackplaylistCheckBox); add(trackplaylistCheckBoxInconsistent); add(trackplaylistPlaylistName); add(trackplaylistTrackChanges); add(trackplaylistCheckBoxOriginal); add(trackplaylistCheckBoxInconsistentOriginal); add(trackplaylistTrackCountOriginal); add(trackplaylistTrackCountChanges); add(trackplaylistVisible); }
      
      Gtk::TreeModelColumn<int> trackplaylistPlaylistID;
      Gtk::TreeModelColumn<bool> trackplaylistCheckBox;
      Gtk::TreeModelColumn<bool> trackplaylistCheckBoxInconsistent;
      Gtk::TreeModelColumn<Glib::ustring> trackplaylistPlaylistName;
      Gtk::TreeModelColumn<Glib::ustring> trackplaylistTrackChanges;
      Gtk::TreeModelColumn<bool> trackplaylistCheckBoxOriginal;
      Gtk::TreeModelColumn<bool> trackplaylistCheckBoxInconsistentOriginal;
      Gtk::TreeModelColumn<int> trackplaylistTrackCountOriginal;
      Gtk::TreeModelColumn<int> trackplaylistTrackCountChanges;
      Gtk::TreeModelColumn<bool> trackplaylistVisible;
  };
  
  class PlaylistColumnRecord : public Gtk::TreeModel::ColumnRecord {
    public:
      PlaylistColumnRecord() { add(playlistIcon); add(playlistID); add(playlistName); add(playlistTracksCount); }
      
      Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > playlistIcon;
      Gtk::TreeModelColumn<int> playlistID;
      Gtk::TreeModelColumn<Glib::ustring> playlistName;
      Gtk::TreeModelColumn<int> playlistTracksCount;
  };
  
  class GenreColumnRecord : public Gtk::TreeModel::ColumnRecord {
    public:
      GenreColumnRecord() { add(genreIcon); add(genreID); add(genreName); add(genreVisible); add(genreTracksCount); }
      
      Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > genreIcon;
      Gtk::TreeModelColumn<int> genreID;
      Gtk::TreeModelColumn<Glib::ustring> genreName;
      Gtk::TreeModelColumn<bool> genreVisible;
      Gtk::TreeModelColumn<int> genreTracksCount;
  };
  
  class ArtistColumnRecord : public Gtk::TreeModel::ColumnRecord {
    public:
      ArtistColumnRecord() { add(artistIcon); add(artistID); add(artistName); add(artistVisible); add(artistTracksCount); }
      
      Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > artistIcon;
      Gtk::TreeModelColumn<int> artistID;
      Gtk::TreeModelColumn<Glib::ustring> artistName;
      Gtk::TreeModelColumn<bool> artistVisible;
      Gtk::TreeModelColumn<int> artistTracksCount;
  };
  
  class YearColumnRecord : public Gtk::TreeModel::ColumnRecord {
    public:
      YearColumnRecord() { add(yearIcon); add(yearID); add(yearName); add(yearVisible); add(yearTracksCount); }
      
      Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > yearIcon;
      Gtk::TreeModelColumn<int> yearID;
      Gtk::TreeModelColumn<Glib::ustring> yearName;
      Gtk::TreeModelColumn<bool> yearVisible;
      Gtk::TreeModelColumn<int> yearTracksCount;
  };
  
  class AlbumColumnRecord : public Gtk::TreeModel::ColumnRecord {
    public:
      AlbumColumnRecord() { add(albumIcon); add(albumID); add(albumName); add(albumVisible); add(albumTracksCount); }
      
      Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > albumIcon;
      Gtk::TreeModelColumn<int> albumID;
      Gtk::TreeModelColumn<Glib::ustring> albumName;
      Gtk::TreeModelColumn<bool> albumVisible;
      Gtk::TreeModelColumn<int> albumTracksCount;
  };
}

#endif //INCLUDED_DINGO_HEADER_H
