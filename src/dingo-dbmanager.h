/* 
 * dingo-dbmanager.h
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

#ifndef INCLUDED_DINGO_DBMANAGER_H
#define INCLUDED_DINGO_DBMANAGER_H

#include "dingo-sqlite3.h"
#include "dingo-tagreader.h"
#include "dingo-processgridobserver.h"
#include "dingo-header.h"

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_SET
#define INCLUDED_SET
#include <set>
#endif //INCLUDED_SET

namespace Dingo {
  class DBManPlaylistObserver;
  class DBManGenreObserver;
  class DBManArtistObserver;
  class DBManYearObserver;
  class DBManAlbumObserver;
  class DBManTrackObserver;
  class ProcessGrid;

  class DBManager : public ProcessGridObserver {
    public:
      DBManager();
      DBManager(Glib::ustring database_path);
      virtual ~DBManager();
      
      //OBSERVER'S METHODS
      void registerDBManPlaylistObserver(DBManPlaylistObserver* observer);
      void removeDBManPlaylistObserver(DBManPlaylistObserver* observer);
      void notifyDBManPlaylistObserver(DBManSignals signal_type);
      
      void registerDBManGenreObserver(DBManGenreObserver* observer);
      void removeDBManGenreObserver(DBManGenreObserver* observer);
      void notifyDBManGenreObserver(DBManSignals signal_type);
      
      void registerDBManArtistObserver(DBManArtistObserver* observer);
      void removeDBManArtistObserver(DBManArtistObserver* observer);
      void notifyDBManArtistObserver(DBManSignals signal_type);
      
      void registerDBManYearObserver(DBManYearObserver* observer);
      void removeDBManYearObserver(DBManYearObserver* observer);
      void notifyDBManYearObserver(DBManSignals signal_type);
      
      void registerDBManAlbumObserver(DBManAlbumObserver* observer);
      void removeDBManAlbumObserver(DBManAlbumObserver* observer);
      void notifyDBManAlbumObserver(DBManSignals signal_type);
      
      void registerDBManTrackObserver(DBManTrackObserver* observer);
      void removeDBManTrackObserver(DBManTrackObserver* observer);
      void notifyDBManTrackObserver(DBManSignals signal_type);
      
      //INHERITED OBSERVER'S METHODS
      void updateProcessFinished(ProcessID process_id);
      
      //PUBLIC ACCESSORS
      void setProcessGrid(ProcessGrid* processgrid);
      Gtk::Grid* getProcessGrid();
      
      void setMainWindowVisible(bool is_visible);
      bool getMainWindowVisible();
      
      void saveMainWindowPosition();
      
      bool getIsAnyTrackInClipboard();
      
      //Dingo::MPRISCommon's METHODS
      Gtk::TreeRowReference getFirstAddedTrackRowRef();
      int getLastImportedPlaylistID();
      
      std::map<int, Glib::ustring> getPlaylists(int max_count, bool is_reverse);
      int getPlaylistsCount();
      Glib::ustring getCurrentPlaylistName();
      
      //Dingo::MenuBar's METHODS
      void addTracksToDatabase(std::vector<Glib::ustring> uris_vector, int playlist_id = 0);
      void addTracksFolderToDatabase(std::vector<Glib::ustring> folder_uris_vector);
      
      //Dingo::ToolBar's METHODS
      void setSearchText(Glib::ustring search_text);
      void setMIMETypeFilter(Glib::ustring mimetype_filter_str);
      Glib::ustring getMIMETypeFilter();
      
      //Dingo::PlaylistTreeView's METHODS
      void setCurrentPlaylistID(int playlist_id);
      int getCurrentPlaylistID();
      
      void setPreviousPlaylistID(int playlist_id);
      int getPreviousPlaylistID();
      
      int createPlaylist(Glib::ustring playlist_name);
      void importPlaylists(std::vector<Glib::ustring> uris_vector);
      void renamePlaylist(int playlist_id, Glib::ustring new_playlist_name);
      void exportPlaylist(Glib::ustring fileuri, int playlist_id, Glib::ustring playlist_name);
      void deletePlaylist(int playlist_id);
      
      void prepareQueryCountTracksInPlaylist();
      int getCountTracksInPlaylist(int playlist_id);
      void finalizeQueryCountTracksInPlaylist();
      
      //Dingo::GenreTreeView's METHODS
      void setCurrentGenresID(std::vector<Gtk::TreeRowReference> ref_vector);
      
      void prepareQueryCountTracksInGenre();
      int getCountTracksInGenre(int genre_id);
      void finalizeQueryCountTracksInGenre();
      
      //Dingo::ArtistTreeView's METHODS
      void setCurrentArtistsID(std::vector<Gtk::TreeRowReference> ref_vector);
      
      void prepareQueryCountTracksInArtist();
      int getCountTracksInArtist(int artist_id);
      void finalizeQueryCountTracksInArtist();
      
      //Dingo::YearTreeView's METHODS
      void setCurrentYearsID(std::vector<Gtk::TreeRowReference> ref_vector);
      
      void prepareQueryCountTracksInYear();
      int getCountTracksInYear(int year_id);
      void finalizeQueryCountTracksInYear();
      
      //Dingo::AlbumTreeView's METHODS
      void setCurrentAlbumsID(std::vector<Gtk::TreeRowReference> ref_vector);
      
      void prepareQueryCountTracksInAlbum();
      int getCountTracksInAlbum(int album_id);
      void finalizeQueryCountTracksInAlbum();
      
      //Dingo::TrackTreeView's METHODS
      //done with optimization...
      void addTracksToPlayQueue(std::vector<int> add_tracks_id_vector);
      void addTracksToNormalPlaylist(std::vector<int> add_tracks_id_vector, int playlist_id);
      void deleteTracksFromDatabase(std::vector<int> delete_tracks_id_vector);
      void deleteTracksFromPlayQueue(std::vector<int> remove_tracks_id_vector);
      void deleteTracksFromHardDrive(std::vector<int> delete_tracks_id_vector);
      void deleteTracksFromNormalPlaylist(std::vector<int> remove_tracks_id_vector, int playlist_id);
      void cutTracks(std::vector<int> clipboard_tracks_id_vector);
      void copyTracks(std::vector<int> clipboard_tracks_id_vector);
      void pasteTracks();
      
      void prepareQueryShouldTrackBeVisible();
      bool getShouldTrackBeVisible(int track_id);
      void finalizeQueryShouldTrackBeVisible();
      
      Glib::ustring getPlaylistInfo();
      
      void setCurrentTrackRowRef(Gtk::TreeRowReference cur_track_row_ref);
      Gtk::TreeRowReference getCurrentTrackRowRef();
      
      //Dingo::TrackPlaylistEditWindow's METHODS
      //done with optimization...
      void prepareQueryTrackInPlaylist();
      bool getTrackInPlaylist(int track_id, int playlist_id);
      void finalizeQueryTrackInPlaylist();
      
      //Dingo::TrackInfoWindow's METHODS
      std::map<Glib::ustring, Glib::ustring> getTrackInfo(int track_id);
      void setTrackInfo(Gtk::TreeRowReference cur_edit_row_ref, std::map<Glib::ustring, Glib::ustring> changed_values_map);
      
      //Dingo::MultipleTracksInfoWindow's METHODS
      //done with optimization...
      std::map<Glib::ustring, Glib::ustring> getMultipleTracksInfo(std::vector<int> tracks_id_vector);
      void setMultipleTracksInfo(std::vector<Gtk::TreeRowReference> cur_row_ref_vector, std::map<Glib::ustring, Glib::ustring> changed_values_map);
      
      //Dingo::Player's METHODS      
      void setWindow(Gtk::Window* window);
      Gtk::Window* getWindow();
      
      //the models are public static
      static Glib::RefPtr<Gtk::ListStore> playlistModel;
      static std::auto_ptr<PlaylistColumnRecord> playlistCR;
      static Glib::RefPtr<Gtk::ListStore> genreModel;
      static std::auto_ptr<GenreColumnRecord> genreCR;
      static Glib::RefPtr<Gtk::ListStore> artistModel;
      static std::auto_ptr<ArtistColumnRecord> artistCR;
      static Glib::RefPtr<Gtk::ListStore> yearModel;
      static std::auto_ptr<YearColumnRecord> yearCR;
      static Glib::RefPtr<Gtk::ListStore> albumModel;
      static std::auto_ptr<AlbumColumnRecord> albumCR;
      static Glib::RefPtr<Gtk::ListStore> trackModel;
      static std::auto_ptr<TrackColumnRecord> trackCR;
      static Glib::RefPtr<Gtk::ListStore> trackplaylistModel;
      static std::auto_ptr<TrackPlaylistColumnRecord> trackplaylistCR;
      
    private:
      //LOADING LISTSTORE
      void createDatabase();
      void createPlaylistModel();
      void createGenreModel();
      void createArtistModel();
      void createYearModel();
      void createAlbumModel();
      void createTrackModel();
      void createTrackPlaylistModel();
      
      //SIGNAL HANDLERS
      void updateGUI(bool is_blocking = true);
      
      //UTILITIES
      //done with optimization...
      void addTracksFolderRecursively(Glib::ustring folder_path, std::vector<std::vector<Glib::ustring> >& all_tracks_info_vector, std::set<Glib::ustring>& all_tracks_genre_set, std::set<Glib::ustring>& all_tracks_artist_set, std::set<Glib::ustring>& all_tracks_year_set, std::set<Glib::ustring>& all_tracks_album_set);
      void insertGenresToDatabase(std::set<Glib::ustring>& all_tracks_genre_set, ProcessID process_id);
      void insertArtistsToDatabase(std::set<Glib::ustring>& all_tracks_artist_set, ProcessID process_id);
      void insertYearsToDatabase(std::set<Glib::ustring>& all_tracks_year_set, ProcessID process_id);
      void insertAlbumsToDatabase(std::set<Glib::ustring>& all_tracks_album_set, ProcessID process_id);
      void insertTracksToDatabase(std::vector<std::vector<Glib::ustring> >& all_tracks_info_vector, ProcessID process_id, int playlist_id = 0);
      void createDatabaseFile();
      
      //DATA MEMBERS
      SQLite3 d_sqlite3;
      TagReader d_tagreader;
      std::vector<DBManPlaylistObserver*> dbman_playlist_obs_vector;
      std::vector<DBManGenreObserver*> dbman_genre_obs_vector;
      std::vector<DBManArtistObserver*> dbman_artist_obs_vector;
      std::vector<DBManYearObserver*> dbman_year_obs_vector;
      std::vector<DBManAlbumObserver*> dbman_album_obs_vector;
      std::vector<DBManTrackObserver*> dbman_track_obs_vector;
      Glib::ustring d_search_text;
      Gtk::Window* parent_window;
      int parent_x, parent_y;
      sigc::connection d_update_gui_connection;
      
      //CURRENT SOURCES
      int cur_playlist_id;
      int prev_playlist_id;
      Gtk::TreeRowReference d_cur_track_row_ref;
      int d_first_added_track_id;
      int d_last_imported_playlist_id;
      std::vector<int> d_clipboard_tracks_id_vector;
      
      std::vector<Gtk::TreeRowReference> cur_genres_ref;
      bool is_all_genres_selected;
      Glib::ustring genres_selected_query;
      
      std::vector<Gtk::TreeRowReference> cur_artists_ref;
      bool is_all_artists_selected;
      Glib::ustring artists_selected_query;
      
      std::vector<Gtk::TreeRowReference> cur_years_ref;
      bool is_all_years_selected;
      Glib::ustring years_selected_query;
      
      std::vector<Gtk::TreeRowReference> cur_albums_ref;
      bool is_all_albums_selected;
      Glib::ustring albums_selected_query;
      
      Gtk::Image d_playlist_image, d_genre_image, d_artist_image, d_year_image, d_album_image;
      
      //int d_x_window_id;
      Glib::ustring d_mimetype_filter_str;
      ProcessGrid* d_processgrid;
      std::vector<bool> d_processid_queue;
      Glib::ustring d_database_path;
  };
}

#endif //INCLUDED_DINGO_DBMANAGER_H
