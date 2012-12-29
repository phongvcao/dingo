#ifndef INCLUDED_DINGO_ALBUMTREEVIEW_H
#define INCLUDED_DINGO_ALBUMTREEVIEW_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#include "dingo-header.h"
#include "dingo-toolbarobserver.h"
#include "dingo-searchentryobserver.h"
#include "dingo-audiovideofilterobserver.h"
#include "dingo-playlisttreeviewobserver.h"
#include "dingo-genretreeviewobserver.h"
#include "dingo-artisttreeviewobserver.h"
#include "dingo-yeartreeviewobserver.h"
#include "dingo-dbmanalbumobserver.h"
#include "dingo-trackplaylisteditwindowobserver.h"

namespace Dingo {
  class DBManager;
  class Settings;
  class StatusBar;
  class AlbumTreeViewObserver;
  
  class AlbumTreeView : public Gtk::TreeView, public ToolBarObserver, public SearchEntryObserver, public AudioVideoFilterObserver, public PlaylistTreeViewObserver, public GenreTreeViewObserver, public ArtistTreeViewObserver, public YearTreeViewObserver, public DBManAlbumObserver, public TrackPlaylistEditWindowObserver {
    public:
      AlbumTreeView();
      AlbumTreeView(DBManager* dbmanager, Settings* settings, StatusBar* statusbar);
      virtual ~AlbumTreeView();
      
      //OBSERVER'S METHODS
      void registerAlbumTreeViewObserver(AlbumTreeViewObserver* observer);
      void removeAlbumTreeViewObserver(AlbumTreeViewObserver* observer);
      void notifyAlbumTreeViewObserver(TreeViewSignals signal_type);
      
      //INHERITED OBSERVER'S METHODS
      void updateHomeToggleActive();
      void updateNowPlayingToggleActive();
      void updatePlayQueueToggleActive();
      
      void updateSearchEntryChanged();
      void updateAudioFilterToggled();
      void updateVideoFilterToggled();
      
      void updateBlockSelection();
      void updateRestoreSelectedRows();
      void updateUnblockSelection();
      
      void updateGenresSelected();
      void updateArtistsSelected();
      void updateYearsSelected();
      
      void updateChangesInCoreAlbums();
      void updateTrackPlaylistEditInfoSaved();
      
      //UTILITIES
      void refreshData();
    
    private:
      //CREATORS
      void createAlbumTreeView();
      
      //SIGNAL HANDLERS
      void onSelectionChanged();
      
      void filterAlbums();
      void onAlbumRowModify(const Gtk::TreeModel::iterator& iter, Glib::ValueBase& value, int column_id);
      int onAlbumNameCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter);
      
      //UTILITIES
      std::vector<Gtk::TreeRowReference> getSelectionRefs();
      
      //DATA MEMBERS
      std::vector<AlbumTreeViewObserver*> album_obs_vector;
      Glib::RefPtr<Gtk::TreeModelFilter> d_album_filter;
      Glib::RefPtr<Gtk::TreeModelSort> d_album_sort;
      Glib::RefPtr<Gtk::TreeModelFilter> d_album_modify_filter;
      int d_visible_albums_count;
      sigc::connection albums_select_connection;
      std::map<int, std::vector<Gtk::TreeRowReference> > d_selected_store_map;
      bool d_is_restoring_playlist_selection;
      
      DBManager* d_dbmanager;
      Settings* d_settings;
      StatusBar* d_statusbar;
  };
}

#endif //INCLUDED_DINGO_ALBUMTREEVIEW_H
