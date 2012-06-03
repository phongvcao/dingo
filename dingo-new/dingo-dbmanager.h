#ifndef INCLUDED_DINGO_DBMANAGER_H
#define INCLUDED_DINGO_DBMANAGER_H

#include "dingo-sqlite3.h"
#include "dingo-tagreader.h"
#include "dingo-header.h"

namespace Dingo {
  class DBManObserver;

  class DBManager {
    public:
      DBManager();
      DBManager(Glib::ustring file_path);
      virtual ~DBManager();
      
      //IMPORT DB
      bool open(Glib::ustring file_path);
      void close();
      
      //OBSERVER'S METHODS
      void registerDBManObserver(DBManObserver* observer);
      void removeDBManObserver(DBManObserver* observer);
      void notifyDBManObserver(BuiltIn::DBNotifyType notify_type);
      
      //ADD NEW SONGS TO DB
      void addTrackToSource(std::vector<Glib::ustring> uris_vector);
      
      //the models are static
      static Glib::RefPtr<Gtk::ListStore> trackModel;
      static std::auto_ptr<TrackColumnRecord> trackCR;
    
    private:
      //LOADING LISTSTORE
      void createTrackModel();
    
      //DATA MEMBERS
      SQLite3 d_sqlite3;
      TagReader d_tagreader;
      std::vector<DBManObserver*> dbman_vector;
      
      //this is for the ACCESSORS only
      Glib::ustring cur_sourcename;
      int cur_track_id;
  };
}

#endif //INCLUDED_DINGO_DBMANAGER_H
