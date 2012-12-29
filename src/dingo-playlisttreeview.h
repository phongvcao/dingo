#ifndef INCLUDED_DINGO_PLAYLISTTREEVIEW_H
#define INCLUDED_DINGO_PLAYLISTTREEVIEW_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#include "dingo-header.h"
#include "dingo-dbmanplaylistobserver.h"
#include "dingo-trackplaylisteditwindowobserver.h"

namespace Dingo {
  class DBManager;
  class Settings;
  class StatusBar;
  class PlaylistTreeViewObserver;

  class PlaylistTreeView : public Gtk::TreeView, public DBManPlaylistObserver, public TrackPlaylistEditWindowObserver {
    public:
      PlaylistTreeView();
      PlaylistTreeView(DBManager* dbmanager, Settings* settings, StatusBar* statusbar);
      virtual ~PlaylistTreeView();
      
      //OBSERVER'S METHODS
      void registerPlaylistTreeViewObserver(PlaylistTreeViewObserver* observer);
      void removePlaylistTreeViewObserver(PlaylistTreeViewObserver* observer);
      void notifyPlaylistTreeViewObserver(TreeViewSignals signal_type);
      
      //INHERITED OBSERVER'S METHODS
      void updateChangesInCorePlaylists();
      void updateTrackPlaylistEditInfoSaved();
      
      //PUBLIC SIGNAL HANDLERS
      void onNewPlaylistActivated();
      void onImportPlaylistActivated();
      void onRenamePlaylistActivated();
      void onExportPlaylistActivated();
      void onDeletePlaylistActivated();
      
      //PUBLIC METHODS
      void activatePlaylist(int playlist_id);
      
      //UTILITIES
      void refreshData();
      
    private:
      //CREATORS
      void createPlaylistTreeView();
      void createPlaylistTreeMenu();
      
      //SIGNAL HANDLERS
      void onSelectionChanged();
      
      void filterPlaylists();
      void onPlaylistRowModify(const Gtk::TreeModel::iterator& iter, Glib::ValueBase& value, int column_id);
      int onPlaylistNameCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter);
      
      bool on_button_press_event(GdkEventButton* event);
      
      //DATA MEMBERS
      Gtk::Menu playlist_menu;
      Gtk::ImageMenuItem playlist_new, playlist_import, playlist_rename, playlist_export, playlist_delete;
      
      bool is_playlist_name_filtered;
      bool is_started_up;
      std::vector<PlaylistTreeViewObserver*> playlist_obs_vector;
      Glib::RefPtr<Gtk::TreeModelSort> d_playlist_sort;
      Glib::RefPtr<Gtk::TreeModelFilter> d_playlist_modify_filter;
      
      Glib::ustring d_last_import_folder_uri, d_last_export_folder_uri;
      
      DBManager* d_dbmanager;
      Settings* d_settings;
      StatusBar* d_statusbar;
  };
}

#endif //INCLUDED_DINGO_PLAYLISTTREEVIEW_H
