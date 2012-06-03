#include "dingo-dbmanager.h"
#include "dingo-dbmanobserver.h"
#include "dingo-converter.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

Glib::RefPtr<Gtk::ListStore> Dingo::DBManager::trackModel;
std::auto_ptr<Dingo::TrackColumnRecord> Dingo::DBManager::trackCR;

//CONSTRUCTORS - DESTRUCTOR
Dingo::DBManager::DBManager() {
  cur_track_id = 0;
  cur_sourcename = "";
  
  Dingo::DBManager::trackCR.reset(new Dingo::TrackColumnRecord());
}

Dingo::DBManager::DBManager(Glib::ustring file_path) :
  d_sqlite3(file_path.raw())
{
  cur_track_id = 0;
  cur_sourcename = "";
  
  Dingo::DBManager::trackCR.reset(new Dingo::TrackColumnRecord());
  
  createTrackModel();
}

Dingo::DBManager::~DBManager() {

}
      
//IMPORT DB
bool Dingo::DBManager::open(Glib::ustring file_path) {
  d_sqlite3.open(file_path.raw());
  
  createTrackModel();
}

void Dingo::DBManager::close() {
  d_sqlite3.close();
}
      
//OBSERVER'S METHODS
void Dingo::DBManager::registerDBManObserver(Dingo::DBManObserver* observer) {
  dbman_vector.push_back(observer);
}

void Dingo::DBManager::removeDBManObserver(Dingo::DBManObserver* observer) {
  std::vector<Dingo::DBManObserver*>::iterator iter = std::find(dbman_vector.begin(), dbman_vector.end(), observer);
  
  if (iter != dbman_vector.end()) {
    dbman_vector.erase(iter);
  }
}

void Dingo::DBManager::notifyDBManObserver(Dingo::BuiltIn::DBNotifyType notify_type) {
  switch (notify_type) {
    case (Dingo::BuiltIn::NORMAL_PLAYLIST_CREATED): {
      for (std::vector<Dingo::DBManObserver*>::iterator iter = dbman_vector.begin(); iter != dbman_vector.end(); ++iter) {
        (*iter)->updateNormalPlaylistCreated();
      }
    }
    
    case (Dingo::BuiltIn::SMART_PLAYLIST_CREATED): {
      for (std::vector<Dingo::DBManObserver*>::iterator iter = dbman_vector.begin(); iter != dbman_vector.end(); ++iter) {
        (*iter)->updateSmartPlaylistCreated();
      }
    }
    
    case (Dingo::BuiltIn::NORMAL_PLAYLIST_DELETED): {
      for (std::vector<Dingo::DBManObserver*>::iterator iter = dbman_vector.begin(); iter != dbman_vector.end(); ++iter) {
        (*iter)->updateNormalPlaylistDeleted();
      }
    }
    
    case (Dingo::BuiltIn::SMART_PLAYLIST_DELETED): {
      for (std::vector<Dingo::DBManObserver*>::iterator iter = dbman_vector.begin(); iter != dbman_vector.end(); ++iter) {
        (*iter)->updateSmartPlaylistDeleted();
      }
    }
    
    default: {
      std::cerr << "Undefined Dingo::BuiltIn::NotifyType" << std::endl;
    }
  }
}
      
