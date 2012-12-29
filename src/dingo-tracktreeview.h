/* 
 * dingo-tracktreeview.h
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

#ifndef INCLUDED_DINGO_TRACKTREEVIEW_H
#define INCLUDED_DINGO_TRACKTREEVIEW_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#include "dingo-header.h"
#include "dingo-trackinfowindow.h"
#include "dingo-multipletracksinfowindow.h"
#include "dingo-trackplaylisteditwindow.h"

#include "dingo-toolbarobserver.h"
#include "dingo-searchentryobserver.h"
#include "dingo-audiovideofilterobserver.h"
#include "dingo-playlisttreeviewobserver.h"
#include "dingo-genretreeviewobserver.h"
#include "dingo-artisttreeviewobserver.h"
#include "dingo-yeartreeviewobserver.h"
#include "dingo-albumtreeviewobserver.h"
#include "dingo-enginenavigationobserver.h"
#include "dingo-repeatmodeobserver.h"
#include "dingo-trackinfowindowobserver.h"
#include "dingo-trackplaylisteditwindowobserver.h"
#include "dingo-dbmantrackobserver.h"
#include "dingo-settingsactionobserver.h"

namespace Dingo {
  class DBManager;
  class PlayerEngine;
  class Settings;
  class TrackTreeViewObserver;
  class StatusBar;
  
  class TrackTreeView : public Gtk::TreeView, public ToolBarObserver, public SearchEntryObserver, public AudioVideoFilterObserver, public PlaylistTreeViewObserver, public GenreTreeViewObserver, public ArtistTreeViewObserver, public YearTreeViewObserver, public AlbumTreeViewObserver, public RepeatModeObserver, public EngineNavigationObserver, public TrackInfoWindowObserver, public TrackPlaylistEditWindowObserver, public DBManTrackObserver, public SettingsActionObserver {
    public:
      TrackTreeView();
      TrackTreeView(DBManager* dbmanager, PlayerEngine* playerengine, Settings* settings, StatusBar* statusbar);
      virtual ~TrackTreeView();
      
      //OBSERVER'S METHODS
      void registerTrackTreeViewObserver(TrackTreeViewObserver* observer);
      void removeTrackTreeViewObserver(TrackTreeViewObserver* observer);
      void notifyTrackTreeViewObserver(TreeViewSignals signal_type);
      
      void delegateTrackPlaylistEditWindowObserver(TrackPlaylistEditWindowObserver* observer);
      
      //PUBLIC ACCESSORS
      void setColumnVisible(int treeview_column_id, bool is_visible);
      
      bool getIsAnyTrackSelected();
      
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
      void updateAlbumsSelected();
      
      void updateRepeatOff();
      void updateRepeatSingle();
      void updateRepeatPlaylist();
      void updateRepeatSection();
      void updateRepeatSectionOff();
      
      void updateEngineTrackNext();
      void updateEngineTrackPrevious();
      void updateEngineTrackCurrent();
      
      void updateGetNextTrackInfo();
      void updateGetPreviousTrackInfo();
      
      void updateTrackPlaylistEditInfoSaved();
      void updateChangesInCoreTracks();
      
      void updateSettingsRead();
      void updateSettingsWrite();
      
      //PUBLIC SIGNAL HANDLERS
      void onEditInfoActivated();
      void onEditAllInfoActivated();
      void onLyricsActivated();
      
      void onAddToSubMenuActivated();
      void onAddToPlayQueueActivated();
      void onAddToNormalPlaylistActivated();
      void onDeleteFromSubMenuActivated();
      void onDeleteFromDatabaseActivated();
      void onDeleteFromPlayQueueActivated();
      void onDeleteFromHardDriveActivated();
      void onDeleteFromCurrentPlaylistActivated();
      void onDeleteFromNormalPlaylistActivated();
      
      void onCutTracksActivated();
      void onCopyTracksActivated();
      void onPasteTracksActivated();
      
      //PUBLIC ACCESSORS
      void activateRow(Gtk::TreeRowReference row_ref);
      bool activateRow(Glib::ustring track_uri);
      void activateCurrentPlaylist();
      std::vector<Gtk::TreeRowReference> getSelectionRefs();
      
      //UTILITIES
      void refreshData();
    
    private:
      //CREATORS
      void createTrackTreeView();
      void createPopupMenu();
      void createAddSubMenu();
      void createDeleteSubMenu();
      
      //SIGNAL HANDLERS
      void onTreeViewRowDoubleClicked(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
      
      void filterTracks();
      bool on_button_press_event(GdkEventButton* event);
      
      int onTrackTitleCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter);
      int onTrackGenreCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter);
      int onTrackArtistCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter);
      int onTrackAlbumCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter);
      int onTrackYearCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter);
      int onTrackFileExtensionCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter);
      int onTrackBitrateCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter);
      
      //UTILITIES
      void saveNextTrackRowRef();
      void savePreviousTrackRowRef();
      
      void highlightRow(Gtk::TreeModel::iterator iter);
      void unhighlightRow(Gtk::TreeModel::iterator iter);
      
      //DATA MEMBERS
      Glib::RefPtr<Gtk::TreeModelFilter> d_track_filter;
      Glib::RefPtr<Gtk::TreeModelSort> d_track_sort;
      
      //MENU ITEMS
      Gtk::Menu track_popup_menu, add_submenu, delete_submenu;
      Gtk::ImageMenuItem add_to, delete_from, edit_info, edit_all_info;
      Gtk::ImageMenuItem cut_tracks, copy_tracks, paste_tracks;
      Gtk::ImageMenuItem add_play_queue, add_playlists;
      Gtk::ImageMenuItem delete_database, delete_play_queue, delete_hard_drive, delete_current_playlist, delete_playlists;
      Gtk::ImageMenuItem lyrics;
      
      std::vector<TrackTreeViewObserver*> track_obs_vector;
      Gtk::TreeRowReference cur_track_row_ref;
      
      Gtk::TreeRowReference cur_edit_row_ref;
      Gtk::TreeRowReference cur_next_edit_row_ref;
      Gtk::TreeRowReference cur_prev_edit_row_ref;
      std::vector<Gtk::TreeRowReference> cur_edit_row_vector;
      std::map<int, std::vector<Gtk::TreeRowReference> > d_selected_store_map;
      TrackInfoWindow d_trackinfowindow;
      MultipleTracksInfoWindow d_multipleinfowindow;
      TrackPlaylistEditWindow d_trackplaylisteditwindow;
      
      Gtk::Image d_pausing_image, d_playing_image, d_status_header_image;
      
      DBManager* d_dbmanager;
      PlayerEngine* d_playerengine;
      Settings* d_settings;
      StatusBar* d_statusbar;
      int d_playing_track_id;
  };
}

#endif //INCLUDED_DINGO_TRACKTREEVIEW_H
