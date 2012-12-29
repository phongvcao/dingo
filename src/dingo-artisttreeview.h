/* 
 * dingo-artisttreeview.h
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

#ifndef INCLUDED_DINGO_ARTISTTREEVIEW_H
#define INCLUDED_DINGO_ARTISTTREEVIEW_H

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
#include "dingo-dbmanartistobserver.h"
#include "dingo-trackplaylisteditwindowobserver.h"

namespace Dingo {
  class DBManager;
  class Settings;
  class StatusBar;
  class ArtistTreeViewObserver;
  
  class ArtistTreeView : public Gtk::TreeView, public ToolBarObserver, public SearchEntryObserver, public AudioVideoFilterObserver, public PlaylistTreeViewObserver, public GenreTreeViewObserver, public DBManArtistObserver, public TrackPlaylistEditWindowObserver {
    public:
      ArtistTreeView();
      ArtistTreeView(DBManager* dbmanager, Settings* settings, StatusBar* statusbar);
      virtual ~ArtistTreeView();
      
      //OBSERVER'S METHODS
      void registerArtistTreeViewObserver(ArtistTreeViewObserver* observer);
      void removeArtistTreeViewObserver(ArtistTreeViewObserver* observer);
      void notifyArtistTreeViewObserver(TreeViewSignals signal_type);
      
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
      
      void updateChangesInCoreArtists();
      void updateTrackPlaylistEditInfoSaved();
      
      //UTILITIES
      void refreshData();
    
    private:
      //CREATORS
      void createArtistTreeView();
      
      //SIGNAL HANDLERS
      void onSelectionChanged();
      
      void filterArtists();
      void onArtistRowModify(const Gtk::TreeModel::iterator& iter, Glib::ValueBase& value, int column_id);
      int onArtistNameCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter);
      
      //UTILITIES
      std::vector<Gtk::TreeRowReference> getSelectionRefs();
      
      //DATA MEMBERS
      std::vector<ArtistTreeViewObserver*> artist_obs_vector;
      Glib::RefPtr<Gtk::TreeModelFilter> d_artist_filter;
      Glib::RefPtr<Gtk::TreeModelSort> d_artist_sort;
      Glib::RefPtr<Gtk::TreeModelFilter> d_artist_modify_filter;
      int d_visible_artists_count;
      sigc::connection artists_select_connection;
      std::map<int, std::vector<Gtk::TreeRowReference> > d_selected_store_map;
      
      DBManager* d_dbmanager;
      Settings* d_settings;
      StatusBar* d_statusbar;
  };
}

#endif //INCLUDED_DINGO_ARTISTTREEVIEW_H