void Dingo::DBManager::addTrackToSource(std::vector<Glib::ustring> uris_vector) {
  Gtk::TreeModel::Row row;

  for (std::vector<Glib::ustring>::iterator iter = uris_vector.begin(); iter != uris_vector.end(); ++iter) {
    d_tagreader.read(*iter);
    
    d_sqlite3.prepareStatement("select coretracks.trackid from coretracks where coretracks.trackuri = :trackuri;");
    
    d_sqlite3.bindParaText(":trackuri", *iter);
    
    if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      d_sqlite3.finalizeStatement();
      
      continue;
    }
    
    else {
      int temp_artistid = 1;
      int temp_albumid = 1;
      int temp_trackid = 1;
      
      d_sqlite3.finalizeStatement();
      
      //insert the new artistname
      d_sqlite3.prepareStatement("insert into coreartists (artistname) values (:artistname);");
        
      d_sqlite3.bindParaText(":artistname", d_tagreader.getArtist());
      
      d_sqlite3.stepStatement();
      
      d_sqlite3.finalizeStatement();
      
      //get the new artist's artistid
      d_sqlite3.prepareStatement("select coreartists.artistid from coreartists where coreartists.artistname = :artistname;");
      
      d_sqlite3.bindParaText(":artistname", d_tagreader.getArtist());
      
      if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
        temp_artistid = d_sqlite3.getIntValue(0);
      }
      
      d_sqlite3.finalizeStatement();
      
      //insert the new albumname
      d_sqlite3.prepareStatement("insert into corealbums (albumname) values (:albumname);");
      
      d_sqlite3.bindParaText(":albumname", d_tagreader.getAlbum());
      
      d_sqlite3.stepStatement();
      
      d_sqlite3.finalizeStatement();
      
      //get the new album's albumid
      d_sqlite3.prepareStatement("select corealbums.albumid from corealbums where corealbums.albumname = :albumname;");
      
      d_sqlite3.bindParaText(":albumname", d_tagreader.getAlbum());
      
      if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
        temp_albumid = d_sqlite3.getIntValue(0);
      }
      
      d_sqlite3.finalizeStatement();
      
      //insert song's info into coretracks  
      d_sqlite3.prepareStatement("insert into coretracks (tracktitle, trackgenre, artistid, albumid, tracknumber, tracktime, tracksize, trackfilename, trackuri, trackyear, trackbitrate, trackdateadded) values (:tracktitle, :trackgenre, :artistid, :albumid, :tracknumber, :tracktime, :tracksize, :trackfilename, :trackuri, :trackyear, :trackbitrate, :trackdateadded);");
        
      d_sqlite3.bindParaInt(":artistid", temp_artistid);
      d_sqlite3.bindParaInt(":albumid", temp_albumid);
      d_sqlite3.bindParaInt(":tracknumber", d_tagreader.getTrackNumber());
      d_sqlite3.bindParaText(":tracktitle", d_tagreader.getTitle());
      std::cout << "Track Title: " << d_tagreader.getTitle() << std::endl;
      
      d_sqlite3.bindParaText(":trackgenre", d_tagreader.getGenre());
      std::cout << "Track Genre: " << d_tagreader.getGenre() << std::endl;
      
      d_sqlite3.bindParaInt64(":tracktime", d_tagreader.getDuration());
      d_sqlite3.bindParaInt64(":tracksize", d_tagreader.getSize());
      d_sqlite3.bindParaText(":trackfilename", d_tagreader.getFileName());
      d_sqlite3.bindParaText(":trackuri", d_tagreader.getFileURI());
      d_sqlite3.bindParaInt(":trackyear", d_tagreader.getYear());
      d_sqlite3.bindParaInt(":trackbitrate", d_tagreader.getBitrate());
      d_sqlite3.bindParaText(":trackdateadded", d_tagreader.getDateAdded());
      
      d_sqlite3.stepStatement();
      
      d_sqlite3.finalizeStatement();
      
      
      //insert song's info into Gtk::ListStore
      d_sqlite3.prepareStatement("select max(trackid) from coretracks;");
      
      if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
        temp_trackid = d_sqlite3.getIntValue(0);
        std::cout << "TrackID: " << temp_trackid << std::endl;
        
        row = *(Dingo::DBManager::trackModel->append());
      
        row[(*Dingo::DBManager::trackCR).trackID] = temp_trackid;
        row[(*Dingo::DBManager::trackCR).trackNumber] = d_tagreader.getTrackNumber();
        row[(*Dingo::DBManager::trackCR).trackTitle] = d_tagreader.getTitle();
        row[(*Dingo::DBManager::trackCR).trackGenre] = d_tagreader.getGenre();
        row[(*Dingo::DBManager::trackCR).trackArtist] = d_tagreader.getArtist();
        row[(*Dingo::DBManager::trackCR).trackAlbum] = d_tagreader.getAlbum();
        row[(*Dingo::DBManager::trackCR).trackTime] = Dingo::Converter::durationToString(d_tagreader.getDuration());
      }

      d_sqlite3.finalizeStatement();
    }
  }
}

void Dingo::DBManager::createTrackModel() {
  Dingo::DBManager::trackModel = Gtk::ListStore::create(*Dingo::DBManager::trackCR);

  d_sqlite3.prepareStatement("select coretracks.trackid, coretracks.tracknumber, coretracks.tracktitle, coretracks.trackgenre, coreartists.artistname, corealbums.albumname, coretracks.tracktime from coretracks inner join coreartists on coretracks.artistid = coreartists.artistid inner join corealbums on coretracks.albumid = corealbums.albumid order by coretracks.tracktitle;");
  
  while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    Gtk::TreeModel::iterator iter = Dingo::DBManager::trackModel->append();
    
    Gtk::TreeModel::Row row = *iter;
    
    row[(*Dingo::DBManager::trackCR).trackID] = d_sqlite3.getIntValue(0);
    row[(*Dingo::DBManager::trackCR).trackNumber] = d_sqlite3.getIntValue(1);
    row[(*Dingo::DBManager::trackCR).trackTitle] = d_sqlite3.getStringValue(2);
    row[(*Dingo::DBManager::trackCR).trackGenre] = d_sqlite3.getStringValue(3);
    row[(*Dingo::DBManager::trackCR).trackArtist] = d_sqlite3.getStringValue(4);
    row[(*Dingo::DBManager::trackCR).trackAlbum] = d_sqlite3.getStringValue(5);
    row[(*Dingo::DBManager::trackCR).trackTime] = Dingo::Converter::durationToString(d_sqlite3.getInt64Value(6));
    
    /* std::cout << "Track ID: "
              << d_sqlite3.getIntValue(0) << std::endl;
    std::cout << "Track Number: "
              << d_sqlite3.getIntValue(1) << std::endl;
    std::cout << "Track Title: " 
              << d_sqlite3.getStringValue(2) << std::endl;
    std::cout << "Track Genre: " 
              << d_sqlite3.getStringValue(3) << std::endl;
    std::cout << "Track Artist: "
              << d_sqlite3.getStringValue(4) << std::endl;
    std::cout << "Track Album: "
              << d_sqlite3.getStringValue(5) << std::endl;
    std::cout << "Track Time: "
              << Dingo::Converter::durationToString(d_sqlite3.getInt64Value(6)) << std::endl;
              
    std::cout << std::endl;
    std::cout << std::endl; */
  }
  
  d_sqlite3.finalizeStatement();
}
    
