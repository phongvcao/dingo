#ifndef INCLUDED_DINGO_HEADER_H
#define INCLUDED_DINGO_HEADER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {  
  namespace Setting {
    enum SettingID {
      DELETE_LIBRARY = 0,
      DELETE_HARDDRIVE,
      REPEAT_MODE,
      SORT_COLUMN,
      SORT_TYPE,
      CLOSE_MODE,
      SYSTRAY_MODE
    };
  
    enum DeleteLibrary {
      DELETE_LIBRARY_NO_ASK = 0,
      DELETE_LIBRARY_ASK
    };
  
    enum DeleteHardDrive {
      DELETE_DRIVE_NO_ASK = 0,
      DELETE_DRIVE_ASK
    };
  
    enum RepeatMode {
      REPEAT_OFF = 0,
      REPEAT_SINGLE,
      REPEAT_ALL
    };     
    
    enum SortColumn {
      COLUMN_TITLE = 0,
      COLUMN_ARTIST,
      COLUMN_ALBUM
    };
    
    enum SortType {
      SORT_NONE = 0,
      SORT_ASCENDING,
      SORT_DESCENDING
    };
  
    enum CloseMode {
      CLOSE_QUIT = 0,
      CLOSE_MINIMIZE
    };
  
    enum SystrayMode {
      SYSTRAY_SHOW = 0,
      SYSTRAY_HIDE
    };
  }
  
  namespace BuiltIn {
    enum PlaylistType {
      PLAYLIST_NORMAL = 0,
      PLAYLIST_SMART
    };
    
    enum SourceType {
      SOURCE_HOME = 0,
      SOURCE_QUEUE
    };
      
    enum DBNotifyType {
      NORMAL_PLAYLIST_CREATED = 0,
      SMART_PLAYLIST_CREATED,
      NORMAL_PLAYLIST_DELETED,
      SMART_PLAYLIST_DELETED
    };
  }
  
  class PlaylistColumnRecord : public Gtk::TreeModel::ColumnRecord {
    public:
      PlaylistColumnRecord() { add(playlistTypeID); add(playlistTypeName); add(playlistID); add(playlistName); add(playlistAutoStart); }
          
      Gtk::TreeModelColumn<int> playlistTypeID;
      Gtk::TreeModelColumn<Glib::ustring> playlistTypeName;
      Gtk::TreeModelColumn<int> playlistID;
      Gtk::TreeModelColumn<Glib::ustring> playlistName;
      Gtk::TreeModelColumn<bool> playlistAutoStart;
  };
  
  class BehaviorsColumnRecord : public Gtk::TreeModel::ColumnRecord {
    public:
      BehaviorsColumnRecord() { add(behaviorID); add(behaviorName); }
          
      Gtk::TreeModelColumn<int> behaviorID;
      Gtk::TreeModelColumn<Glib::ustring> behaviorName;
  };
    
  class TrackColumnRecord : public Gtk::TreeModel::ColumnRecord {
    public:
      TrackColumnRecord() { add(trackStatus); add(trackID); add(trackNumber); add(trackTitle); add(trackGenre); add(trackArtist); add(trackAlbum); add(trackTime); }
        
      Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > trackStatus;
      Gtk::TreeModelColumn<int> trackID;
      Gtk::TreeModelColumn<int> trackNumber;
      Gtk::TreeModelColumn<Glib::ustring> trackTitle;
      Gtk::TreeModelColumn<Glib::ustring> trackGenre;
      Gtk::TreeModelColumn<Glib::ustring> trackArtist;
      Gtk::TreeModelColumn<Glib::ustring> trackAlbum;
      Gtk::TreeModelColumn<Glib::ustring> trackTime;
  };
}

#endif //INCLUDED_DINGO_HEADER_H
