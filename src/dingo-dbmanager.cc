/* 
 * dingo-dbmanager.cc
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

#include "dingo-dbmanager.h"
#include "dingo-playlistreader.h"
#include "dingo-playlistwriter.h"
#include "dingo-dbmanplaylistobserver.h"
#include "dingo-dbmangenreobserver.h"
#include "dingo-dbmanartistobserver.h"
#include "dingo-dbmanyearobserver.h"
#include "dingo-dbmanalbumobserver.h"
#include "dingo-dbmantrackobserver.h"
#include "dingo-processgrid.h"
#include "dingo-utilities.h"
#include "dingo-mprisv2.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

#ifndef INCLUDED_GLIBMM_I18N_H
#define INCLUDED_GLIBMM_I18N_H
#include <glibmm/i18n.h>
#endif //INCLUDED_GLIBMM_I18N_H

#ifndef INCLUDED_SET
#define INCLUDED_SET
#include <set>
#endif //INCLUDED_SET

//PUBLIC STATIC DATA MEMBERS      
Glib::RefPtr<Gtk::ListStore> Dingo::DBManager::playlistModel;
std::auto_ptr<Dingo::PlaylistColumnRecord> Dingo::DBManager::playlistCR;
Glib::RefPtr<Gtk::ListStore> Dingo::DBManager::genreModel;
std::auto_ptr<Dingo::GenreColumnRecord> Dingo::DBManager::genreCR;
Glib::RefPtr<Gtk::ListStore> Dingo::DBManager::artistModel;
std::auto_ptr<Dingo::ArtistColumnRecord> Dingo::DBManager::artistCR;
Glib::RefPtr<Gtk::ListStore> Dingo::DBManager::yearModel;
std::auto_ptr<Dingo::YearColumnRecord> Dingo::DBManager::yearCR;
Glib::RefPtr<Gtk::ListStore> Dingo::DBManager::albumModel;
std::auto_ptr<Dingo::AlbumColumnRecord> Dingo::DBManager::albumCR;
Glib::RefPtr<Gtk::ListStore> Dingo::DBManager::trackModel;
std::auto_ptr<Dingo::TrackColumnRecord> Dingo::DBManager::trackCR;
Glib::RefPtr<Gtk::ListStore> Dingo::DBManager::trackplaylistModel;
std::auto_ptr<Dingo::TrackPlaylistColumnRecord> Dingo::DBManager::trackplaylistCR;

//CONSTRUCTORS - DESTRUCTOR
Dingo::DBManager::DBManager() {

}

Dingo::DBManager::DBManager(Glib::ustring database_path) :
  d_processid_queue(20, false),
  d_playlist_image(DATADIR"/dingo/icons/16x16/dingo-playlist.png"),
  d_genre_image(DATADIR"/dingo/icons/16x16/dingo-genre.png"),
  d_artist_image(DATADIR"/dingo/icons/16x16/dingo-artist.png"),
  d_year_image(DATADIR"/dingo/icons/16x16/dingo-year.png"),
  d_album_image(DATADIR"/dingo/icons/16x16/dingo-album.png")
{
  cur_playlist_id = 0;
  is_all_artists_selected = true;
  is_all_albums_selected = true;
  
  artists_selected_query = "";
  albums_selected_query = "";
  
  d_search_text = "";
  d_mimetype_filter_str = "%%";
  prev_playlist_id = -1;
  d_database_path = database_path;
  
  parent_x = 0;
  parent_y = 0;
  
  d_first_added_track_id = 0;
  d_last_imported_playlist_id = 0;
  
  Dingo::DBManager::playlistCR.reset(new Dingo::PlaylistColumnRecord());
  Dingo::DBManager::genreCR.reset(new Dingo::GenreColumnRecord());
  Dingo::DBManager::artistCR.reset(new Dingo::ArtistColumnRecord());
  Dingo::DBManager::yearCR.reset(new Dingo::YearColumnRecord());
  Dingo::DBManager::albumCR.reset(new Dingo::AlbumColumnRecord());
  Dingo::DBManager::trackCR.reset(new Dingo::TrackColumnRecord());
  Dingo::DBManager::trackplaylistCR.reset(new Dingo::TrackPlaylistColumnRecord());
  
  createDatabase();
  createGenreModel();
  createArtistModel();
  createYearModel();
  createAlbumModel();
  createTrackModel();
  createPlaylistModel();
  createTrackPlaylistModel();
}

Dingo::DBManager::~DBManager() {

}

//OBSERVER'S METHODS
void Dingo::DBManager::registerDBManPlaylistObserver(Dingo::DBManPlaylistObserver* observer) {
  dbman_playlist_obs_vector.push_back(observer);
}

void Dingo::DBManager::removeDBManPlaylistObserver(Dingo::DBManPlaylistObserver* observer) {
  std::vector<Dingo::DBManPlaylistObserver*>::iterator iter = std::find(dbman_playlist_obs_vector.begin(), dbman_playlist_obs_vector.end(), observer);
  
  if (iter != dbman_playlist_obs_vector.end()) {
    dbman_playlist_obs_vector.erase(iter);
  }
}

void Dingo::DBManager::notifyDBManPlaylistObserver(Dingo::DBManSignals signal_type) {
  switch (signal_type) {
    case Dingo::CHANGES_IN_CORE_PLAYLISTS: {
      for (std::vector<Dingo::DBManPlaylistObserver*>::iterator iter = dbman_playlist_obs_vector.begin(); iter != dbman_playlist_obs_vector.end(); ++iter) {
        (*iter)->updateChangesInCorePlaylists();
      }
      //std::cout << "Changes in Core Playlists!" << std::endl;
      break;
    }
  
    default: {
      //std::cerr << "Undefined Dingo::DBManSignals!" << std::endl;
      break;
    }
  }
}

void Dingo::DBManager::registerDBManGenreObserver(Dingo::DBManGenreObserver* observer) {
  dbman_genre_obs_vector.push_back(observer);
}

void Dingo::DBManager::removeDBManGenreObserver(Dingo::DBManGenreObserver* observer) {
  std::vector<Dingo::DBManGenreObserver*>::iterator iter = std::find(dbman_genre_obs_vector.begin(), dbman_genre_obs_vector.end(), observer);
  
  if (iter != dbman_genre_obs_vector.end()) {
    dbman_genre_obs_vector.erase(iter);
  }
}

void Dingo::DBManager::notifyDBManGenreObserver(Dingo::DBManSignals signal_type) {
  switch (signal_type) {
    case Dingo::CHANGES_IN_CORE_GENRES: {
      for (std::vector<Dingo::DBManGenreObserver*>::iterator iter = dbman_genre_obs_vector.begin(); iter != dbman_genre_obs_vector.end(); ++iter) {
        (*iter)->updateChangesInCoreGenres();
      }
      break;
    }
    
    default: {
      //std::cout << "Unknown Dingo::DBManSignals passed to notifyDBManGenreObserver()!" << std::endl;
      break;
    }
  }
}

void Dingo::DBManager::registerDBManArtistObserver(Dingo::DBManArtistObserver* observer) {
  dbman_artist_obs_vector.push_back(observer);
}

void Dingo::DBManager::removeDBManArtistObserver(Dingo::DBManArtistObserver* observer) {
  std::vector<Dingo::DBManArtistObserver*>::iterator iter = std::find(dbman_artist_obs_vector.begin(), dbman_artist_obs_vector.end(), observer);
  
  if (iter != dbman_artist_obs_vector.end()) {
    dbman_artist_obs_vector.erase(iter);
  }
}

void Dingo::DBManager::notifyDBManArtistObserver(Dingo::DBManSignals signal_type) {
  switch (signal_type) {
    case Dingo::CHANGES_IN_CORE_ARTISTS: {
      for (std::vector<Dingo::DBManArtistObserver*>::iterator iter = dbman_artist_obs_vector.begin(); iter != dbman_artist_obs_vector.end(); ++iter) {
        (*iter)->updateChangesInCoreArtists(); 
      }
      //std::cout << "Changes in Core Artists" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unknown Dingo::DBManSignals passed to notifyDBManArtistObserver()!" << std::endl;
      break;
    }
  }
}

void Dingo::DBManager::registerDBManYearObserver(Dingo::DBManYearObserver* observer) {
  dbman_year_obs_vector.push_back(observer);
}

void Dingo::DBManager::removeDBManYearObserver(Dingo::DBManYearObserver* observer) {
  std::vector<Dingo::DBManYearObserver*>::iterator iter = std::find(dbman_year_obs_vector.begin(), dbman_year_obs_vector.end(), observer);
  
  if (iter != dbman_year_obs_vector.end()) {
    dbman_year_obs_vector.erase(iter);
  }
}

void Dingo::DBManager::notifyDBManYearObserver(Dingo::DBManSignals signal_type) {
  switch (signal_type) {
    case Dingo::CHANGES_IN_CORE_YEARS: {
      for (std::vector<Dingo::DBManYearObserver*>::iterator iter = dbman_year_obs_vector.begin(); iter != dbman_year_obs_vector.end(); ++iter) {
        (*iter)->updateChangesInCoreYears();
      }
    }
    
    default: {
      //std::cout << "Unknown Dingo::DBManSignals passed to notifyDBManYearObserver()!" << std::endl;
      break;
    }
  }
}
      
void Dingo::DBManager::registerDBManAlbumObserver(Dingo::DBManAlbumObserver* observer) {
  dbman_album_obs_vector.push_back(observer);
}

void Dingo::DBManager::removeDBManAlbumObserver(Dingo::DBManAlbumObserver* observer) {
  std::vector<Dingo::DBManAlbumObserver*>::iterator iter = std::find(dbman_album_obs_vector.begin(), dbman_album_obs_vector.end(), observer);
  
  if (iter != dbman_album_obs_vector.end()) {
    dbman_album_obs_vector.erase(iter);
  }
}

void Dingo::DBManager::notifyDBManAlbumObserver(Dingo::DBManSignals signal_type) {
  switch (signal_type) {
    case Dingo::CHANGES_IN_CORE_ALBUMS: {
      for (std::vector<Dingo::DBManAlbumObserver*>::iterator iter = dbman_album_obs_vector.begin(); iter != dbman_album_obs_vector.end(); ++iter) {
        (*iter)->updateChangesInCoreAlbums();
      }
      //std::cout << "Changes in Core Albums" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unknown Dingo::DBManSignals passed to notifyDBManAlbumObserver()!" << std::endl;
      break;
    }
  }
}

void Dingo::DBManager::registerDBManTrackObserver(Dingo::DBManTrackObserver* observer) {
  dbman_track_obs_vector.push_back(observer);
}

void Dingo::DBManager::removeDBManTrackObserver(Dingo::DBManTrackObserver* observer) {
  std::vector<Dingo::DBManTrackObserver*>::iterator iter = std::find(dbman_track_obs_vector.begin(), dbman_track_obs_vector.end(), observer);
  
  if (iter != dbman_track_obs_vector.end()) {
    dbman_track_obs_vector.erase(iter);
  }
}

void Dingo::DBManager::notifyDBManTrackObserver(Dingo::DBManSignals signal_type) {
  switch (signal_type) {
    case Dingo::CHANGES_IN_CORE_TRACKS: {
      for (std::vector<Dingo::DBManTrackObserver*>::iterator iter = dbman_track_obs_vector.begin(); iter != dbman_track_obs_vector.end(); ++iter) {
        (*iter)->updateChangesInCoreTracks();
      }
      //std::cout << "Changes in Core Tracks" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unknown Dingo::DBManSignals passed to notifyDBManTrackObserver()!" << std::endl;
      break;
    }
  }
}

//INHERITED OBSERVER'S METHODS
void Dingo::DBManager::updateProcessFinished(Dingo::ProcessID process_id) {
  int temp_process_id = process_id;

  d_processid_queue[temp_process_id] = false;
  
  //check if there is any active process
  //  if there is one or more then set the current process_id to that
  //  process
  //  also set the major information in processgrid to fit the process_id
  //  (this should be done inside Dingo::ProcessGrid)
  std::vector<bool>::iterator iter = std::find(d_processid_queue.begin(), d_processid_queue.end(), true);
  
  if (iter != d_processid_queue.end()) {
    d_processgrid->setCurrentProcessID(process_id);
  }
  
  else {
    d_processgrid->setCurrentProcessID(Dingo::PROCESS_NONE);
  }
}

//PUBLIC ACCESSORS
void Dingo::DBManager::setProcessGrid(Dingo::ProcessGrid* processgrid) {
  d_processgrid = processgrid;
}

Gtk::Grid* Dingo::DBManager::getProcessGrid() {
  return d_processgrid;
}

void Dingo::DBManager::setMainWindowVisible(bool is_visible) {
  if (is_visible) {
    parent_window->show();
    
    parent_window->move(parent_x, parent_y);
  }
  
  else {
    parent_window->get_position(parent_x, parent_y);
    
    parent_window->hide();
  }
}

bool Dingo::DBManager::getMainWindowVisible() {
  return parent_window->get_visible();
}

void Dingo::DBManager::saveMainWindowPosition() {
  parent_window->get_position(parent_x, parent_y);
}

bool Dingo::DBManager::getIsAnyTrackInClipboard() {
  return !d_clipboard_tracks_id_vector.empty();
}

//Dingo::MPRISCommon's METHODS
Gtk::TreeRowReference Dingo::DBManager::getFirstAddedTrackRowRef() {
  Gtk::TreeModel::Children children = Dingo::DBManager::trackModel->children();
  Gtk::TreeRowReference return_ref;
  
  for (Gtk::TreeModel::Children::iterator children_iter = children.begin(); children_iter != children.end(); ++children_iter) {
    Gtk::TreeModel::Row row = *children_iter;
    
    if (row[(*Dingo::DBManager::trackCR).trackID] == d_first_added_track_id) {
      Gtk::TreeRowReference temp_ref(Dingo::DBManager::trackModel, Dingo::DBManager::trackModel->get_path(children_iter));
      
      return_ref = temp_ref;
      
      break;
    } 
  }
  
  return return_ref;
}

int Dingo::DBManager::getLastImportedPlaylistID() {
  return d_last_imported_playlist_id;
}

std::map<int, Glib::ustring> Dingo::DBManager::getPlaylists(int max_count, bool is_reverse) {
  std::map<int, Glib::ustring> return_map;
  
  return_map.insert(std::pair<int, Glib::ustring>(0, _("All Tracks")));
  return_map.insert(std::pair<int, Glib::ustring>(1, _("Play Queue")));
  
  max_count = max_count - 2;
  
  d_sqlite3.quickExecute("BEGIN TRANSACTION;");
  
  Glib::ustring query_statement = "SELECT CorePlaylists.PlaylistID, CorePlaylists.PlaylistName FROM CorePlaylists ORDER BY CorePlaylists.PlaylistName";
  
  if (!is_reverse) {
    query_statement += ";";
  }
  
  else {
    query_statement += " DESC;";
  }
  
  d_sqlite3.prepareStatement(query_statement);
  
  while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    return_map.insert(std::pair<int, Glib::ustring>(d_sqlite3.getIntValue(0), d_sqlite3.getStringValue(1)));
    
    --max_count;
    
    if (max_count == 0) {
      break;
    }
  }
  
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
  return return_map;
}

int Dingo::DBManager::getPlaylistsCount() {
  int playlists_count = 0;

  d_sqlite3.quickExecute("BEGIN TRANSACTION;");
  
  d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CorePlaylists;");
  
  if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    playlists_count = d_sqlite3.getIntValue(0);
  }
  
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
  return ++playlists_count;
}

Glib::ustring Dingo::DBManager::getCurrentPlaylistName() {
  if (cur_playlist_id == 0) {
    return _("All Tracks");
  }
  
  else if (cur_playlist_id == 1) {
    return _("Play Queue");
  }
  
  else {
    Glib::ustring return_playlist_name = _("All Tracks");
  
    d_sqlite3.quickExecute("BEGIN TRANSACTION;");
  
    d_sqlite3.prepareStatement("SELECT CorePlaylists.PlaylistName FROM CorePlaylists WHERE CorePlaylists.PlaylistID = :PlaylistID;");
  
    d_sqlite3.bindParaInt(":PlaylistID", cur_playlist_id);
  
    if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      return_playlist_name = d_sqlite3.getStringValue(0);
    }
  
    d_sqlite3.finalizeStatement();
  
    d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
    return return_playlist_name;
  }
}
      
//Dingo::MenuBar's METHODS
void Dingo::DBManager::addTracksToDatabase(std::vector<Glib::ustring> uris_vector, int playlist_id) {
  d_processid_queue[Dingo::PROCESS_IMPORTING_FILES] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_IMPORTING_FILES);
  
  updateGUI();
  
  if (!d_processid_queue[Dingo::PROCESS_IMPORTING_FILES]) {
    return;
  }
  
  std::vector<std::vector<Glib::ustring> > all_tracks_info_vector;
  std::set<Glib::ustring> all_tracks_genre_set;
  std::set<Glib::ustring> all_tracks_artist_set;
  std::set<Glib::ustring> all_tracks_year_set;
  std::set<Glib::ustring> all_tracks_album_set;
  
  for (std::vector<Glib::ustring>::iterator iter = uris_vector.begin(); iter != uris_vector.end(); ++iter) {
    if (!d_processid_queue[Dingo::PROCESS_IMPORTING_FILES]) {
      break;
    }
  
    if (d_tagreader.read(*iter)) {
      std::vector<Glib::ustring> track_info_vector;
    
      track_info_vector.push_back(d_tagreader.getGenre());
      all_tracks_genre_set.insert(d_tagreader.getGenre());
    
      track_info_vector.push_back(d_tagreader.getArtist());
      all_tracks_artist_set.insert(d_tagreader.getArtist());
    
      track_info_vector.push_back(d_tagreader.getYear());
      all_tracks_year_set.insert(d_tagreader.getYear());
    
      track_info_vector.push_back(d_tagreader.getAlbum());
      all_tracks_album_set.insert(d_tagreader.getAlbum());
    
      track_info_vector.push_back(Dingo::Utilities::intToString(d_tagreader.getTrackNumber()));
      track_info_vector.push_back(d_tagreader.getTitle());
      track_info_vector.push_back(d_tagreader.getComment());
      track_info_vector.push_back(d_tagreader.getLyrics());
      track_info_vector.push_back(Dingo::Utilities::GInt64ToString(d_tagreader.getDuration()));
      track_info_vector.push_back(Dingo::Utilities::GInt64ToString(d_tagreader.getFileSize()));
      track_info_vector.push_back(d_tagreader.getFileName());
      track_info_vector.push_back(d_tagreader.getFileExtension());
      track_info_vector.push_back(d_tagreader.getFileURI());
      track_info_vector.push_back("");
      track_info_vector.push_back(Dingo::Utilities::intToString(d_tagreader.getBitrate()));
      track_info_vector.push_back(d_tagreader.getMIMEType());
      track_info_vector.push_back(d_tagreader.getDateAdded());
    
      all_tracks_info_vector.push_back(track_info_vector);
    }
    
    updateGUI();
  }
  
  d_sqlite3.quickExecute("BEGIN TRANSACTION;");
  
  insertGenresToDatabase(all_tracks_genre_set, Dingo::PROCESS_IMPORTING_FILES);
  insertArtistsToDatabase(all_tracks_artist_set, Dingo::PROCESS_IMPORTING_FILES);
  insertYearsToDatabase(all_tracks_year_set, Dingo::PROCESS_IMPORTING_FILES);
  insertAlbumsToDatabase(all_tracks_album_set, Dingo::PROCESS_IMPORTING_FILES);
  insertTracksToDatabase(all_tracks_info_vector, Dingo::PROCESS_IMPORTING_FILES, playlist_id);  
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
  updateGUI();
  
  d_processid_queue[Dingo::PROCESS_IMPORTING_FILES] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_IMPORTING_FILES) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }
}

void Dingo::DBManager::addTracksFolderToDatabase(std::vector<Glib::ustring> folder_uris_vector) {
  d_processid_queue[Dingo::PROCESS_IMPORTING_FOLDERS] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_IMPORTING_FOLDERS);
  
  updateGUI();
  
  if (!d_processid_queue[Dingo::PROCESS_IMPORTING_FOLDERS]) {
    return;
  }
  
  std::vector<std::vector<Glib::ustring> > all_tracks_info_vector;
  std::set<Glib::ustring> all_tracks_genre_set;
  std::set<Glib::ustring> all_tracks_artist_set;
  std::set<Glib::ustring> all_tracks_year_set;
  std::set<Glib::ustring> all_tracks_album_set;
  
  for (std::vector<Glib::ustring>::iterator iter = folder_uris_vector.begin(); iter != folder_uris_vector.end(); ++iter) {
    addTracksFolderRecursively(Glib::filename_from_uri(*iter), all_tracks_info_vector, all_tracks_genre_set, all_tracks_artist_set, all_tracks_year_set, all_tracks_album_set);
  }
  
  d_sqlite3.quickExecute("BEGIN TRANSACTION;");
  
  insertGenresToDatabase(all_tracks_genre_set, Dingo::PROCESS_IMPORTING_FOLDERS);
  insertArtistsToDatabase(all_tracks_artist_set, Dingo::PROCESS_IMPORTING_FOLDERS);
  insertYearsToDatabase(all_tracks_year_set, Dingo::PROCESS_IMPORTING_FOLDERS);
  insertAlbumsToDatabase(all_tracks_album_set, Dingo::PROCESS_IMPORTING_FOLDERS);
  insertTracksToDatabase(all_tracks_info_vector, Dingo::PROCESS_IMPORTING_FOLDERS);  
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
  updateGUI();
  
  d_processid_queue[Dingo::PROCESS_IMPORTING_FOLDERS] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_IMPORTING_FOLDERS) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }
}
      
//Dingo::ToolBar's METHODS
void Dingo::DBManager::setSearchText(Glib::ustring search_text) {
  d_search_text = search_text;
}

void Dingo::DBManager::setMIMETypeFilter(Glib::ustring mimetype_filter_str) {
  d_mimetype_filter_str = mimetype_filter_str;
}

Glib::ustring Dingo::DBManager::getMIMETypeFilter() {
  return d_mimetype_filter_str;
}

//Dingo::PlaylistTreeView's METHODS
void Dingo::DBManager::setCurrentPlaylistID(int playlist_id) {
  cur_playlist_id = playlist_id;
}

int Dingo::DBManager::getCurrentPlaylistID() {
  return cur_playlist_id;
}

void Dingo::DBManager::setPreviousPlaylistID(int playlist_id) {
  prev_playlist_id = playlist_id;
}

int Dingo::DBManager::getPreviousPlaylistID() {
  return prev_playlist_id;
}

int Dingo::DBManager::createPlaylist(Glib::ustring playlist_name) {
  int return_playlist_id = 0;

  d_sqlite3.prepareStatement("INSERT INTO CorePlaylists (PlaylistName) VALUES (:PlaylistName);");
  
  d_sqlite3.bindParaText(":PlaylistName", playlist_name);
  
  d_sqlite3.stepStatement();
  
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.prepareStatement("SELECT MAX(PlaylistID) FROM CorePlaylists;");
  
  if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    return_playlist_id = d_sqlite3.getIntValue(0);
  }
  
  d_sqlite3.finalizeStatement();
  
  Gtk::TreeModel::Row playlist_row = *(Dingo::DBManager::playlistModel->append());
  
  playlist_row[(*Dingo::DBManager::playlistCR).playlistIcon] = d_playlist_image.get_pixbuf();
  playlist_row[(*Dingo::DBManager::playlistCR).playlistID] = return_playlist_id;
  playlist_row[(*Dingo::DBManager::playlistCR).playlistName] = Dingo::Utilities::encodeFromEntityString(playlist_name);
  playlist_row[(*Dingo::DBManager::playlistCR).playlistTracksCount] = 0;
  
  Gtk::TreeModel::Row trackplaylist_row = *(Dingo::DBManager::trackplaylistModel->append());
  trackplaylist_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistID] = return_playlist_id;    
  trackplaylist_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox] = false;
  trackplaylist_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistent] = true;
  trackplaylist_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistName] = Dingo::Utilities::encodeFromEntityString(playlist_name);
  trackplaylist_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountOriginal] = 0;
  trackplaylist_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges] = 0;
  trackplaylist_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistVisible] = true;
  
  //emit "PlaylistCount" property changed signal
  Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYLISTS, "PlaylistCount");
  
  return return_playlist_id;
}

void Dingo::DBManager::importPlaylists(std::vector<Glib::ustring> uris_vector) {
  d_processid_queue[Dingo::PROCESS_IMPORTING_PLAYLISTS] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_IMPORTING_PLAYLISTS);
  
  if (!d_processid_queue[Dingo::PROCESS_IMPORTING_PLAYLISTS]) {
    return;
  }

  Dingo::PlaylistReader temp_playlistreader;
  
  std::vector<std::vector<Glib::ustring> > all_tracks_info_vector;
  std::set<Glib::ustring> all_tracks_genre_set;
  std::set<Glib::ustring> all_tracks_artist_set;
  std::set<Glib::ustring> all_tracks_year_set;
  std::set<Glib::ustring> all_tracks_album_set;
  
  for (std::vector<Glib::ustring>::iterator iter = uris_vector.begin(); iter != uris_vector.end(); ++iter) {
    if (!d_processid_queue[Dingo::PROCESS_IMPORTING_PLAYLISTS]) {
      break;
    }
  
    temp_playlistreader.open(*iter);
    
    //first create an empty playlist using createPlaylist()
    int temp_playlist_id = createPlaylist(temp_playlistreader.getPlaylistName());
    
    if (iter == --uris_vector.end()) {
      d_last_imported_playlist_id = temp_playlist_id;
    }
    
    //then fill tracks into the newly created playlist
    while (temp_playlistreader.moveToNextTrack()) {
      if (!d_processid_queue[Dingo::PROCESS_IMPORTING_PLAYLISTS]) {
        break;
      }
    
      std::vector<Glib::ustring> track_info_vector;
    
      track_info_vector.push_back(temp_playlistreader.getTrackGenre());
      all_tracks_genre_set.insert(temp_playlistreader.getTrackGenre());
    
      track_info_vector.push_back(temp_playlistreader.getTrackArtist());
      all_tracks_artist_set.insert(temp_playlistreader.getTrackArtist());
    
      track_info_vector.push_back(temp_playlistreader.getTrackYear());
      all_tracks_year_set.insert(temp_playlistreader.getTrackYear());
    
      track_info_vector.push_back(temp_playlistreader.getTrackAlbum());
      all_tracks_album_set.insert(temp_playlistreader.getTrackAlbum());
    
      track_info_vector.push_back(Dingo::Utilities::intToString(temp_playlistreader.getTrackNumber()));
      track_info_vector.push_back(temp_playlistreader.getTrackTitle());
      track_info_vector.push_back(temp_playlistreader.getTrackComment());
      track_info_vector.push_back(temp_playlistreader.getTrackLyrics());
      track_info_vector.push_back(Dingo::Utilities::GInt64ToString(temp_playlistreader.getTrackDuration()));
      track_info_vector.push_back(Dingo::Utilities::GInt64ToString(temp_playlistreader.getTrackSize()));
      track_info_vector.push_back(temp_playlistreader.getTrackFileName());
      track_info_vector.push_back(temp_playlistreader.getTrackFileExtension());
      track_info_vector.push_back(temp_playlistreader.getTrackURI());
      track_info_vector.push_back(temp_playlistreader.getTrackSubtitleURI());
      track_info_vector.push_back(Dingo::Utilities::intToString(temp_playlistreader.getTrackBitrate()));
      track_info_vector.push_back(temp_playlistreader.getTrackMIMEType());
      track_info_vector.push_back(temp_playlistreader.getTrackDateAdded());
    
      all_tracks_info_vector.push_back(track_info_vector);
    }
    
    d_sqlite3.quickExecute("BEGIN TRANSACTION;");
  
    insertGenresToDatabase(all_tracks_genre_set, Dingo::PROCESS_IMPORTING_PLAYLISTS);
    insertArtistsToDatabase(all_tracks_artist_set, Dingo::PROCESS_IMPORTING_PLAYLISTS);
    insertYearsToDatabase(all_tracks_year_set, Dingo::PROCESS_IMPORTING_PLAYLISTS);
    insertAlbumsToDatabase(all_tracks_album_set, Dingo::PROCESS_IMPORTING_PLAYLISTS);
    insertTracksToDatabase(all_tracks_info_vector, Dingo::PROCESS_IMPORTING_PLAYLISTS, temp_playlist_id);  
  
    d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  }
    
  d_processid_queue[Dingo::PROCESS_IMPORTING_PLAYLISTS] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_IMPORTING_PLAYLISTS) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }
}

void Dingo::DBManager::renamePlaylist(int playlist_id, Glib::ustring playlist_name_new) {
  d_sqlite3.prepareStatement("UPDATE CorePlaylists SET PlaylistName = :PlaylistName WHERE CorePlaylists.PlaylistID = :PlaylistID;");
  
  d_sqlite3.bindParaText(":PlaylistName", playlist_name_new);
  d_sqlite3.bindParaInt(":PlaylistID", playlist_id);
  
  if (d_sqlite3.stepStatement() != Dingo::SQLite::ERROR) {
    //std::cout << "Playlist Rename Succeeded!" << std::endl;
  }
  
  else {
    //std::cout << "Playlist Rename Failed!" << std::endl;
  }
  
  d_sqlite3.finalizeStatement();
  
  //rename the playlist inside Dingo::DBManager::playlistModel
  Gtk::TreeModel::Children playlistModel_children = Dingo::DBManager::playlistModel->children();
  
  for (Gtk::TreeModel::Children::iterator playlistModel_iter = playlistModel_children.begin(); playlistModel_iter != playlistModel_children.end(); ++playlistModel_iter) {
    Gtk::TreeModel::Row playlistModel_row = *playlistModel_iter;
    
    if (playlistModel_row[(*Dingo::DBManager::playlistCR).playlistID] == playlist_id) {
      playlistModel_row[(*Dingo::DBManager::playlistCR).playlistName] = Dingo::Utilities::encodeFromEntityString(playlist_name_new);
      break;
    }
  }
  
  //rename the playlist inside Dingo::DBManager::trackplaylistModel
  Gtk::TreeModel::Children trackplaylistModel_children = Dingo::DBManager::trackplaylistModel->children();
  
  for (Gtk::TreeModel::Children::iterator trackplaylistModel_iter = trackplaylistModel_children.begin(); trackplaylistModel_iter != trackplaylistModel_children.end(); ++trackplaylistModel_iter) {
    Gtk::TreeModel::Row trackplaylistModel_row = *trackplaylistModel_iter;
    
    if (trackplaylistModel_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistID] == playlist_id) {
      trackplaylistModel_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistName] = Dingo::Utilities::encodeFromEntityString(playlist_name_new);
      break;
    }
  }
}

void Dingo::DBManager::exportPlaylist(Glib::ustring fileuri, int playlist_id, Glib::ustring playlist_name) {
  Dingo::PlaylistWriter temp_playlistwriter;
  int playlist_size = 0;
  
  d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracksPlaylists WHERE CoreTracksPlaylists.PlaylistID = :PlaylistID");
  
  d_sqlite3.bindParaInt(":PlaylistID", playlist_id);
  
  if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    playlist_size = d_sqlite3.getIntValue(0);
  }
  
  d_sqlite3.finalizeStatement();
  
  //set processgrid & processid_queue
  d_processid_queue[Dingo::PROCESS_EXPORTING_PLAYLIST] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_EXPORTING_PLAYLIST);
  
  int exported_files_count = 0;
  
  if (!d_processid_queue[Dingo::PROCESS_EXPORTING_PLAYLIST]) {
    return;
  }
    
  updateGUI();
  
  //continue to process normally
  temp_playlistwriter.open(Dingo::Utilities::addFileExtension(fileuri, ".dpls"), playlist_name, playlist_size);

  d_sqlite3.prepareStatement("SELECT CoreArtists.ArtistName, CoreAlbums.AlbumName, CoreTracks.TrackNumber, CoreTracks.TrackTitle, CoreGenres.GenreName, CoreTracks.TrackTime, CoreTracks.TrackSize, CoreTracks.TrackFileName, CoreTracks.TrackFileExtension, CoreTracks.TrackURI, CoreTracks.TrackSubtitleURI, CoreYears.YearName, CoreTracks.TrackBitrate, CoreTracks.TrackMIMEType, CoreTracks.TrackDateAdded, CoreTracks.TrackComment, CoreTracks.TrackLyrics FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreYears ON CoreTracks.YearID = CoreYears.YearID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID WHERE CoreTracksPlaylists.PlaylistID = :PlaylistID ORDER BY CoreTracks.TrackTitle;");
  
  d_sqlite3.bindParaInt(":PlaylistID", playlist_id);
  
  while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    if (!d_processid_queue[Dingo::PROCESS_EXPORTING_PLAYLIST]) {
      break;
    }
    
    ++exported_files_count;
    
    if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_EXPORTING_PLAYLIST) {
      d_processgrid->setTotalFilesCount(playlist_size);
      
      d_processgrid->setCurrentFilesCount(exported_files_count);
      
      d_processgrid->setCurrentFileName(d_sqlite3.getStringValue(7));
    }
  
    temp_playlistwriter.createTrack();
    
    temp_playlistwriter.setTrackArtist(d_sqlite3.getStringValue(0));
    temp_playlistwriter.setTrackAlbum(d_sqlite3.getStringValue(1));
    temp_playlistwriter.setTrackNumber(d_sqlite3.getIntValue(2));
    temp_playlistwriter.setTrackTitle(d_sqlite3.getStringValue(3));
    temp_playlistwriter.setTrackGenre(d_sqlite3.getStringValue(4));
    temp_playlistwriter.setTrackDuration(d_sqlite3.getInt64Value(5));
    temp_playlistwriter.setTrackSize(d_sqlite3.getInt64Value(6));
    temp_playlistwriter.setTrackFileName(d_sqlite3.getStringValue(7));
    temp_playlistwriter.setTrackFileExtension(d_sqlite3.getStringValue(8));
    temp_playlistwriter.setTrackURI(d_sqlite3.getStringValue(9));
    temp_playlistwriter.setTrackSubtitleURI(d_sqlite3.getStringValue(10));
    temp_playlistwriter.setTrackYear(d_sqlite3.getStringValue(11));
    temp_playlistwriter.setTrackBitrate(d_sqlite3.getIntValue(12));
    temp_playlistwriter.setTrackMIMEType(d_sqlite3.getStringValue(13));
    temp_playlistwriter.setTrackDateAdded(d_sqlite3.getStringValue(14));
    temp_playlistwriter.setTrackComment(d_sqlite3.getStringValue(15));
    temp_playlistwriter.setTrackLyrics(d_sqlite3.getStringValue(16));
    
    temp_playlistwriter.saveTrack();
  }
  
  temp_playlistwriter.writeToFile();
  
  d_sqlite3.finalizeStatement();
    
  updateGUI();
  
  d_processid_queue[Dingo::PROCESS_EXPORTING_PLAYLIST] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_EXPORTING_PLAYLIST) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }
}

void Dingo::DBManager::deletePlaylist(int playlist_id) {
  d_sqlite3.prepareStatement("DELETE FROM CorePlaylists WHERE CorePlaylists.PlaylistID = :PlaylistID;");
  
  d_sqlite3.bindParaInt(":PlaylistID", playlist_id);
  
  if (d_sqlite3.stepStatement() != Dingo::SQLite::ERROR) {
    //std::cout << "Playlist Deletion Succeeded!" << std::endl;
  }
  
  else {
    //std::cout << "Playlist Deletion Failed!" << std::endl;
  }
  
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.prepareStatement("DELETE FROM CoreTracksPlaylists WHERE CoreTracksPlaylists.PlaylistID = :PlaylistID;");
  
  d_sqlite3.bindParaInt(":PlaylistID", playlist_id);
  
  if (d_sqlite3.stepStatement() != Dingo::SQLite::ERROR) {
    //std::cout << "TracksPlaylists Deletion Succeeded!" << std::endl;
  }
  
  else {
    //std::cout << "TracksPlaylists Deletion Failed!" << std::endl;
  }
  
  d_sqlite3.finalizeStatement();
  
  //delete playlist inside Dingo::DBManager::playlistModel
  Gtk::TreeModel::Children playlistModel_children = Dingo::DBManager::playlistModel->children();
  
  for (Gtk::TreeModel::Children::iterator playlistModel_iter = playlistModel_children.begin(); playlistModel_iter != playlistModel_children.end(); ++playlistModel_iter) {
    Gtk::TreeModel::Row playlistModel_row = *playlistModel_iter;
    
    if (playlistModel_row[(*Dingo::DBManager::playlistCR).playlistID] == playlist_id) {
      Dingo::DBManager::playlistModel->erase(playlistModel_iter);
      break;
    }
  }
  
  //delete playlist inside Dingo::DBManager::trackplaylistModel
  Gtk::TreeModel::Children trackplaylistModel_children = Dingo::DBManager::trackplaylistModel->children();
  
  for (Gtk::TreeModel::Children::iterator trackplaylistModel_iter = trackplaylistModel_children.begin(); trackplaylistModel_iter != trackplaylistModel_children.end(); ++trackplaylistModel_iter) {
    Gtk::TreeModel::Row trackplaylistModel_row = *trackplaylistModel_iter;
    
    if (trackplaylistModel_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistID] == playlist_id) {
      Dingo::DBManager::trackplaylistModel->erase(trackplaylistModel_iter);
      break;
    }
  }
  
  //emit "PlaylistCount" property changed signal
  Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYLISTS, "PlaylistCount");
}

void Dingo::DBManager::prepareQueryCountTracksInPlaylist() {
  d_sqlite3.quickExecute("BEGIN TRANSACTION;");

  d_sqlite3.prepareStatement("SELECT COUNT(*) FROM (SELECT CoreTracks.TrackID FROM CoreTracks INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID WHERE CoreTracksPlaylists.PlaylistID = :PlaylistID);");
  
  //d_sqlite3.bindParaInt(":PlaylistID", playlist_id);
}

int Dingo::DBManager::getCountTracksInPlaylist(int playlist_id) {
  int playlist_tracks_count = 0;
  
  d_sqlite3.bindParaInt(":PlaylistID", playlist_id);
  
  if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    playlist_tracks_count = d_sqlite3.getIntValue(0);
  }
  
  d_sqlite3.clearStatementBindings();
  
  d_sqlite3.resetStatement();
  
  //std::cout << "Tracks Counted: " << playlist_tracks_count << std::endl;
  
  return playlist_tracks_count;
}

void Dingo::DBManager::finalizeQueryCountTracksInPlaylist() {
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
}

//Dingo::GenreTreeView's METHODS
void Dingo::DBManager::setCurrentGenresID(std::vector<Gtk::TreeRowReference> ref_vector) {
  cur_genres_ref = ref_vector;
  
  if (cur_genres_ref.empty()) {
    is_all_genres_selected = true;
    
    //reset genres_selected_query
    genres_selected_query = "";
  }
  
  else {
    is_all_genres_selected = false;
    
    //build genres_selected_query
    genres_selected_query = " AND CoreTracks.GenreID IN (";
    
    for (unsigned int i = 0; i != cur_genres_ref.size(); ++i) {
      genres_selected_query += ":Genre" + Dingo::Utilities::intToString(i);
      
      if (i == cur_genres_ref.size() - 1) {
        genres_selected_query += ")";
      }
      
      else {
        genres_selected_query += ", ";
      }
    }
  }
  
  //std::cout << "genres_selected_query: " << genres_selected_query << std::endl;
}

void Dingo::DBManager::prepareQueryCountTracksInGenre() {
  updateGUI();
  
  d_sqlite3.quickExecute("BEGIN TRANSACTION;");

  if (cur_playlist_id == 0) {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID WHERE CoreTracks.GenreID = :GenreID AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracks.TrackMIMEType = :TrackMIMEType;");
      
      //d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    else {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID WHERE CoreTracks.GenreID = :GenreID AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText);");
    }
  }
  
  else {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID WHERE CoreTracks.GenreID = :GenreID AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracksPlaylists.PlaylistID = :PlaylistID AND CoreTracks.TrackMIMEType = :TrackMIMEType;");
      
      //d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    else {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID WHERE CoreTracks.GenreID = :GenreID AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracksPlaylists.PlaylistID = :PlaylistID;");
    }
    
    //d_sqlite3.bindParaInt(":PlaylistID", cur_playlist_id);
  }
}

int Dingo::DBManager::getCountTracksInGenre(int genre_id) {
  int genre_tracks_count = 0;
  
  if (cur_playlist_id == 0) {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
  }
  
  else {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    d_sqlite3.bindParaInt(":PlaylistID", cur_playlist_id);
  }
  
  d_sqlite3.bindParaInt(":GenreID", genre_id);
  d_sqlite3.bindParaText(":SearchText", "%" + d_search_text + "%");
    
  if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    genre_tracks_count = d_sqlite3.getIntValue(0);
  }
    
  d_sqlite3.clearStatementBindings();
  
  d_sqlite3.resetStatement();
  
  return genre_tracks_count;
}

void Dingo::DBManager::finalizeQueryCountTracksInGenre() {
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
}
      
//Dingo::ArtistTreeView's METHODS
void Dingo::DBManager::setCurrentArtistsID(std::vector<Gtk::TreeRowReference> ref_vector) {
  cur_artists_ref = ref_vector;
  
  if (cur_artists_ref.empty()) {
    is_all_artists_selected = true;
    
    //reset artists_selected_query
    artists_selected_query = "";
  }
  
  else {
    is_all_artists_selected = false;
   
    //build artists_selected_query       
    artists_selected_query = " AND CoreTracks.ArtistID IN (";
    
    for (unsigned int i = 0; i != cur_artists_ref.size(); ++i) {
      artists_selected_query += ":Artist" + Dingo::Utilities::intToString(i);
        
      if (i == cur_artists_ref.size() - 1) {
        artists_selected_query += ")";
      }
        
      else {
        artists_selected_query += ", ";
      }
    }
  }
    
  //std::cout << "artists_selected_query: " << artists_selected_query << std::endl;
}

void Dingo::DBManager::prepareQueryCountTracksInArtist() {
  updateGUI();
  
  d_sqlite3.quickExecute("BEGIN TRANSACTION;");

  if (cur_playlist_id == 0) {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID WHERE CoreTracks.ArtistID = :ArtistID " + genres_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracks.TrackMIMEType = :TrackMIMEType;");
      
      //d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    else {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID WHERE CoreTracks.ArtistID = :ArtistID " + genres_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText);");
    }
  }
  
  else {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID WHERE CoreTracks.ArtistID = :ArtistID " + genres_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracksPlaylists.PlaylistID = :PlaylistID AND CoreTracks.TrackMIMEType = :TrackMIMEType;");
      
      //d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    else {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID WHERE CoreTracks.ArtistID = :ArtistID " + genres_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracksPlaylists.PlaylistID = :PlaylistID;");
    }
    
    //d_sqlite3.bindParaInt(":PlaylistID", cur_playlist_id);
  }
}

int Dingo::DBManager::getCountTracksInArtist(int artist_id) {
  int artist_tracks_count = 0;
  
  if (cur_playlist_id == 0) {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
  }
  
  else {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    d_sqlite3.bindParaInt(":PlaylistID", cur_playlist_id);
  }
  
  d_sqlite3.bindParaInt(":ArtistID", artist_id);
  d_sqlite3.bindParaText(":SearchText", "%" + d_search_text + "%");
    
  for (unsigned int i = 0; i != cur_genres_ref.size(); ++i) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::genreModel->get_iter(cur_genres_ref[i].get_path()));
      
    d_sqlite3.bindParaInt(":Genre" + Dingo::Utilities::intToString(i), row[(*Dingo::DBManager::genreCR).genreID]);
  }
  
  if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    artist_tracks_count = d_sqlite3.getIntValue(0);
  }
    
  d_sqlite3.clearStatementBindings();
  
  d_sqlite3.resetStatement();
  
  return artist_tracks_count;
}

void Dingo::DBManager::finalizeQueryCountTracksInArtist() {
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
}

//Dingo::YearTreeView's METHODS
void Dingo::DBManager::setCurrentYearsID(std::vector<Gtk::TreeRowReference> ref_vector) {
  cur_years_ref = ref_vector;
  
  if (cur_years_ref.empty()) {
    is_all_years_selected = true;
    
    //reset years_selected_query
    years_selected_query = "";
  } 
  
  else {
    is_all_years_selected = false;
    
    //build years_selected_query
    years_selected_query = " AND CoreTracks.YearID IN (";
    
    for (unsigned int i = 0; i != cur_years_ref.size(); ++i) {
      years_selected_query += ":Year" + Dingo::Utilities::intToString(i);
      
      if (i == cur_years_ref.size() - 1) {
        years_selected_query += ")";
      }
      
      else {
        years_selected_query += ", ";
      }
    }
  }
  
  //std::cout << "years_selected_query: " << artists_selected_query << std::endl;
}

void Dingo::DBManager::prepareQueryCountTracksInYear() {
  updateGUI();
  
  d_sqlite3.quickExecute("BEGIN TRANSACTION;");

  if (cur_playlist_id == 0) {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID WHERE CoreTracks.YearID = :YearID " + genres_selected_query + artists_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracks.TrackMIMEType = :TrackMIMEType;");
      
      //d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    else {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID WHERE CoreTracks.YearID = :YearID " + genres_selected_query + artists_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText);");
    }
  }
  
  else {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID WHERE CoreTracks.YearID = :YearID " + genres_selected_query + artists_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracksPlaylists.PlaylistID = :PlaylistID AND CoreTracks.TrackMIMEType = :TrackMIMEType;");
      
      //d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    else {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID WHERE CoreTracks.YearID = :YearID " + genres_selected_query + artists_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracksPlaylists.PlaylistID = :PlaylistID;");
    }
    
    //d_sqlite3.bindParaInt(":PlaylistID", cur_playlist_id);
  }
}

int Dingo::DBManager::getCountTracksInYear(int year_id) {
  int year_tracks_count = 0;
  
  if (cur_playlist_id == 0) {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
  }
  
  else {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    d_sqlite3.bindParaInt(":PlaylistID", cur_playlist_id);
  }
  
  d_sqlite3.bindParaInt(":YearID", year_id);
  d_sqlite3.bindParaText(":SearchText", "%" + d_search_text + "%");
    
  for (unsigned int i = 0; i != cur_genres_ref.size(); ++i) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::genreModel->get_iter(cur_genres_ref[i].get_path()));
      
    d_sqlite3.bindParaInt(":Genre" + Dingo::Utilities::intToString(i), row[(*Dingo::DBManager::genreCR).genreID]);
  }
  
  for (unsigned int i = 0; i != cur_artists_ref.size(); ++i) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::artistModel->get_iter(cur_artists_ref[i].get_path()));
      
    d_sqlite3.bindParaInt(":Artist" + Dingo::Utilities::intToString(i), row[(*Dingo::DBManager::artistCR).artistID]);
  }
  
  if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    year_tracks_count = d_sqlite3.getIntValue(0);
  }
    
  d_sqlite3.clearStatementBindings();
  
  d_sqlite3.resetStatement();
  
  return year_tracks_count;
}

void Dingo::DBManager::finalizeQueryCountTracksInYear() {
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
}

//Dingo::AlbumTreeView's METHODS
void Dingo::DBManager::setCurrentAlbumsID(std::vector<Gtk::TreeRowReference> ref_vector) {
  cur_albums_ref = ref_vector;
  
  if (cur_albums_ref.empty()) {
    is_all_albums_selected = true;
    
    //reset artists_selected_query
    albums_selected_query = "";
  }
  
  else {
    is_all_albums_selected = false;
   
    //build artists_selected_query       
    albums_selected_query = " AND CoreTracks.AlbumID IN (";
    
    for (unsigned int i = 0; i != cur_albums_ref.size(); ++i) {
      albums_selected_query += ":Album" + Dingo::Utilities::intToString(i);
        
      if (i == cur_albums_ref.size() - 1) {
        albums_selected_query += ")";
      }
        
      else {
        albums_selected_query += ", ";
      }
    }
  }
}

void Dingo::DBManager::prepareQueryCountTracksInAlbum() {
  updateGUI();
  
  d_sqlite3.quickExecute("BEGIN TRANSACTION;");

  if (cur_playlist_id == 0) {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID WHERE CoreTracks.AlbumID = :AlbumID " + genres_selected_query + artists_selected_query + years_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracks.TrackMIMEType = :TrackMIMEType;");
    
      //d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    else {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID WHERE CoreTracks.AlbumID = :AlbumID " + genres_selected_query + artists_selected_query + years_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText);");
    }
  }

  else {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID WHERE CoreTracks.AlbumID = :AlbumID " + genres_selected_query + artists_selected_query + years_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracksPlaylists.PlaylistID = :PlaylistID AND CoreTracks.TrackMIMEType = :TrackMIMEType;");
    
      //d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    else {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID WHERE CoreTracks.AlbumID = :AlbumID " + genres_selected_query + artists_selected_query + years_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracksPlaylists.PlaylistID = :PlaylistID;");
    }
    
    //d_sqlite3.bindParaInt(":PlaylistID", cur_playlist_id);
  }
}

int Dingo::DBManager::getCountTracksInAlbum(int album_id) {
  int album_tracks_count = 0;
  
  if (cur_playlist_id == 0) {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
  }

  else {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    d_sqlite3.bindParaInt(":PlaylistID", cur_playlist_id);
  }
  
  d_sqlite3.bindParaInt(":AlbumID", album_id);
  d_sqlite3.bindParaText(":SearchText", "%" + d_search_text + "%");
  
  for (unsigned int i = 0; i != cur_genres_ref.size(); ++i) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::genreModel->get_iter(cur_genres_ref[i].get_path()));
      
    d_sqlite3.bindParaInt(":Genre" + Dingo::Utilities::intToString(i), row[(*Dingo::DBManager::genreCR).genreID]);
  }
  
  for (unsigned int i = 0; i != cur_artists_ref.size(); ++i) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::artistModel->get_iter(cur_artists_ref[i].get_path()));
      
    d_sqlite3.bindParaInt(":Artist" + Dingo::Utilities::intToString(i), row[(*Dingo::DBManager::artistCR).artistID]);
  }
  
  for (unsigned int i = 0; i != cur_years_ref.size(); ++i) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::yearModel->get_iter(cur_years_ref[i].get_path()));
    
    d_sqlite3.bindParaInt(":Year" + Dingo::Utilities::intToString(i), row[(*Dingo::DBManager::yearCR).yearID]);
  }

  if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    album_tracks_count = d_sqlite3.getIntValue(0);
  }
    
  d_sqlite3.clearStatementBindings();
  
  d_sqlite3.resetStatement();
  
  return album_tracks_count;
}

void Dingo::DBManager::finalizeQueryCountTracksInAlbum() {
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
}
      
//Dingo::TrackTreeView's METHODS
void Dingo::DBManager::addTracksToPlayQueue(std::vector<int> add_tracks_id_vector) {
  //set processgrid & processid_queue
  d_processid_queue[Dingo::PROCESS_ADDING_TO_PLAYQUEUE] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_ADDING_TO_PLAYQUEUE);

  updateGUI();
  
  if (!d_processid_queue[Dingo::PROCESS_ADDING_TO_PLAYQUEUE]) {
    return;
  }

  d_sqlite3.quickExecute("BEGIN TRANSACTION;");

  d_sqlite3.prepareStatement("INSERT INTO CoreTracksPlaylists (TrackID, PlaylistID) VALUES (:TrackID, :PlaylistID);");
  
  for (unsigned int i = 0; i != add_tracks_id_vector.size(); ++i) {
    if (!d_processid_queue[Dingo::PROCESS_ADDING_TO_PLAYQUEUE]) {
      break;
    }
  
    d_sqlite3.bindParaInt(":TrackID", add_tracks_id_vector[i]);
    d_sqlite3.bindParaInt(":PlaylistID", 1);
    
    d_sqlite3.stepStatement();
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
  updateGUI();
  
  d_processid_queue[Dingo::PROCESS_ADDING_TO_PLAYQUEUE] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_ADDING_TO_PLAYQUEUE) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }
}

void Dingo::DBManager::addTracksToNormalPlaylist(std::vector<int> add_tracks_id_vector, int playlist_id) {
  //set processgrid & processid_queue
  d_processid_queue[Dingo::PROCESS_ADDING_TO_PLAYLISTS] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_ADDING_TO_PLAYLISTS);

  updateGUI();
  
  if (!d_processid_queue[Dingo::PROCESS_ADDING_TO_PLAYLISTS]) {
    return;
  }

  d_sqlite3.quickExecute("BEGIN TRANSACTION;");
  
  d_sqlite3.prepareStatement("INSERT INTO CoreTracksPlaylists (TrackID, PlaylistID) VALUES (:TrackID, :PlaylistID);");
  
  for (unsigned int i = 0; i != add_tracks_id_vector.size(); ++i) {
    if (!d_processid_queue[Dingo::PROCESS_ADDING_TO_PLAYLISTS]) {
      break;
    }
  
    d_sqlite3.bindParaInt(":TrackID", add_tracks_id_vector[i]);
    d_sqlite3.bindParaInt(":PlaylistID", playlist_id);
    
    d_sqlite3.stepStatement();
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
  updateGUI();
  
  d_processid_queue[Dingo::PROCESS_ADDING_TO_PLAYLISTS] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_ADDING_TO_PLAYLISTS) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }
}

void Dingo::DBManager::deleteTracksFromDatabase(std::vector<int> delete_tracks_id_vector) {
  //set processgrid & processid_queue
  d_processid_queue[Dingo::PROCESS_DELETING_FROM_DATABASE] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_DELETING_FROM_DATABASE);

  updateGUI();
  
  if (!d_processid_queue[Dingo::PROCESS_DELETING_FROM_DATABASE]) {
    return;
  }

  d_sqlite3.quickExecute("BEGIN TRANSACTION;");
  
  d_sqlite3.prepareStatement("DELETE FROM CoreTracks WHERE CoreTracks.TrackID = :TrackID;");
  
  for (unsigned int i = 0; i != delete_tracks_id_vector.size(); ++i) {
    if (!d_processid_queue[Dingo::PROCESS_DELETING_FROM_DATABASE]) {
      break;
    }
  
    d_sqlite3.bindParaInt(":TrackID", delete_tracks_id_vector[i]);
    
    d_sqlite3.stepStatement();
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.prepareStatement("DELETE FROM CoreTracksPlaylists WHERE CoreTracksPlaylists.TrackID = :TrackID;");
  
  for (unsigned int i = 0; i != delete_tracks_id_vector.size(); ++i) {
    if (!d_processid_queue[Dingo::PROCESS_DELETING_FROM_DATABASE]) {
      break;
    }
  
    d_sqlite3.bindParaInt(":TrackID", delete_tracks_id_vector[i]);
    
    d_sqlite3.stepStatement();
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
  updateGUI();
  
  d_processid_queue[Dingo::PROCESS_DELETING_FROM_DATABASE] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_DELETING_FROM_DATABASE) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }
}

void Dingo::DBManager::deleteTracksFromPlayQueue(std::vector<int> remove_tracks_id_vector) {
  //set processgrid & processid_queue
  d_processid_queue[Dingo::PROCESS_DELETING_FROM_PLAYQUEUE] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_DELETING_FROM_PLAYQUEUE);

  updateGUI();
  
  if (!d_processid_queue[Dingo::PROCESS_DELETING_FROM_PLAYQUEUE]) {
    return;
  }

  d_sqlite3.quickExecute("BEGIN TRANSACTION;");
  
  d_sqlite3.prepareStatement("DELETE FROM CoreTracksPlaylists WHERE CoreTracksPlaylists.PlaylistID = :PlaylistID AND CoreTracksPlaylists.TrackID = :TrackID;");
  
  for (unsigned int i = 0; i != remove_tracks_id_vector.size(); ++i) {
    if (!d_processid_queue[Dingo::PROCESS_DELETING_FROM_PLAYQUEUE]) {
      break;
    }
  
    d_sqlite3.bindParaInt(":PlaylistID", 1);
    d_sqlite3.bindParaInt(":TrackID", remove_tracks_id_vector[i]);
    
    d_sqlite3.stepStatement();
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
  updateGUI();
  
  d_processid_queue[Dingo::PROCESS_DELETING_FROM_PLAYQUEUE] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_DELETING_FROM_PLAYQUEUE) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }
}

void Dingo::DBManager::deleteTracksFromHardDrive(std::vector<int> delete_tracks_id_vector) {
  //set processgrid & processid_queue
  d_processid_queue[Dingo::PROCESS_DELETING_FROM_HARDDRIVE] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_DELETING_FROM_HARDDRIVE);

  updateGUI();
  
  if (!d_processid_queue[Dingo::PROCESS_DELETING_FROM_HARDDRIVE]) {
    return;
  }

  d_sqlite3.quickExecute("BEGIN TRANSACTION;");

  d_sqlite3.prepareStatement("SELECT CoreTracks.TrackURI FROM CoreTracks WHERE CoreTracks.TrackID = :TrackID;");

  for (unsigned int i = 0; i != delete_tracks_id_vector.size(); ++i) {
    if (!d_processid_queue[Dingo::PROCESS_DELETING_FROM_HARDDRIVE]) {
      break;
    }
  
    d_sqlite3.bindParaInt(":TrackID", delete_tracks_id_vector[i]);
    
    if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      Glib::RefPtr<Gio::File> temp_file = Gio::File::create_for_uri(d_sqlite3.getStringValue(0));
    
      temp_file->remove();
    }
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();

  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
  updateGUI();
  
  d_processid_queue[Dingo::PROCESS_DELETING_FROM_HARDDRIVE] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_DELETING_FROM_HARDDRIVE) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }

  //Now Delete from Database
  deleteTracksFromDatabase(delete_tracks_id_vector);
}

void Dingo::DBManager::deleteTracksFromNormalPlaylist(std::vector<int> remove_tracks_id_vector, int playlist_id) {
  //set processgrid & processid_queue
  d_processid_queue[Dingo::PROCESS_DELETING_FROM_PLAYLISTS] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_DELETING_FROM_PLAYLISTS);

  updateGUI();
  
  if (!d_processid_queue[Dingo::PROCESS_DELETING_FROM_PLAYLISTS]) {
    return;
  }

  d_sqlite3.quickExecute("BEGIN TRANSACTION;");
  
  d_sqlite3.prepareStatement("DELETE FROM CoreTracksPlaylists WHERE CoreTracksPlaylists.PlaylistID = :PlaylistID AND CoreTracksPlaylists.TrackID = :TrackID;");
  
  for (unsigned int i = 0; i != remove_tracks_id_vector.size(); ++i) {
    if (!d_processid_queue[Dingo::PROCESS_DELETING_FROM_PLAYLISTS]) {
      break;
    }
  
    d_sqlite3.bindParaInt(":PlaylistID", playlist_id);
    d_sqlite3.bindParaInt(":TrackID", remove_tracks_id_vector[i]);
    
    d_sqlite3.stepStatement();
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
  updateGUI();
  
  d_processid_queue[Dingo::PROCESS_DELETING_FROM_PLAYLISTS] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_DELETING_FROM_PLAYLISTS) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }
}

void Dingo::DBManager::cutTracks(std::vector<int> clipboard_tracks_id_vector) {
  d_clipboard_tracks_id_vector = clipboard_tracks_id_vector;

  //set processgrid & processid_queue
  d_processid_queue[Dingo::PROCESS_CUTTING_TRACKS] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_CUTTING_TRACKS);

  updateGUI();
  
  if (!d_processid_queue[Dingo::PROCESS_CUTTING_TRACKS]) {
    return;
  }

  d_sqlite3.quickExecute("BEGIN TRANSACTION;");

  d_sqlite3.prepareStatement("DELETE FROM CoreTracksPlaylists WHERE CoreTracksPlaylists.PlaylistID = :PlaylistID AND CoreTracksPlaylists.TrackID = :TrackID;");
  
  for (unsigned int i = 0; i != d_clipboard_tracks_id_vector.size(); ++i) {
    if (!d_processid_queue[Dingo::PROCESS_CUTTING_TRACKS]) {
      break;
    }
  
    d_sqlite3.bindParaInt(":PlaylistID", cur_playlist_id);
    d_sqlite3.bindParaInt(":TrackID", d_clipboard_tracks_id_vector[i]);
    
    d_sqlite3.stepStatement();
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
  updateGUI();
  
  d_processid_queue[Dingo::PROCESS_CUTTING_TRACKS] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_CUTTING_TRACKS) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }
}

void Dingo::DBManager::copyTracks(std::vector<int> clipboard_tracks_id_vector) {  
  //set processgrid & processid_queue
  d_processid_queue[Dingo::PROCESS_COPYING_TRACKS] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_COPYING_TRACKS);
  
  updateGUI();
  
  if (!d_processid_queue[Dingo::PROCESS_COPYING_TRACKS]) {
    return;
  }

  d_clipboard_tracks_id_vector = clipboard_tracks_id_vector;
  
  updateGUI();
  
  d_processid_queue[Dingo::PROCESS_COPYING_TRACKS] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_COPYING_TRACKS) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }
}

void Dingo::DBManager::pasteTracks() {
  //set processgrid & processid_queue
  d_processid_queue[Dingo::PROCESS_PASTING_TRACKS] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_PASTING_TRACKS);
  
  updateGUI();
  
  if (!d_processid_queue[Dingo::PROCESS_PASTING_TRACKS]) {
    return;
  }

  d_sqlite3.quickExecute("BEGIN TRANSACTION;");
  
  d_sqlite3.prepareStatement("INSERT INTO CoreTracksPlaylists (TrackID, PlaylistID) VALUES (:TrackID, :PlaylistID);");
  
  for (unsigned int i = 0; i != d_clipboard_tracks_id_vector.size(); ++i) {
    if (!d_processid_queue[Dingo::PROCESS_PASTING_TRACKS]) {
      break;
    }
  
    d_sqlite3.bindParaInt(":TrackID", d_clipboard_tracks_id_vector[i]);
    d_sqlite3.bindParaInt(":PlaylistID", cur_playlist_id);
    
    d_sqlite3.stepStatement();
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
  updateGUI();
  
  d_processid_queue[Dingo::PROCESS_PASTING_TRACKS] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_PASTING_TRACKS) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }
}

void Dingo::DBManager::prepareQueryShouldTrackBeVisible() {
  updateGUI();
  
  d_sqlite3.quickExecute("BEGIN TRANSACTION;");

  if (cur_playlist_id == 0) {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.prepareStatement("SELECT CoreTracks.TrackID, CoreTracks.TrackTime, CoreTracks.TrackSize FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID WHERE CoreTracks.TrackID = :TrackID " + genres_selected_query + artists_selected_query + years_selected_query + albums_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracks.TrackMIMEType = :TrackMIMEType;");
      
      //d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    else {
      d_sqlite3.prepareStatement("SELECT CoreTracks.TrackID, CoreTracks.TrackTime, CoreTracks.TrackSize FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID WHERE CoreTracks.TrackID = :TrackID " + genres_selected_query + artists_selected_query + years_selected_query + albums_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText);");
    }
  }

  else {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.prepareStatement("SELECT CoreTracks.TrackID, CoreTracks.TrackTime, CoreTracks.TrackSize FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID WHERE CoreTracks.TrackID = :TrackID " + genres_selected_query + artists_selected_query + years_selected_query + albums_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracksPlaylists.PlaylistID = :PlaylistID AND CoreTracks.TrackMIMEType = :TrackMIMEType;");
      
      //d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    else {
      d_sqlite3.prepareStatement("SELECT CoreTracks.TrackID, CoreTracks.TrackTime, CoreTracks.TrackSize FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID WHERE CoreTracks.TrackID = :TrackID " + genres_selected_query + artists_selected_query + years_selected_query + albums_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracksPlaylists.PlaylistID = :PlaylistID;");
    }
    
    //d_sqlite3.bindParaInt(":PlaylistID", cur_playlist_id);
  }
}

bool Dingo::DBManager::getShouldTrackBeVisible(int track_id) {
  bool should_visible = false;
  
  if (cur_playlist_id == 0) {
    if (d_mimetype_filter_str != "%%") {      
      d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
  }

  else {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    d_sqlite3.bindParaInt(":PlaylistID", cur_playlist_id);
  }
  
  d_sqlite3.bindParaInt(":TrackID", track_id);
  d_sqlite3.bindParaText(":SearchText", "%" + d_search_text + "%");
  
  for (unsigned int i = 0; i != cur_genres_ref.size(); ++i) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::genreModel->get_iter(cur_genres_ref[i].get_path()));
      
    d_sqlite3.bindParaInt(":Genre" + Dingo::Utilities::intToString(i), row[(*Dingo::DBManager::genreCR).genreID]);
  }
  
  for (unsigned int i = 0; i != cur_artists_ref.size(); ++i) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::artistModel->get_iter(cur_artists_ref[i].get_path()));
      
    d_sqlite3.bindParaInt(":Artist" + Dingo::Utilities::intToString(i), row[(*Dingo::DBManager::artistCR).artistID]);
  }
  
  for (unsigned int i = 0; i != cur_years_ref.size(); ++i) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::yearModel->get_iter(cur_years_ref[i].get_path()));
    
    d_sqlite3.bindParaInt(":Year" + Dingo::Utilities::intToString(i), row[(*Dingo::DBManager::yearCR).yearID]);
  }
  
  for (unsigned int i = 0; i != cur_albums_ref.size(); ++i) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::albumModel->get_iter(cur_albums_ref[i].get_path()));
    
    d_sqlite3.bindParaInt(":Album" + Dingo::Utilities::intToString(i), row[(*Dingo::DBManager::albumCR).albumID]);
  }
  
  if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    should_visible = true;
  }
    
  d_sqlite3.clearStatementBindings();
  
  d_sqlite3.resetStatement();
  
  return should_visible;
}

void Dingo::DBManager::finalizeQueryShouldTrackBeVisible() {
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
}

Glib::ustring Dingo::DBManager::getPlaylistInfo() {
  int cur_playlist_total_tracks = 0;
  gint64 cur_playlist_total_time = 0;
  gint64 cur_playlist_total_size = 0;
  
  if (cur_playlist_id == 0) {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.prepareStatement("SELECT COUNT(CoreTracks.TrackID), SUM(CoreTracks.TrackTime), SUM(CoreTracks.TrackSize) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID WHERE CoreTracks.TrackMIMEType = :TrackMIMEType " + genres_selected_query + artists_selected_query + years_selected_query + albums_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText);");
      
      d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    else {
      d_sqlite3.prepareStatement("SELECT COUNT(CoreTracks.TrackID), SUM(CoreTracks.TrackTime), SUM(CoreTracks.TrackSize) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID WHERE (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) " + genres_selected_query + artists_selected_query + years_selected_query + albums_selected_query + ";");
    }
  }

  else {
    if (d_mimetype_filter_str != "%%") {
      d_sqlite3.prepareStatement("SELECT COUNT(CoreTracks.TrackID), SUM(CoreTracks.TrackTime), SUM(CoreTracks.TrackSize) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID WHERE CoreTracks.TrackMIMEType = :TrackMIMEType " + genres_selected_query + artists_selected_query + years_selected_query + albums_selected_query + " AND (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) AND CoreTracksPlaylists.PlaylistID = :PlaylistID;");
      
      d_sqlite3.bindParaText(":TrackMIMEType", d_mimetype_filter_str);
    }
    
    else {
      d_sqlite3.prepareStatement("SELECT COUNT(CoreTracks.TrackID), SUM(CoreTracks.TrackTime), SUM(CoreTracks.TrackSize) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID WHERE (CoreTracks.TrackTitle LIKE :SearchText OR CoreGenres.GenreName LIKE :SearchText OR CoreArtists.ArtistName LIKE :SearchText OR CoreAlbums.AlbumName LIKE :SearchText) " + genres_selected_query + artists_selected_query + years_selected_query + albums_selected_query + " AND CoreTracksPlaylists.PlaylistID = :PlaylistID;");
    }
    
    d_sqlite3.bindParaInt(":PlaylistID", cur_playlist_id);
  }
  
  d_sqlite3.bindParaText(":SearchText", "%" + d_search_text + "%");
  
  for (unsigned int i = 0; i != cur_genres_ref.size(); ++i) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::genreModel->get_iter(cur_genres_ref[i].get_path()));
      
    d_sqlite3.bindParaInt(":Genre" + Dingo::Utilities::intToString(i), row[(*Dingo::DBManager::genreCR).genreID]);
  }
  
  for (unsigned int i = 0; i != cur_artists_ref.size(); ++i) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::artistModel->get_iter(cur_artists_ref[i].get_path()));
      
    d_sqlite3.bindParaInt(":Artist" + Dingo::Utilities::intToString(i), row[(*Dingo::DBManager::artistCR).artistID]);
  }
  
  for (unsigned int i = 0; i != cur_years_ref.size(); ++i) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::yearModel->get_iter(cur_years_ref[i].get_path()));
    
    d_sqlite3.bindParaInt(":Year" + Dingo::Utilities::intToString(i), row[(*Dingo::DBManager::yearCR).yearID]);
  }
  
  for (unsigned int i = 0; i != cur_albums_ref.size(); ++i) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::albumModel->get_iter(cur_albums_ref[i].get_path()));
    
    d_sqlite3.bindParaInt(":Album" + Dingo::Utilities::intToString(i), row[(*Dingo::DBManager::albumCR).albumID]);
  }
  
  if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    cur_playlist_total_tracks = d_sqlite3.getIntValue(0);
    cur_playlist_total_time = d_sqlite3.getInt64Value(1);
    cur_playlist_total_size = d_sqlite3.getInt64Value(2);
  }
    
  d_sqlite3.finalizeStatement();

  return Dingo::Utilities::tracksToString(cur_playlist_total_tracks) + " - " + Dingo::Utilities::durationToString(cur_playlist_total_time, true) + " - " + Dingo::Utilities::filesizeToString(cur_playlist_total_size);
}
      
void Dingo::DBManager::setCurrentTrackRowRef(Gtk::TreeRowReference cur_track_row_ref) {
  d_cur_track_row_ref = cur_track_row_ref;
}

Gtk::TreeRowReference Dingo::DBManager::getCurrentTrackRowRef() {
  return d_cur_track_row_ref;
}

//Dingo::TrackPlaylistEditWindow's METHODS
void Dingo::DBManager::prepareQueryTrackInPlaylist() {
  d_sqlite3.quickExecute("BEGIN TRANSACTION;");

  d_sqlite3.prepareStatement("SELECT CoreTracksPlaylists.TrackID FROM CoreTracksPlaylists WHERE CoreTracksPlaylists.TrackID = :TrackID AND CoreTracksPlaylists.PlaylistID = :PlaylistID;");
}

bool Dingo::DBManager::getTrackInPlaylist(int track_id, int playlist_id) {
  bool is_track_in_playlist = false;
  
  d_sqlite3.bindParaInt(":TrackID", track_id);
  d_sqlite3.bindParaInt(":PlaylistID", playlist_id);
  
  if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    is_track_in_playlist = true;
  }
  
  d_sqlite3.clearStatementBindings();
  
  d_sqlite3.resetStatement();
  
  return is_track_in_playlist;
}

void Dingo::DBManager::finalizeQueryTrackInPlaylist() {
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
}

//Dingo::TrackInfoWindow's METHODS
std::map<Glib::ustring, Glib::ustring> Dingo::DBManager::getTrackInfo(int track_id) {
  std::map<Glib::ustring, Glib::ustring> return_map;
  
  d_sqlite3.prepareStatement("SELECT CoreTracks.TrackTitle, CoreArtists.ArtistName, CoreAlbums.AlbumName, CoreGenres.GenreName, CoreTracks.TrackNumber, CoreYears.YearName, CoreTracks.TrackComment, CoreTracks.TrackFileName, CoreTracks.TrackTime, CoreTracks.TrackBitrate, CoreTracks.TrackURI, CoreTracks.TrackSubtitleURI, CoreTracks.TrackSize, CoreTracks.TrackDateAdded, CoreTracks.TrackLyrics, CoreTracks.TrackMIMEType FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreYears ON CoreTracks.YearID = CoreYears.YearID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID WHERE CoreTracks.TrackID = :TrackID;");
  
  d_sqlite3.bindParaInt(":TrackID", track_id);
  
  while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackTitle", d_sqlite3.getStringValue(0)));
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("ArtistName", d_sqlite3.getStringValue(1)));
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("AlbumName", d_sqlite3.getStringValue(2)));
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("GenreName", d_sqlite3.getStringValue(3)));
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackNumber", Dingo::Utilities::intToString(d_sqlite3.getIntValue(4))));
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("YearName", d_sqlite3.getStringValue(5)));
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackComment", d_sqlite3.getStringValue(6)));
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackFileName", d_sqlite3.getStringValue(7)));
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackTime", Dingo::Utilities::GInt64ToString(d_sqlite3.getInt64Value(8))));
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackBitrate", Dingo::Utilities::intToString(d_sqlite3.getIntValue(9))));
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackLocation", Glib::filename_from_uri(d_sqlite3.getStringValue(10))));
    
    if (!d_sqlite3.getStringValue(11).empty()) {
      return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackSubtitlePath", Glib::filename_from_uri(d_sqlite3.getStringValue(11))));
    }
    
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackSize", Dingo::Utilities::filesizeToString(d_sqlite3.getInt64Value(12))));
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackDateAdded", d_sqlite3.getStringValue(13)));
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackLyrics", d_sqlite3.getStringValue(14)));
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackMIMEType", d_sqlite3.getStringValue(15)));
  }
  
  d_sqlite3.finalizeStatement();
  
  return return_map;
}

void Dingo::DBManager::setTrackInfo(Gtk::TreeRowReference cur_edit_row_ref, std::map<Glib::ustring, Glib::ustring> changed_values_map) {
  //set processgrid & processid_queue
  d_processid_queue[Dingo::PROCESS_EDITING_SINGLE_TRACK] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_EDITING_SINGLE_TRACK);

  updateGUI();
  
  if (!d_processid_queue[Dingo::PROCESS_EDITING_SINGLE_TRACK]) {
    return;
  }

  d_sqlite3.quickExecute("BEGIN TRANSACTION;");

  std::set<Glib::ustring> genre_set;
  std::set<Glib::ustring> artist_set;
  std::set<Glib::ustring> year_set;
  std::set<Glib::ustring> album_set;
  
  //get the Gtk::TreeModel::Row of d_cur_track_row_ref
  Gtk::TreeModel::Row cur_track_row;
  if (d_cur_track_row_ref.is_valid()) {
    cur_track_row = *(Dingo::DBManager::trackModel->get_iter(d_cur_track_row_ref.get_path()));
  }
  
  //get the Gtk::TreeModel::Row of cur_edit_row_ref
  Gtk::TreeModel::Row cur_edit_row = *(Dingo::DBManager::trackModel->get_iter(cur_edit_row_ref.get_path()));
  
  //now start to update the row in CoreTracks
  Glib::ustring set_track_info_query = "UPDATE CoreTracks SET ";
  
  for (std::map<Glib::ustring, Glib::ustring>::iterator iter = changed_values_map.begin(); iter != changed_values_map.end(); ++iter) {
    if (iter->first == "TrackArtist") {
      artist_set.insert(iter->second);
      insertArtistsToDatabase(artist_set, Dingo::PROCESS_EDITING_SINGLE_TRACK);
      if (cur_track_row) {
        cur_edit_row[(*Dingo::DBManager::trackCR).trackArtist] = cur_track_row[(*Dingo::DBManager::trackCR).trackID] == cur_edit_row[(*Dingo::DBManager::trackCR).trackID] ? Dingo::Utilities::highlightString(Dingo::Utilities::encodeFromEntityString(iter->second)) : Dingo::Utilities::encodeFromEntityString(iter->second);
      }
      set_track_info_query += "ArtistID = (SELECT CoreArtists.ArtistID FROM CoreArtists WHERE CoreArtists.ArtistName = :ArtistName)";
    }
    
    else if (iter->first == "TrackAlbum") {
      album_set.insert(iter->second);
      insertAlbumsToDatabase(album_set, Dingo::PROCESS_EDITING_SINGLE_TRACK);
      if (cur_track_row) {
        cur_edit_row[(*Dingo::DBManager::trackCR).trackAlbum] = cur_track_row[(*Dingo::DBManager::trackCR).trackID] == cur_edit_row[(*Dingo::DBManager::trackCR).trackID] ? Dingo::Utilities::highlightString(Dingo::Utilities::encodeFromEntityString(iter->second)) : Dingo::Utilities::encodeFromEntityString(iter->second);
      }
      set_track_info_query += "AlbumID = (SELECT CoreAlbums.AlbumID FROM CoreAlbums WHERE CoreAlbums.AlbumName = :AlbumName)";
    }
    
    else if (iter->first == "TrackGenre") {
      genre_set.insert(iter->second);
      insertGenresToDatabase(genre_set, Dingo::PROCESS_EDITING_SINGLE_TRACK);
      if (cur_track_row) {
        cur_edit_row[(*Dingo::DBManager::trackCR).trackGenre] = cur_track_row[(*Dingo::DBManager::trackCR).trackID] == cur_edit_row[(*Dingo::DBManager::trackCR).trackID] ? Dingo::Utilities::highlightString(Dingo::Utilities::encodeFromEntityString(iter->second)) : Dingo::Utilities::encodeFromEntityString(iter->second);
      }
      set_track_info_query += "GenreID = (SELECT CoreGenres.GenreID FROM CoreGenres WHERE CoreGenres.GenreName = :GenreName)";
    }
    
    else if (iter->first == "TrackYear") {
      year_set.insert(iter->second);
      insertYearsToDatabase(year_set, Dingo::PROCESS_EDITING_SINGLE_TRACK);
      if (cur_track_row) {
        cur_edit_row[(*Dingo::DBManager::trackCR).trackYear] = cur_track_row[(*Dingo::DBManager::trackCR).trackID] == cur_edit_row[(*Dingo::DBManager::trackCR).trackID] ? Dingo::Utilities::highlightString(Dingo::Utilities::encodeFromEntityString(iter->second)) : Dingo::Utilities::encodeFromEntityString(iter->second);
      }
      set_track_info_query += "YearID = (SELECT CoreYears.YearID FROM CoreYears WHERE CoreYears.YearName = :YearName)";
    }
    
    else if (iter->first == "TrackTitle") {
      if (cur_track_row) {
        cur_edit_row[(*Dingo::DBManager::trackCR).trackTitle] = cur_track_row[(*Dingo::DBManager::trackCR).trackID] == cur_edit_row[(*Dingo::DBManager::trackCR).trackID] ? Dingo::Utilities::highlightString(Dingo::Utilities::encodeFromEntityString(iter->second)) : Dingo::Utilities::encodeFromEntityString(iter->second);
      }
      set_track_info_query += "TrackTitle = :TrackTitle";
    }
    
    else if (iter->first == "TrackNumber") {
      if (cur_track_row) {
        cur_edit_row[(*Dingo::DBManager::trackCR).trackNumber] = cur_track_row[(*Dingo::DBManager::trackCR).trackID] == cur_edit_row[(*Dingo::DBManager::trackCR).trackID] ? Dingo::Utilities::highlightString(Dingo::Utilities::encodeFromEntityString(iter->second)) : Dingo::Utilities::encodeFromEntityString(iter->second);
      }
      set_track_info_query += "TrackNumber = :TrackNumber";
    }
    
    else if (iter->first == "TrackSubtitlePath") {
      if (iter->second != "") {
        cur_edit_row[(*Dingo::DBManager::trackCR).trackSubtitleURI] = Glib::filename_to_uri(iter->second);
      }
      
      else {
        cur_edit_row[(*Dingo::DBManager::trackCR).trackSubtitleURI] = iter->second;
      }
      
      set_track_info_query += "TrackSubtitleURI = :TrackSubtitleURI";
    }
    
    else if (iter->first == "TrackMIMEType") {
      continue;
    }
    
    else {
      set_track_info_query += iter->first + " = :" + iter->first;
    }
  
    if (iter != --changed_values_map.end()) {
      set_track_info_query += ", ";
    }
  }
  
  set_track_info_query += " WHERE CoreTracks.TrackID = :TrackID;";
  
  d_sqlite3.prepareStatement(set_track_info_query);
  
  d_sqlite3.bindParaInt(":TrackID", cur_edit_row[(*Dingo::DBManager::trackCR).trackID]);
  
  for (std::map<Glib::ustring, Glib::ustring>::iterator iter = changed_values_map.begin(); iter != changed_values_map.end(); ++iter) {
    if (!d_processid_queue[Dingo::PROCESS_EDITING_SINGLE_TRACK]) {
      break;
    }
  
    if (iter->first == "TrackNumber") {
      d_sqlite3.bindParaInt(":TrackNumber", Dingo::Utilities::stringToInt(iter->second));
    }
    
    else if (iter->first == "TrackArtist") {
      d_sqlite3.bindParaText(":ArtistName", iter->second);
    }
    
    else if (iter->first == "TrackAlbum") {
      d_sqlite3.bindParaText(":AlbumName", iter->second);
    }
    
    else if (iter->first == "TrackGenre") {
      d_sqlite3.bindParaText(":GenreName", iter->second);
    }
    
    else if (iter->first == "TrackYear") {
      d_sqlite3.bindParaText(":YearName", iter->second);
    }
    
    else if (iter->first == "TrackSubtitlePath") {
      if (iter->second != "") {
        d_sqlite3.bindParaText(":TrackSubtitleURI", Glib::filename_to_uri(iter->second));
      }
      
      else {
        d_sqlite3.bindParaText(":TrackSubtitleURI", iter->second);
      }
    }
    
    else if (iter->first == "TrackMIMEType") {
      continue;
    }
    
    else {
      d_sqlite3.bindParaText(":" + iter->first, iter->second);
    }
    
    updateGUI();
  }
  
  if (d_sqlite3.stepStatement() != Dingo::SQLite::ERROR) {
    //std::cout << "Update Track Succeeded!" << std::endl;
  }
  
  else {
    //std::cout << "Update Track Failed!" << std::endl;
  }
  
  d_sqlite3.finalizeStatement(); 
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
  updateGUI();
  
  d_processid_queue[Dingo::PROCESS_EDITING_SINGLE_TRACK] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_EDITING_SINGLE_TRACK) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }
}

//Dingo::MultipleTracksInfoWindow's METHODS
std::map<Glib::ustring, Glib::ustring> Dingo::DBManager::getMultipleTracksInfo(std::vector<int> tracks_id_vector) {
  unsigned int resets_count = tracks_id_vector.size() / (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1) + 1;

  unsigned int variables_count_ceiling = resets_count > 1 ? Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1 : tracks_id_vector.size();

  //first build the multiple_tracks_query
  Glib::ustring multiple_tracks_query = " CoreTracks.TrackID IN (";
  
  for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
    multiple_tracks_query += ":TrackID" + Dingo::Utilities::intToString(i);
    
    if (i != variables_count_ceiling - 1) {
      multiple_tracks_query += ", ";
    }
  }
  
  multiple_tracks_query += ")";
  
  //now get the values to return
  std::map<Glib::ustring, Glib::ustring> return_map;
  
  //first check to see if all selected tracks have the same TrackTitle
  std::set<Glib::ustring> tracktitle_set;
  
  d_sqlite3.prepareStatement("SELECT CoreTracks.TrackTitle FROM CoreTracks WHERE " + multiple_tracks_query + " GROUP BY CoreTracks.TrackTitle HAVING COUNT(CoreTracks.TrackTitle) = :SelectedTracksCount;");
  
  for (unsigned int j = 0; j != resets_count; ++j) {
    variables_count_ceiling = j == resets_count - 1 ? tracks_id_vector.size() - j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1) : Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1;
  
    d_sqlite3.bindParaInt(":SelectedTracksCount", variables_count_ceiling);
  
    for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
      d_sqlite3.bindParaInt(":TrackID" + Dingo::Utilities::intToString(i), tracks_id_vector[i + j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1)]);
    }
  
    if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      tracktitle_set.insert(d_sqlite3.getStringValue(0));
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackTitle", d_sqlite3.getStringValue(0)));
    }
  
    else {
      tracktitle_set.insert("");
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackTitle", ""));
    }
  
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
  }
  
  d_sqlite3.finalizeStatement();
  
  if (tracktitle_set.size() == 1) {
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackTitle", *tracktitle_set.begin()));
  }
  
  //now check to see if all selected tracks have the same ArtistName
  std::set<Glib::ustring> artistname_set;
  
  d_sqlite3.prepareStatement("SELECT CoreArtists.ArtistName FROM CoreTracks INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID WHERE " + multiple_tracks_query + " GROUP BY CoreArtists.ArtistName HAVING COUNT(CoreArtists.ArtistName) = :SelectedTracksCount;");
  
  for (unsigned int j = 0; j != resets_count; ++j) {
    variables_count_ceiling = j == resets_count - 1 ? tracks_id_vector.size() - j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1) : Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1;
    
    d_sqlite3.bindParaInt(":SelectedTracksCount", variables_count_ceiling);
  
    for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
      d_sqlite3.bindParaInt(":TrackID" + Dingo::Utilities::intToString(i), tracks_id_vector[i + j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1)]);
    }
  
    if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      artistname_set.insert(d_sqlite3.getStringValue(0));
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("ArtistName", d_sqlite3.getStringValue(0)));
    }
  
    else {
      artistname_set.insert("");
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("ArtistName", ""));
    }
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
  }
  
  d_sqlite3.finalizeStatement();
  
  if (artistname_set.size() == 1) {
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("ArtistName", *artistname_set.begin()));
  }
  
  //now check to see if all selected tracks have the same AlbumName
  std::set<Glib::ustring> albumname_set;
  
  d_sqlite3.prepareStatement("SELECT CoreAlbums.AlbumName FROM CoreTracks INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID WHERE " + multiple_tracks_query + " GROUP BY CoreAlbums.AlbumName HAVING COUNT(CoreAlbums.AlbumName) = :SelectedTracksCount;");
  
  for (unsigned int j = 0; j != resets_count; ++j) { 
    variables_count_ceiling = j == resets_count - 1 ? tracks_id_vector.size() - j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1) : Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1;
    
    d_sqlite3.bindParaInt(":SelectedTracksCount", variables_count_ceiling);
  
    for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
      d_sqlite3.bindParaInt(":TrackID" + Dingo::Utilities::intToString(i), tracks_id_vector[i + j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1)]);
    }
  
    if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      albumname_set.insert(d_sqlite3.getStringValue(0));
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("AlbumName", d_sqlite3.getStringValue(0)));
    }
  
    else {
      albumname_set.insert("");
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("AlbumName", ""));
    }
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
  }
  
  d_sqlite3.finalizeStatement();
  
  if (albumname_set.size() == 1) {
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("AlbumName", *albumname_set.begin()));
  }
  
  //now check to see if all selected tracks have the same GenreName
  std::set<Glib::ustring> genrename_set;
  
  d_sqlite3.prepareStatement("SELECT CoreGenres.GenreName FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID WHERE " + multiple_tracks_query + " GROUP BY CoreGenres.GenreName HAVING COUNT(CoreGenres.GenreName) = :SelectedTracksCount;");
  
  for (unsigned int j = 0; j != resets_count; ++j) {
    variables_count_ceiling = j == resets_count - 1 ? tracks_id_vector.size() - j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1) : Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1;
  
    d_sqlite3.bindParaInt(":SelectedTracksCount", variables_count_ceiling);
  
    for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
      d_sqlite3.bindParaInt(":TrackID" + Dingo::Utilities::intToString(i), tracks_id_vector[i + j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1)]);
    }
  
    if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      genrename_set.insert(d_sqlite3.getStringValue(0));
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("GenreName", d_sqlite3.getStringValue(0)));
    }
  
    else {
      genrename_set.insert("");
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("GenreName", ""));
    }
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
  }
  
  d_sqlite3.finalizeStatement();
  
  if (genrename_set.size() == 1) {
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("GenreName", *genrename_set.begin()));
  }
  
  //now check to see if all selected tracks have the same TrackNumber
  std::set<int> tracknumber_set;
  
  d_sqlite3.prepareStatement("SELECT CoreTracks.TrackNumber FROM CoreTracks WHERE " + multiple_tracks_query + " GROUP BY CoreTracks.TrackNumber HAVING COUNT(CoreTracks.TrackNumber) = :SelectedTracksCount;");
  
  for (unsigned int j = 0; j != resets_count; ++j) {
    variables_count_ceiling = j == resets_count - 1 ? tracks_id_vector.size() - j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1) : Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1;
  
    d_sqlite3.bindParaInt(":SelectedTracksCount", variables_count_ceiling);
  
    for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
      d_sqlite3.bindParaInt(":TrackID" + Dingo::Utilities::intToString(i), tracks_id_vector[i + j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1)]);
    }
  
    if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      if (d_sqlite3.getIntValue(0)) {
        tracknumber_set.insert(d_sqlite3.getIntValue(0));
        //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackNumber", Dingo::Utilities::intToString(d_sqlite3.getIntValue(0))));
      }
    
      else {
        tracknumber_set.insert(-1);
        //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackNumber", ""));
      }
    }
  
    else {
      tracknumber_set.insert(-1);
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackNumber", ""));
    }
  
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
  }
  
  d_sqlite3.finalizeStatement();
  
  if (tracknumber_set.size() == 1) {
    if ((*tracknumber_set.begin() != 0) && (*tracknumber_set.begin() != -1)) {
      return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackNumber", Dingo::Utilities::intToString(*tracknumber_set.begin())));
    }
    
    else {
      return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackNumber", ""));
    }
  }
  
  //now check to see if all selected tracks have the same YearName
  std::set<Glib::ustring> yearname_set;
  
  d_sqlite3.prepareStatement("SELECT CoreYears.YearName FROM CoreTracks INNER JOIN CoreYears ON CoreTracks.YearID = CoreYears.YearID WHERE " + multiple_tracks_query + " GROUP BY CoreYears.YearName HAVING COUNT(CoreYears.YearName) = :SelectedTracksCount;");
  
  for (unsigned int j = 0; j != resets_count; ++j) {
    variables_count_ceiling = j == resets_count - 1 ? tracks_id_vector.size() - j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1) : Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1;
  
    d_sqlite3.bindParaInt(":SelectedTracksCount", variables_count_ceiling);
  
    for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
      d_sqlite3.bindParaInt(":TrackID" + Dingo::Utilities::intToString(i), tracks_id_vector[i + j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1)]);
    }
  
    if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      yearname_set.insert(d_sqlite3.getStringValue(0));
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("YearName", d_sqlite3.getStringValue(0)));
    }
  
    else {
      yearname_set.insert("");
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("YearName", ""));
    }
  
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
  }
  
  d_sqlite3.finalizeStatement();
  
  if (yearname_set.size() == 1) {
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("YearName", *yearname_set.begin()));
  }
  
  //now get the TotalDuration and TotalSize of all selected tracks
  gint64 total_trackduration = 0;
  gint64 total_tracksize = 0;
  
  d_sqlite3.prepareStatement("SELECT SUM(CoreTracks.TrackTime), SUM(CoreTracks.TrackSize) FROM CoreTracks WHERE " + multiple_tracks_query + ";");
  
  for (unsigned int j = 0; j != resets_count; ++j) {
    variables_count_ceiling = j == resets_count - 1 ? tracks_id_vector.size() - j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1) : Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1;
  
    for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
      d_sqlite3.bindParaInt(":TrackID" + Dingo::Utilities::intToString(i), tracks_id_vector[i + j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1)]);
    }
  
    if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      total_trackduration += d_sqlite3.getInt64Value(0);
      total_tracksize += d_sqlite3.getInt64Value(1);
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TotalDuration", Dingo::Utilities::durationToString(d_sqlite3.getInt64Value(0))));
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TotalSize", Dingo::Utilities::filesizeToString(d_sqlite3.getInt64Value(1))));
    }
  
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
  }
  
  d_sqlite3.finalizeStatement();
  
  return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TotalDuration", Dingo::Utilities::durationToString(total_trackduration)));
  return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TotalSize", Dingo::Utilities::filesizeToString(total_tracksize)));
  
  //now check to see if all selected tracks have the same TrackComment
  std::set<Glib::ustring> trackcomment_set;
  
  d_sqlite3.prepareStatement("SELECT CoreTracks.TrackComment FROM CoreTracks WHERE " + multiple_tracks_query + " GROUP BY CoreTracks.TrackComment HAVING COUNT(CoreTracks.TrackComment) = :SelectedTracksCount;");
  
  for (unsigned int j = 0; j != resets_count; ++j) {
    variables_count_ceiling = j == resets_count - 1 ? tracks_id_vector.size() - j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1) : Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1;
  
    d_sqlite3.bindParaInt(":SelectedTracksCount", variables_count_ceiling);
  
    for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
      d_sqlite3.bindParaInt(":TrackID" + Dingo::Utilities::intToString(i), tracks_id_vector[i + j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - 1)]);
    }
  
    if (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      trackcomment_set.insert(d_sqlite3.getStringValue(0));
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackComment", d_sqlite3.getStringValue(0)));
    }
  
    else {
      trackcomment_set.insert(" ");
      //return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackComment", ""));
    }
  
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
  }
  
  d_sqlite3.finalizeStatement();
  
  if (trackcomment_set.size() == 1) {
    return_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackComment", *trackcomment_set.begin()));
  }
  
  //now return the map
  return return_map;
}

void Dingo::DBManager::setMultipleTracksInfo(std::vector<Gtk::TreeRowReference> cur_row_ref_vector, std::map<Glib::ustring, Glib::ustring> changed_values_map) {
  //set processgrid & processid_queue
  d_processid_queue[Dingo::PROCESS_EDITING_MULTIPLE_TRACKS] = true;
  
  d_processgrid->setCurrentProcessID(Dingo::PROCESS_EDITING_MULTIPLE_TRACKS);

  updateGUI();
  
  if (!d_processid_queue[Dingo::PROCESS_EDITING_MULTIPLE_TRACKS]) {
    return;
  }

  d_sqlite3.quickExecute("BEGIN TRANSACTION;");
  
  std::set<Glib::ustring> genre_set;
  std::set<Glib::ustring> artist_set;
  std::set<Glib::ustring> year_set;
  std::set<Glib::ustring> album_set;
  
  Glib::ustring changed_track_title = "";
  Glib::ustring changed_track_artist = "";
  Glib::ustring changed_track_album = "";
  Glib::ustring changed_track_genre = "";
  Glib::ustring changed_track_year = "";
  Glib::ustring changed_track_number = "";
  
  //construct update_tracks_query
  Glib::ustring update_tracks_query = "UPDATE CoreTracks SET ";
  
  for (std::map<Glib::ustring, Glib::ustring>::iterator iter = changed_values_map.begin(); iter != changed_values_map.end(); ++iter) {
    if (iter->first == "TrackTitle") {
      changed_track_title = Dingo::Utilities::encodeFromEntityString(iter->second);
      update_tracks_query += "TrackTitle = :TrackTitle";
    }
    
    else if (iter->first == "TrackArtist") {
      artist_set.insert(iter->second);
      insertArtistsToDatabase(artist_set, Dingo::PROCESS_EDITING_MULTIPLE_TRACKS);
      changed_track_artist = Dingo::Utilities::encodeFromEntityString(iter->second);
      update_tracks_query += "ArtistID = (SELECT CoreArtists.ArtistID FROM CoreArtists WHERE CoreArtists.ArtistName = :ArtistName)";
    }
    
    else if (iter->first == "TrackAlbum") {
      album_set.insert(iter->second);
      insertAlbumsToDatabase(album_set, Dingo::PROCESS_EDITING_MULTIPLE_TRACKS);
      changed_track_album = Dingo::Utilities::encodeFromEntityString(iter->second);
      update_tracks_query += "AlbumID = (SELECT CoreAlbums.AlbumID FROM CoreAlbums WHERE CoreAlbums.AlbumName = :AlbumName)";
    }
    
    else if (iter->first == "TrackGenre") {
      genre_set.insert(iter->second);
      insertGenresToDatabase(genre_set, Dingo::PROCESS_EDITING_MULTIPLE_TRACKS);
      changed_track_genre = Dingo::Utilities::encodeFromEntityString(iter->second);
      update_tracks_query += "GenreID = (SELECT CoreGenres.GenreID FROM CoreGenres WHERE CoreGenres.GenreName = :GenreName)";
    }
    
    else if (iter->first == "TrackNumber") {
      changed_track_number = iter->second;
      update_tracks_query += "TrackNumber = :TrackNumber";
    }
    
    else if (iter->first == "TrackYear") {
      year_set.insert(iter->second);
      insertYearsToDatabase(year_set, Dingo::PROCESS_EDITING_MULTIPLE_TRACKS);
      changed_track_year = Dingo::Utilities::encodeFromEntityString(iter->second);
      update_tracks_query += "YearID = (SELECT CoreYears.YearID FROM CoreYears WHERE CoreYears.YearName = :YearName)";
    }
    
    else if (iter->first == "TrackComment") {
      update_tracks_query += "TrackComment = :TrackComment";
    }
    
    if (iter != --changed_values_map.end()) {
      update_tracks_query += ", ";
    }
  }
  
  //change Tracks' values in Dingo::DBManager::trackModel
  //also construct the 'WHERE' part of update_tracks_query
  unsigned int resets_count = cur_row_ref_vector.size() / (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - changed_values_map.size()) + 1;
  
  unsigned int variables_count_ceiling = resets_count > 1 ? Dingo::SQLite::LIMIT_VARIABLE_NUMBER - changed_values_map.size() : cur_row_ref_vector.size();
  
  bool is_statement_completed = false;
  
  update_tracks_query += " WHERE CoreTracks.TrackID IN (";
  
  for (unsigned int j = 0; j != resets_count; ++j) {
    variables_count_ceiling = j == resets_count - 1 ? cur_row_ref_vector.size() - j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - changed_values_map.size()) : Dingo::SQLite::LIMIT_VARIABLE_NUMBER - changed_values_map.size();
  
    for (unsigned int i = 0; i != variables_count_ceiling; ++i) { 
      Gtk::TreeModel::Row edit_row = *(Dingo::DBManager::trackModel->get_iter(cur_row_ref_vector[i + j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - changed_values_map.size())].get_path()));
      
      //get the Gtk::TreeModel::Row of d_cur_track_row_ref
      Gtk::TreeModel::Row cur_track_row;
      if (d_cur_track_row_ref.is_valid()) {
        cur_track_row = *(Dingo::DBManager::trackModel->get_iter(d_cur_track_row_ref.get_path()));
      }
    
      if (!changed_track_title.empty()) {
        if (cur_track_row) {
          edit_row[(*Dingo::DBManager::trackCR).trackTitle] = cur_track_row[(*Dingo::DBManager::trackCR).trackID] == edit_row[(*Dingo::DBManager::trackCR).trackID] ? Dingo::Utilities::highlightString(changed_track_title) : changed_track_title;
        }
      }
    
      if (!changed_track_artist.empty()) {
        if (cur_track_row) {
          edit_row[(*Dingo::DBManager::trackCR).trackArtist] = cur_track_row[(*Dingo::DBManager::trackCR).trackID] == edit_row[(*Dingo::DBManager::trackCR).trackID] ? Dingo::Utilities::highlightString(changed_track_title) : changed_track_title;
        }
      }
    
      if (!changed_track_album.empty()) {
        if (cur_track_row) {
          edit_row[(*Dingo::DBManager::trackCR).trackAlbum] = cur_track_row[(*Dingo::DBManager::trackCR).trackID] == edit_row[(*Dingo::DBManager::trackCR).trackID] ? Dingo::Utilities::highlightString(changed_track_title) : changed_track_title;
        }
      }
    
      if (!changed_track_genre.empty()) {
        if (cur_track_row) {
          edit_row[(*Dingo::DBManager::trackCR).trackGenre] = cur_track_row[(*Dingo::DBManager::trackCR).trackID] == edit_row[(*Dingo::DBManager::trackCR).trackID] ? Dingo::Utilities::highlightString(changed_track_title) : changed_track_title;
        }
      }
    
      if (!changed_track_year.empty()) {
        if (cur_track_row) {
          edit_row[(*Dingo::DBManager::trackCR).trackYear] = cur_track_row[(*Dingo::DBManager::trackCR).trackID] == edit_row[(*Dingo::DBManager::trackCR).trackID] ? Dingo::Utilities::highlightString(changed_track_title) : changed_track_title;
        }
      }
    
      if (!changed_track_number.empty()) {
        if (cur_track_row) {
          edit_row[(*Dingo::DBManager::trackCR).trackNumber] = cur_track_row[(*Dingo::DBManager::trackCR).trackID] == edit_row[(*Dingo::DBManager::trackCR).trackID] ? Dingo::Utilities::highlightString(changed_track_title) : changed_track_title;
        }
      }
    
      if (!is_statement_completed) {
        //Continue to construct UPDATE Query
        update_tracks_query += ":TrackID" + Dingo::Utilities::intToString(i);
    
        if (i != variables_count_ceiling - 1) {
          update_tracks_query += ", ";
        }
    
        else {
          is_statement_completed = true;
  
          update_tracks_query += ");";
        }
      }
    }
  }
    
  d_sqlite3.prepareStatement(update_tracks_query);
  
  for (unsigned int j = 0; j != resets_count; ++j) {
    if (!d_processid_queue[Dingo::PROCESS_EDITING_MULTIPLE_TRACKS]) {
      break;
    }
    
    variables_count_ceiling = j == resets_count - 1 ? cur_row_ref_vector.size() - j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - changed_values_map.size()) : Dingo::SQLite::LIMIT_VARIABLE_NUMBER - changed_values_map.size();
    
    for (std::map<Glib::ustring, Glib::ustring>::iterator iter = changed_values_map.begin(); iter != changed_values_map.end(); ++iter) {
      if (!d_processid_queue[Dingo::PROCESS_EDITING_MULTIPLE_TRACKS]) {
        break;
      }
    
      if (iter->first == "TrackNumber") {
        d_sqlite3.bindParaInt(":TrackNumber", Dingo::Utilities::stringToInt(iter->second));
      }
    
      else if (iter->first == "TrackArtist") {
        d_sqlite3.bindParaText(":ArtistName", iter->second);
      }
    
      else if (iter->first == "TrackAlbum") {
        d_sqlite3.bindParaText(":AlbumName", iter->second);
      }
    
      else if (iter->first == "TrackGenre") {
        d_sqlite3.bindParaText(":GenreName", iter->second);
      }
      
      else if (iter->first == "TrackYear") {
        d_sqlite3.bindParaText(":YearName", iter->second);
      }
    
      else {
        d_sqlite3.bindParaText(":" + iter->first, iter->second);
      }
  
      for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
        if (!d_processid_queue[Dingo::PROCESS_EDITING_MULTIPLE_TRACKS]) {
          break;
        }
      
        Gtk::TreeModel::Row edit_row = *(Dingo::DBManager::trackModel->get_iter(cur_row_ref_vector[i + j * (Dingo::SQLite::LIMIT_VARIABLE_NUMBER - changed_values_map.size())].get_path()));
  
        d_sqlite3.bindParaInt(":TrackID" + Dingo::Utilities::intToString(i), edit_row[(*Dingo::DBManager::trackCR).trackID]);
      }
  
      d_sqlite3.stepStatement();
    
      d_sqlite3.clearStatementBindings();
    
      d_sqlite3.resetStatement();
      
      updateGUI();
    }
  }
  
  d_sqlite3.finalizeStatement();
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
  
  updateGUI();
  
  d_processid_queue[Dingo::PROCESS_EDITING_MULTIPLE_TRACKS] = false;
  
  if (d_processgrid->getCurrentProcessID() == Dingo::PROCESS_EDITING_MULTIPLE_TRACKS) {
    d_processgrid->notifyProcessGridObserver(Dingo::PROCESS_FINISHED);
  }
}

//Dingo::Player's METHODS
void Dingo::DBManager::setWindow(Gtk::Window* window) {
  parent_window = window;
}

Gtk::Window* Dingo::DBManager::getWindow() {
  return parent_window;
}
      
//LOADING LISTSTORE
void Dingo::DBManager::createDatabase() {
  Glib::RefPtr<Gio::File> sqlite3_db_file = Gio::File::create_for_path(Glib::get_home_dir() + "/.config/dingo-0.5/database/dingo.db");

  if (!sqlite3_db_file->query_exists()) {
    Dingo::Utilities::createConfigFolders();
    
    createDatabaseFile();
  }
  
  else {
    if (d_sqlite3.open(d_database_path)) {
      d_sqlite3.prepareStatement("SELECT COUNT(*) FROM CoreTracks INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreYears ON CoreTracks.YearID = CoreYears.YearID INNER JOIN CoreAlbums ON CoreAlbums.AlbumID INNER JOIN CoreTracksPlaylists ON CoreTracks.TrackID = CoreTracksPlaylists.TrackID INNER JOIN CorePlaylists ON CoreTracksPlaylists.PlaylistID = CorePlaylists.PlaylistID;");
    
      if (d_sqlite3.stepStatement() != Dingo::SQLite::ROW) {
        sqlite3_db_file->remove();
    
        d_sqlite3.finalizeStatement();
      
        createDatabaseFile();
      }
    
      else {
        d_sqlite3.finalizeStatement();
      }
    }
    
    else {
      sqlite3_db_file->remove();
      
      createDatabaseFile();
    }
  }
}

void Dingo::DBManager::createPlaylistModel() {
  Dingo::DBManager::playlistModel = Gtk::ListStore::create(*Dingo::DBManager::playlistCR);
  
  //first load all playlists into Dingo::PlaylistTreeView
  d_sqlite3.prepareStatement("SELECT CorePlaylists.PlaylistID, CorePlaylists.PlaylistName FROM CorePlaylists WHERE CorePlaylists.PlaylistID != 1 ORDER BY CorePlaylists.PlaylistName;");
  
  while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    Gtk::TreeModel::Row temp_row = *(Dingo::DBManager::playlistModel->append());
    
    temp_row[(*Dingo::DBManager::playlistCR).playlistIcon] = d_playlist_image.get_pixbuf();
    temp_row[(*Dingo::DBManager::playlistCR).playlistID] = d_sqlite3.getIntValue(0);
    temp_row[(*Dingo::DBManager::playlistCR).playlistName] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(1));
  }
  
  d_sqlite3.finalizeStatement();
  
  Gtk::TreeModel::iterator first_iter = Dingo::DBManager::playlistModel->prepend();
  
  Gtk::TreeModel::Row first_row = *first_iter;
  
  first_row[(*Dingo::DBManager::playlistCR).playlistIcon] = d_playlist_image.get_pixbuf();
  first_row[(*Dingo::DBManager::playlistCR).playlistID] = 0;
  first_row[(*Dingo::DBManager::playlistCR).playlistName] = _("All Tracks");
}

void Dingo::DBManager::createGenreModel() {
  Dingo::DBManager::genreModel = Gtk::ListStore::create(*Dingo::DBManager::genreCR);
  
  //first load all genres into Dingo::GenreTreeView
  d_sqlite3.prepareStatement("SELECT CoreGenres.GenreID, CoreGenres.GenreName FROM CoreGenres ORDER BY CoreGenres.GenreName;");
  
  while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    Gtk::TreeModel::Row temp_row = *(Dingo::DBManager::genreModel->append());
    
    temp_row[(*Dingo::DBManager::genreCR).genreIcon] = d_genre_image.get_pixbuf();
    temp_row[(*Dingo::DBManager::genreCR).genreID] = d_sqlite3.getIntValue(0);
    temp_row[(*Dingo::DBManager::genreCR).genreName] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(1));
    temp_row[(*Dingo::DBManager::genreCR).genreVisible] = true;
  }
  
  d_sqlite3.finalizeStatement();
  
  Gtk::TreeModel::Row first_row = *(Dingo::DBManager::genreModel->prepend());
  
  first_row[(*Dingo::DBManager::genreCR).genreIcon] = d_genre_image.get_pixbuf();
  first_row[(*Dingo::DBManager::genreCR).genreID] = 0;
  first_row[(*Dingo::DBManager::genreCR).genreName] = _("All Genres");
  first_row[(*Dingo::DBManager::genreCR).genreVisible] = true;
}

void Dingo::DBManager::createArtistModel() {
  Dingo::DBManager::artistModel = Gtk::ListStore::create(*Dingo::DBManager::artistCR);
  
  //first load all albums into Dingo::ArtistTreeView
  d_sqlite3.prepareStatement("SELECT CoreArtists.ArtistID, CoreArtists.ArtistName FROM CoreArtists ORDER BY CoreArtists.ArtistName;");
  
  while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {    
    Gtk::TreeModel::Row temp_row = *(Dingo::DBManager::artistModel->append());
    
    temp_row[(*Dingo::DBManager::artistCR).artistIcon] = d_artist_image.get_pixbuf();
    temp_row[(*Dingo::DBManager::artistCR).artistID] = d_sqlite3.getIntValue(0);
    temp_row[(*Dingo::DBManager::artistCR).artistName] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(1));
    temp_row[(*Dingo::DBManager::artistCR).artistVisible] = true;
  }
  
  d_sqlite3.finalizeStatement();
  
  Gtk::TreeModel::iterator first_iter = Dingo::DBManager::artistModel->prepend();
  
  Gtk::TreeModel::Row first_row = *first_iter;
  
  first_row[(*Dingo::DBManager::artistCR).artistIcon] = d_artist_image.get_pixbuf();
  first_row[(*Dingo::DBManager::artistCR).artistID] = 0;
  first_row[(*Dingo::DBManager::artistCR).artistName] = _("All Artists");
  first_row[(*Dingo::DBManager::artistCR).artistVisible] = true;
}

void Dingo::DBManager::createYearModel() {
  Dingo::DBManager::yearModel = Gtk::ListStore::create(*Dingo::DBManager::yearCR);
  
  //first load all years into Dingo::YearTreeView
  d_sqlite3.prepareStatement("SELECT CoreYears.YearID, CoreYears.YearName FROM CoreYears ORDER BY CoreYears.YearName;");
  
  while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    Gtk::TreeModel::Row temp_row = *(Dingo::DBManager::yearModel->append());
    
    temp_row[(*Dingo::DBManager::yearCR).yearIcon] = d_year_image.get_pixbuf();
    temp_row[(*Dingo::DBManager::yearCR).yearID] = d_sqlite3.getIntValue(0);
    temp_row[(*Dingo::DBManager::yearCR).yearName] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(1));
    temp_row[(*Dingo::DBManager::yearCR).yearVisible] = true;
  }
  
  d_sqlite3.finalizeStatement();
  
  Gtk::TreeModel::Row first_row = *(Dingo::DBManager::yearModel->prepend());
  
  first_row[(*Dingo::DBManager::yearCR).yearIcon] = d_year_image.get_pixbuf();
  first_row[(*Dingo::DBManager::yearCR).yearID] = 0;
  first_row[(*Dingo::DBManager::yearCR).yearName] = _("All Years");
  first_row[(*Dingo::DBManager::yearCR).yearVisible] = true;
}

void Dingo::DBManager::createAlbumModel() {
  Dingo::DBManager::albumModel = Gtk::ListStore::create(*Dingo::DBManager::albumCR);
   
  //first load all albums into Dingo::ArtistTreeView
  d_sqlite3.prepareStatement("SELECT CoreAlbums.AlbumID, CoreAlbums.AlbumName FROM CoreAlbums ORDER BY CoreAlbums.AlbumName;");
  
  while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    Gtk::TreeModel::Row temp_row = *(Dingo::DBManager::albumModel->append());
    
    temp_row[(*Dingo::DBManager::albumCR).albumIcon] = d_album_image.get_pixbuf();
    temp_row[(*Dingo::DBManager::albumCR).albumID] = d_sqlite3.getIntValue(0);
    temp_row[(*Dingo::DBManager::albumCR).albumName] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(1));
    temp_row[(*Dingo::DBManager::albumCR).albumVisible] = true;
  }
  
  d_sqlite3.finalizeStatement();
  
  Gtk::TreeModel::iterator first_iter = Dingo::DBManager::albumModel->prepend();
  
  Gtk::TreeModel::Row first_row = *first_iter;
  
  first_row[(*Dingo::DBManager::albumCR).albumIcon] = d_album_image.get_pixbuf();
  first_row[(*Dingo::DBManager::albumCR).albumID] = 0;
  first_row[(*Dingo::DBManager::albumCR).albumName] = _("All Albums"); 
  first_row[(*Dingo::DBManager::albumCR).albumVisible] = true;
}

void Dingo::DBManager::createTrackModel() {
  Dingo::DBManager::trackModel = Gtk::ListStore::create(*Dingo::DBManager::trackCR);
  
  d_sqlite3.prepareStatement("SELECT CoreTracks.TrackID, CoreTracks.TrackNumber, CoreTracks.TrackTitle, CoreGenres.GenreName, CoreArtists.ArtistName, CoreAlbums.AlbumName, CoreYears.YearName, CoreTracks.TrackTime, CoreTracks.TrackFileExtension, CoreTracks.TrackBitrate, CoreTracks.TrackMIMEType, CoreTracks.TrackURI, CoreTracks.TrackSubtitleURI FROM CoreTracks INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreYears ON CoreTracks.YearID = CoreYears.YearID ORDER BY CoreTracks.TrackTitle;");
  
  while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    Gtk::TreeModel::iterator iter = Dingo::DBManager::trackModel->append();
    
    Gtk::TreeModel::Row row = *iter;
    
    row[(*Dingo::DBManager::trackCR).trackID] = d_sqlite3.getIntValue(0);
    if (d_sqlite3.getIntValue(1) > 0) {
      row[(*Dingo::DBManager::trackCR).trackNumber] = Dingo::Utilities::intToString(d_sqlite3.getIntValue(1));
    }
    row[(*Dingo::DBManager::trackCR).trackTitle] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(2));
    row[(*Dingo::DBManager::trackCR).trackGenre] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(3));
    row[(*Dingo::DBManager::trackCR).trackArtist] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(4));
    row[(*Dingo::DBManager::trackCR).trackAlbum] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(5));
    row[(*Dingo::DBManager::trackCR).trackYear] = d_sqlite3.getStringValue(6);
    row[(*Dingo::DBManager::trackCR).trackTime] = Dingo::Utilities::durationToString(d_sqlite3.getInt64Value(7));
    row[(*Dingo::DBManager::trackCR).trackFileExtension] = d_sqlite3.getStringValue(8);
    row[(*Dingo::DBManager::trackCR).trackBitrate] = Dingo::Utilities::intToString(d_sqlite3.getIntValue(9)) + " kbps";
    row[(*Dingo::DBManager::trackCR).trackMIMEType] = d_sqlite3.getStringValue(10);
    row[(*Dingo::DBManager::trackCR).trackURI] = d_sqlite3.getStringValue(11);
    row[(*Dingo::DBManager::trackCR).trackSubtitleURI] = d_sqlite3.getStringValue(12);
  }
  
  d_sqlite3.finalizeStatement();
}

void Dingo::DBManager::createTrackPlaylistModel() {
  Dingo::DBManager::trackplaylistModel = Gtk::ListStore::create(*Dingo::DBManager::trackplaylistCR);
  
  d_sqlite3.prepareStatement("SELECT CorePlaylists.PlaylistID, CorePlaylists.PlaylistName FROM CorePlaylists WHERE CorePlaylists.PlaylistID != 1 ORDER BY CorePlaylists.PlaylistName;");
  
  while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
    Gtk::TreeModel::iterator iter = Dingo::DBManager::trackplaylistModel->append();
    
    Gtk::TreeModel::Row row = *iter;
    
    row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistID] = d_sqlite3.getIntValue(0);    
    row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox] = false;
    row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistent] = true;
    row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistName] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(1));
    row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountOriginal] = d_sqlite3.getIntValue(2);
    row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges] = 0;
    row[(*Dingo::DBManager::trackplaylistCR).trackplaylistVisible] = true;
  }
  
  d_sqlite3.finalizeStatement();
}

//SIGNAL HANDLERS
void Dingo::DBManager::updateGUI(bool is_blocking) {
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
}

//UTILITIES
void Dingo::DBManager::addTracksFolderRecursively(Glib::ustring folder_path, std::vector<std::vector<Glib::ustring> >& all_tracks_info_vector, std::set<Glib::ustring>& all_tracks_genre_set, std::set<Glib::ustring>& all_tracks_artist_set, std::set<Glib::ustring>& all_tracks_year_set, std::set<Glib::ustring>& all_tracks_album_set) {
  Glib::Dir top_dir(folder_path);
  Glib::ustring file_name = top_dir.read_name();
  
  while (!file_name.empty()) {
    if (Glib::file_test(folder_path + "/" + file_name, Glib::FILE_TEST_IS_DIR)) {
      updateGUI();
    
      addTracksFolderRecursively(folder_path + "/" + file_name, all_tracks_info_vector, all_tracks_genre_set, all_tracks_artist_set, all_tracks_year_set, all_tracks_album_set);
    }
    
    else if (Glib::file_test(folder_path + "/" + file_name, Glib::FILE_TEST_IS_REGULAR)) {
      updateGUI();
    
      if (d_tagreader.read(Glib::filename_to_uri(folder_path + "/" + file_name))) {
        std::vector<Glib::ustring> track_info_vector;
    
        track_info_vector.push_back(d_tagreader.getGenre());
        all_tracks_genre_set.insert(d_tagreader.getGenre());
    
        track_info_vector.push_back(d_tagreader.getArtist());
        all_tracks_artist_set.insert(d_tagreader.getArtist());
    
        track_info_vector.push_back(d_tagreader.getYear());
        all_tracks_year_set.insert(d_tagreader.getYear());
    
        track_info_vector.push_back(d_tagreader.getAlbum());
        all_tracks_album_set.insert(d_tagreader.getAlbum());
    
        track_info_vector.push_back(Dingo::Utilities::intToString(d_tagreader.getTrackNumber()));
        track_info_vector.push_back(d_tagreader.getTitle());
        track_info_vector.push_back(d_tagreader.getComment());
        track_info_vector.push_back(d_tagreader.getLyrics());
        track_info_vector.push_back(Dingo::Utilities::GInt64ToString(d_tagreader.getDuration()));
        track_info_vector.push_back(Dingo::Utilities::GInt64ToString(d_tagreader.getFileSize()));
        track_info_vector.push_back(d_tagreader.getFileName());
        track_info_vector.push_back(d_tagreader.getFileExtension());
        track_info_vector.push_back(d_tagreader.getFileURI());
        track_info_vector.push_back("");
        track_info_vector.push_back(Dingo::Utilities::intToString(d_tagreader.getBitrate()));
        track_info_vector.push_back(d_tagreader.getMIMEType());
        track_info_vector.push_back(d_tagreader.getDateAdded());
    
        all_tracks_info_vector.push_back(track_info_vector);
    
        updateGUI();
      }
    }
    
    file_name = top_dir.read_name();
  }
}

void Dingo::DBManager::insertGenresToDatabase(std::set<Glib::ustring>& all_tracks_genre_set, Dingo::ProcessID process_id) {
  //First insert all the Genres in
  std::vector<Glib::ustring> new_genres_vector;
  
  updateGUI();
  
  d_sqlite3.prepareStatement("INSERT INTO CoreGenres (GenreName) VALUES (:GenreName);");
  
  for (std::set<Glib::ustring>::iterator iter = all_tracks_genre_set.begin(); iter != all_tracks_genre_set.end(); ++iter) {
    d_sqlite3.bindParaText(":GenreName", *iter);
    
    if (d_sqlite3.stepStatement() == Dingo::SQLite::DONE) {
      new_genres_vector.push_back(*iter);
    }
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
  
  unsigned int resets_count = new_genres_vector.size() / Dingo::SQLite::LIMIT_VARIABLE_NUMBER + 1;
  
  unsigned int variables_count_ceiling = resets_count > 1 ? Dingo::SQLite::LIMIT_VARIABLE_NUMBER : new_genres_vector.size();
  
  //Then get all GenreIDs & GenreNames & insert into genreModel
  Glib::ustring select_genres_query = "SELECT CoreGenres.GenreID, CoreGenres.GenreName FROM CoreGenres WHERE CoreGenres.GenreName IN (";
    
  for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
    select_genres_query += ":GenreName" + Dingo::Utilities::intToString(i);
    
    if (i != variables_count_ceiling - 1) {
      select_genres_query += ", ";
    }
  }
  
  select_genres_query += ");";
  
  d_sqlite3.prepareStatement(select_genres_query);
      
  for (unsigned int j = 0; j != resets_count; ++j) {
    variables_count_ceiling = j == resets_count - 1 ? new_genres_vector.size() - j * Dingo::SQLite::LIMIT_VARIABLE_NUMBER : Dingo::SQLite::LIMIT_VARIABLE_NUMBER;
  
    for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
      d_sqlite3.bindParaText(":GenreName" + Dingo::Utilities::intToString(i), new_genres_vector[i + j * Dingo::SQLite::LIMIT_VARIABLE_NUMBER]);
    }
  
    while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      Gtk::TreeModel::Row genre_row = *(Dingo::DBManager::genreModel->append());
    
      genre_row[(*Dingo::DBManager::genreCR).genreIcon] = d_genre_image.get_pixbuf();
      genre_row[(*Dingo::DBManager::genreCR).genreID] = d_sqlite3.getIntValue(0);
      genre_row[(*Dingo::DBManager::genreCR).genreName] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(1));
    }
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
}

void Dingo::DBManager::insertArtistsToDatabase(std::set<Glib::ustring>& all_tracks_artist_set, Dingo::ProcessID process_id) {
  //Then insert all the Artists in
  std::vector<Glib::ustring> new_artists_vector;
  
  updateGUI();
  
  d_sqlite3.prepareStatement("INSERT INTO CoreArtists (ArtistName) VALUES (:ArtistName);");
  
  for (std::set<Glib::ustring>::iterator iter = all_tracks_artist_set.begin(); iter != all_tracks_artist_set.end(); ++iter) {
    d_sqlite3.bindParaText(":ArtistName", *iter);
    
    if (d_sqlite3.stepStatement() == Dingo::SQLite::DONE) {
      new_artists_vector.push_back(*iter);
    }
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
  
  unsigned int resets_count = new_artists_vector.size() / Dingo::SQLite::LIMIT_VARIABLE_NUMBER + 1;
  
  unsigned int variables_count_ceiling = resets_count > 1 ? Dingo::SQLite::LIMIT_VARIABLE_NUMBER : new_artists_vector.size();
  
  //Then get all ArtistIDs and ArtistNames & insert into artistModel
  Glib::ustring select_artists_query = "SELECT CoreArtists.ArtistID, CoreArtists.ArtistName FROM CoreArtists WHERE CoreArtists.ArtistName IN (";
  
  for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
    select_artists_query += ":ArtistName" + Dingo::Utilities::intToString(i);
    
    if (i != variables_count_ceiling - 1) {
      select_artists_query += ", ";
    }
  }
  
  select_artists_query += ");";
  
  d_sqlite3.prepareStatement(select_artists_query);
  
  for (unsigned int j = 0; j != resets_count; ++j) {
    variables_count_ceiling = j == resets_count - 1 ? new_artists_vector.size() - j * Dingo::SQLite::LIMIT_VARIABLE_NUMBER : Dingo::SQLite::LIMIT_VARIABLE_NUMBER;
  
    for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
      d_sqlite3.bindParaText(":ArtistName" + Dingo::Utilities::intToString(i), new_artists_vector[i + j * Dingo::SQLite::LIMIT_VARIABLE_NUMBER]);
    }
  
    while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      Gtk::TreeModel::Row artist_row = *(Dingo::DBManager::artistModel->append());
    
      artist_row[(*Dingo::DBManager::artistCR).artistIcon] = d_artist_image.get_pixbuf();
      artist_row[(*Dingo::DBManager::artistCR).artistID] = d_sqlite3.getIntValue(0);
      artist_row[(*Dingo::DBManager::artistCR).artistName] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(1));
    }
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
}

void Dingo::DBManager::insertYearsToDatabase(std::set<Glib::ustring>& all_tracks_year_set, Dingo::ProcessID process_id) {
  //Then insert all the Years in
  std::vector<Glib::ustring> new_years_vector;
  
  updateGUI();
  
  d_sqlite3.prepareStatement("INSERT INTO CoreYears (YearName) VALUES (:YearName);");
  
  for (std::set<Glib::ustring>::iterator iter = all_tracks_year_set.begin(); iter != all_tracks_year_set.end(); ++iter) {
    d_sqlite3.bindParaText(":YearName", *iter);
    
    if (d_sqlite3.stepStatement() == Dingo::SQLite::DONE) {
      new_years_vector.push_back(*iter);
    }
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
  
  unsigned int resets_count = new_years_vector.size() / Dingo::SQLite::LIMIT_VARIABLE_NUMBER + 1;
  
  unsigned int variables_count_ceiling = resets_count > 1 ? Dingo::SQLite::LIMIT_VARIABLE_NUMBER : new_years_vector.size();
  
  //Then get all YearIDs and YearNames & insert into yearModel
  Glib::ustring select_years_query = "SELECT CoreYears.YearID, CoreYears.YearName FROM CoreYears WHERE CoreYears.YearName IN (";
  
  for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
    select_years_query += ":YearName" + Dingo::Utilities::intToString(i);
    
    if (i != variables_count_ceiling - 1) {
      select_years_query += ", ";
    }
  }
  
  select_years_query += ");";
  
  d_sqlite3.prepareStatement(select_years_query);
    
  for (unsigned int j = 0; j != resets_count; ++j) {
    variables_count_ceiling = j == resets_count - 1 ? new_years_vector.size() - j * Dingo::SQLite::LIMIT_VARIABLE_NUMBER : Dingo::SQLite::LIMIT_VARIABLE_NUMBER;
  
    for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
      d_sqlite3.bindParaText(":YearName" + Dingo::Utilities::intToString(i), new_years_vector[i + j * Dingo::SQLite::LIMIT_VARIABLE_NUMBER]);
    }
  
    while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      Gtk::TreeModel::Row year_row = *(Dingo::DBManager::yearModel->append());
    
      year_row[(*Dingo::DBManager::yearCR).yearIcon] = d_year_image.get_pixbuf();
      year_row[(*Dingo::DBManager::yearCR).yearID] = d_sqlite3.getIntValue(0);
      year_row[(*Dingo::DBManager::yearCR).yearName] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(1));
    }
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
}

void Dingo::DBManager::insertAlbumsToDatabase(std::set<Glib::ustring>& all_tracks_album_set, Dingo::ProcessID process_id) {
  //Then insert all the Albums in
  std::vector<Glib::ustring> new_albums_vector;
  
  updateGUI();
  
  d_sqlite3.prepareStatement("INSERT INTO CoreAlbums (AlbumName) VALUES (:AlbumName);");
  
  for (std::set<Glib::ustring>::iterator iter = all_tracks_album_set.begin(); iter != all_tracks_album_set.end(); ++iter) {
    d_sqlite3.bindParaText(":AlbumName", *iter);
    
    if (d_sqlite3.stepStatement() == Dingo::SQLite::DONE) {
      new_albums_vector.push_back(*iter);
    }
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
  
  unsigned int resets_count = new_albums_vector.size() / Dingo::SQLite::LIMIT_VARIABLE_NUMBER + 1;
  
  unsigned int variables_count_ceiling = resets_count > 1 ? Dingo::SQLite::LIMIT_VARIABLE_NUMBER : new_albums_vector.size();
  
  //Then get all AlbumIDs and AlbumNames & insert into albumModel
  Glib::ustring select_albums_query = "SELECT CoreAlbums.AlbumID, CoreAlbums.AlbumName FROM CoreAlbums WHERE CoreAlbums.AlbumName IN (";
  
  for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
    select_albums_query += ":AlbumName" + Dingo::Utilities::intToString(i);
    
    if (i != variables_count_ceiling - 1) {
      select_albums_query += ", ";
    }
  }
  
  select_albums_query += ");";
  
  d_sqlite3.prepareStatement(select_albums_query);
  
  for (unsigned int j = 0; j != resets_count; ++j) {
    variables_count_ceiling = j == resets_count - 1 ? new_albums_vector.size() - j * Dingo::SQLite::LIMIT_VARIABLE_NUMBER : Dingo::SQLite::LIMIT_VARIABLE_NUMBER;
  
    for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
      d_sqlite3.bindParaText(":AlbumName" + Dingo::Utilities::intToString(i), new_albums_vector[i + j * Dingo::SQLite::LIMIT_VARIABLE_NUMBER]);
    }
  
    while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      Gtk::TreeModel::Row album_row = *(Dingo::DBManager::albumModel->append());
    
      album_row[(*Dingo::DBManager::albumCR).albumIcon] = d_album_image.get_pixbuf();
      album_row[(*Dingo::DBManager::albumCR).albumID] = d_sqlite3.getIntValue(0);
      album_row[(*Dingo::DBManager::albumCR).albumName] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(1));
    }
  
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
    
    updateGUI();
  }
  
  d_sqlite3.finalizeStatement();
}

void Dingo::DBManager::insertTracksToDatabase(std::vector<std::vector<Glib::ustring> >& all_tracks_info_vector, Dingo::ProcessID process_id, int playlist_id) {
  //insert Tracks into CoreTracks
  std::vector<Glib::ustring> track_model_uris_vector;
  
  updateGUI();
  
  d_sqlite3.prepareStatement("INSERT INTO CoreTracks (GenreID, ArtistID, YearID, AlbumID, TrackNumber, TrackTitle, TrackComment, TrackLyrics, TrackTime, TrackSize, TrackFileName, TrackFileExtension, TrackURI, TrackSubtitleURI, TrackBitrate, TrackMIMEType, TrackDateAdded) SELECT (SELECT CoreGenres.GenreID FROM CoreGenres WHERE CoreGenres.GenreName = :GenreName), (SELECT CoreArtists.ArtistID FROM CoreArtists WHERE CoreArtists.ArtistName = :ArtistName), (SELECT CoreYears.YearID FROM CoreYears WHERE CoreYears.YearName = :YearName), (SELECT CoreAlbums.AlbumID FROM CoreAlbums WHERE CoreAlbums.AlbumName = :AlbumName), :TrackNumber, :TrackTitle, :TrackComment, :TrackLyrics, :TrackTime, :TrackSize, :TrackFileName, :TrackFileExtension, :TrackURI, :TrackSubtitleURI, :TrackBitrate, :TrackMIMEType, :TrackDateAdded;");
  
  d_processgrid->setPulse(false);
  
  for (unsigned int i = 0; i != all_tracks_info_vector.size(); ++i) {
    if (d_processgrid->getCurrentProcessID() == process_id) {
      d_processgrid->setTotalFilesCount(all_tracks_info_vector.size());
      
      d_processgrid->setCurrentFilesCount(i + 1);
      
      d_processgrid->setCurrentFileName(all_tracks_info_vector[i][10]);
    }
    
    updateGUI();
    
    d_sqlite3.bindParaText(":GenreName", all_tracks_info_vector[i][0]);
    d_sqlite3.bindParaText(":ArtistName", all_tracks_info_vector[i][1]);
    d_sqlite3.bindParaText(":YearName", all_tracks_info_vector[i][2]);
    d_sqlite3.bindParaText(":AlbumName", all_tracks_info_vector[i][3]);
      
    d_sqlite3.bindParaInt(":TrackNumber", Dingo::Utilities::stringToInt(all_tracks_info_vector[i][4]));
    d_sqlite3.bindParaText(":TrackTitle", all_tracks_info_vector[i][5]);
    d_sqlite3.bindParaText(":TrackComment", all_tracks_info_vector[i][6]);
    d_sqlite3.bindParaText(":TrackLyrics", all_tracks_info_vector[i][7]);
    d_sqlite3.bindParaInt64(":TrackTime", Dingo::Utilities::stringToSQLite3Int64(all_tracks_info_vector[i][8]));
    d_sqlite3.bindParaInt64(":TrackSize", Dingo::Utilities::stringToSQLite3Int64(all_tracks_info_vector[i][9]));
    d_sqlite3.bindParaText(":TrackFileName", all_tracks_info_vector[i][10]);
    d_sqlite3.bindParaText(":TrackFileExtension", all_tracks_info_vector[i][11]);
    d_sqlite3.bindParaText(":TrackURI", all_tracks_info_vector[i][12]);
    d_sqlite3.bindParaText(":TrackSubtitleURI", all_tracks_info_vector[i][13]);
    d_sqlite3.bindParaInt(":TrackBitrate", Dingo::Utilities::stringToInt(all_tracks_info_vector[i][14]));
    d_sqlite3.bindParaText(":TrackMIMEType", all_tracks_info_vector[i][15]);
    d_sqlite3.bindParaText(":TrackDateAdded", all_tracks_info_vector[i][16]);
    
    if (d_sqlite3.stepStatement() == Dingo::SQLite::DONE) {
      track_model_uris_vector.push_back(all_tracks_info_vector[i][12]);
    }
    
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
  }
      
  d_sqlite3.finalizeStatement();
  
  //Then insert Tracks' info into Dingo::DBManager::trackModel
  unsigned int resets_count = track_model_uris_vector.size() / Dingo::SQLite::LIMIT_VARIABLE_NUMBER + 1;
  
  unsigned int variables_count_ceiling = resets_count > 1 ? Dingo::SQLite::LIMIT_VARIABLE_NUMBER : track_model_uris_vector.size();
  
  Glib::ustring select_tracks_query = "SELECT CoreTracks.TrackID, CoreTracks.TrackNumber, CoreTracks.TrackTitle, CoreGenres.GenreName, CoreArtists.ArtistName, CoreAlbums.AlbumName, CoreYears.YearName, CoreTracks.TrackTime, CoreTracks.TrackFileExtension, CoreTracks.TrackBitrate, CoreTracks.TrackMIMEType, CoreTracks.TrackURI, CoreTracks.TrackSubtitleURI, CoreTracks.TrackFileName FROM CoreTracks INNER JOIN CoreArtists ON CoreTracks.ArtistID = CoreArtists.ArtistID INNER JOIN CoreAlbums ON CoreTracks.AlbumID = CoreAlbums.AlbumID INNER JOIN CoreGenres ON CoreTracks.GenreID = CoreGenres.GenreID INNER JOIN CoreYears ON CoreTracks.YearID = CoreYears.YearID WHERE CoreTracks.TrackURI IN (";
  
  for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
    select_tracks_query += ":TrackURI" + Dingo::Utilities::intToString(i);
    
    if (i != variables_count_ceiling - 1) {
      select_tracks_query += ", ";
    }
  }
  
  select_tracks_query += ") ORDER BY CoreTracks.TrackTitle;";
  
  d_sqlite3.prepareStatement(select_tracks_query);
  
  for (unsigned int j = 0; j != resets_count; ++j) {
    variables_count_ceiling = j == resets_count - 1 ? track_model_uris_vector.size() - j * Dingo::SQLite::LIMIT_VARIABLE_NUMBER : Dingo::SQLite::LIMIT_VARIABLE_NUMBER;
  
    for (unsigned int i = 0; i != variables_count_ceiling; ++i) {
      d_sqlite3.bindParaText(":TrackURI" + Dingo::Utilities::intToString(i), track_model_uris_vector[i + j * Dingo::SQLite::LIMIT_VARIABLE_NUMBER]);
    }
  
    while (d_sqlite3.stepStatement() == Dingo::SQLite::ROW) {
      updateGUI();
  
      Gtk::TreeModel::Row row = *(Dingo::DBManager::trackModel->append());
    
      row[(*Dingo::DBManager::trackCR).trackID] = d_sqlite3.getIntValue(0);
      d_first_added_track_id = d_sqlite3.getIntValue(0);
      
      if (d_sqlite3.getIntValue(1) > 0) {
        row[(*Dingo::DBManager::trackCR).trackNumber] = Dingo::Utilities::intToString(d_sqlite3.getIntValue(1));
      }
      row[(*Dingo::DBManager::trackCR).trackTitle] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(2));
      row[(*Dingo::DBManager::trackCR).trackGenre] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(3));
      row[(*Dingo::DBManager::trackCR).trackArtist] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(4));
      row[(*Dingo::DBManager::trackCR).trackAlbum] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(5));
      row[(*Dingo::DBManager::trackCR).trackYear] = Dingo::Utilities::encodeFromEntityString(d_sqlite3.getStringValue(6));
      row[(*Dingo::DBManager::trackCR).trackTime] = Dingo::Utilities::durationToString(d_sqlite3.getInt64Value(7));
      row[(*Dingo::DBManager::trackCR).trackFileExtension] = d_sqlite3.getStringValue(8);
      row[(*Dingo::DBManager::trackCR).trackBitrate] = Dingo::Utilities::intToString(d_sqlite3.getIntValue(9)) + " kbps";
      row[(*Dingo::DBManager::trackCR).trackMIMEType] = d_sqlite3.getStringValue(10);
      row[(*Dingo::DBManager::trackCR).trackURI] = d_sqlite3.getStringValue(11);
      row[(*Dingo::DBManager::trackCR).trackSubtitleURI] = d_sqlite3.getStringValue(12);
    }
  
    d_sqlite3.clearStatementBindings();
    
    d_sqlite3.resetStatement();
  }
  
  d_sqlite3.finalizeStatement();
  
  //our purpose here is to divide the whole
  //all_tracks_info_vector into 998 ele chunks.
  if (playlist_id != 0) {
    d_sqlite3.prepareStatement("INSERT INTO CoreTracksPlaylists (TrackID, PlaylistID) SELECT CoreTracks.TrackID, :PlaylistID FROM CoreTracks WHERE CoreTracks.TrackURI = :TrackURI;");
    
    for (unsigned int i = 0; i != all_tracks_info_vector.size(); ++i) { 
      d_sqlite3.bindParaInt(":PlaylistID", playlist_id);
      
      d_sqlite3.bindParaText(":TrackURI", all_tracks_info_vector[i][12]);
    
      d_sqlite3.stepStatement();
      
      d_sqlite3.clearStatementBindings();
      
      d_sqlite3.resetStatement();
    }
    
    d_sqlite3.finalizeStatement();
  }
}

void Dingo::DBManager::createDatabaseFile() {
  d_sqlite3.open(d_database_path);
  
  d_sqlite3.quickExecute("BEGIN TRANSACTION;");
  
  d_sqlite3.quickExecute("CREATE TABLE CoreArtists (ArtistID INTEGER PRIMARY KEY, ArtistName TEXT CHECK(ArtistName <> '') UNIQUE NOT NULL);");
    
  d_sqlite3.quickExecute("CREATE TABLE CoreAlbums (AlbumID INTEGER PRIMARY KEY, AlbumName TEXT CHECK(AlbumName <> '') UNIQUE NOT NULL);");
    
  d_sqlite3.quickExecute("CREATE TABLE CoreGenres (GenreID INTEGER PRIMARY KEY, GenreName TEXT CHECK(GenreName <> '') UNIQUE NOT NULL);");
    
  d_sqlite3.quickExecute("CREATE TABLE CoreYears (YearID INTEGER PRIMARY KEY, YearName TEXT CHECK(YearName <> '') UNIQUE NOT NULL);");
    
  d_sqlite3.quickExecute("CREATE TABLE CoreTracks (TrackID INTEGER PRIMARY KEY, ArtistID INTEGER DEFAULT 1 NOT NULL REFERENCES CoreArtists(ArtistID) ON DELETE SET DEFAULT, AlbumID INTEGER DEFAULT 1 NOT NULL REFERENCES CoreAlbums(AlbumID) ON DELETE SET DEFAULT, GenreID INTEGER DEFAULT 1 NOT NULL REFERENCES CoreGenres(GenreID) ON DELETE SET DEFAULT, YearID INTEGER DEFAULT 1 NOT NULL REFERENCES CoreYears(YearID) ON DELETE SET DEFAULT, TrackNumber INTEGER, TrackTitle TEXT CHECK(TrackTitle <> '') NOT NULL, TrackComment TEXT DEFAULT '', TrackLyrics TEXT DEFAULT '', TrackTime INTEGER NOT NULL, TrackSize INTEGER NOT NULL, TrackFileName TEXT CHECK(TrackFileName <> '') NOT NULL, TrackFileExtension TEXT CHECK(TrackFileExtension <> '') NOT NULL, TrackURI TEXT CHECK(TrackURI <> '') UNIQUE NOT NULL, TrackSubtitleURI TEXT DEFAULT '', TrackBitrate INTEGER NOT NULL, TrackMIMEType TEXT CHECK(TrackMIMEType <> '') NOT NULL, TrackDateAdded TEXT CHECK(TrackDateAdded <> '') NOT NULL);");
    
  d_sqlite3.quickExecute("CREATE TABLE CorePlaylists (PlaylistID INTEGER PRIMARY KEY, PlaylistName TEXT CHECK(PlaylistName <> '') NOT NULL);");
    
  d_sqlite3.quickExecute("CREATE TABLE CoreTracksPlaylists (TrackID INTEGER NOT NULL REFERENCES CoreTracks(TrackID) ON DELETE CASCADE, PlaylistID INTEGER NOT NULL REFERENCES CorePlaylists(PlaylistID) ON DELETE CASCADE, PRIMARY KEY (TrackID, PlaylistID));");
    
  //Now let's do some default insertation:
  d_sqlite3.quickExecute("INSERT INTO CoreArtists (ArtistName) VALUES ('Unknown Artist');");
    
  d_sqlite3.quickExecute("INSERT INTO CoreAlbums (AlbumName) VALUES ('Unknown Album');");
    
  d_sqlite3.quickExecute("INSERT INTO CoreGenres (GenreName) VALUES ('Unknown Genre');");
    
  d_sqlite3.quickExecute("INSERT INTO CoreYears (YearName) VALUES ('Unknown Year');");
    
  d_sqlite3.quickExecute("INSERT INTO CorePlaylists (PlaylistName) VALUES ('Play Queue');");
  
  d_sqlite3.quickExecute("COMMIT TRANSACTION;");
}
