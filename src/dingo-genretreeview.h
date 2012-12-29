/* 
 * dingo-genretreeview.h
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

#ifndef INCLUDED_DINGO_GENRETREEVIEW_H
#define INCLUDED_DINGO_GENRETREEVIEW_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#include "dingo-header.h"
#include "dingo-toolbarobserver.h"
#include "dingo-searchentryobserver.h"
#include "dingo-audiovideofilterobserver.h"
#include "dingo-playlisttreeviewobserver.h"
#include "dingo-dbmangenreobserver.h"
#include "dingo-trackplaylisteditwindowobserver.h"

namespace Dingo {
  class DBManager;
  class Settings;
  class GenreTreeViewObserver;
  
  class GenreTreeView : public Gtk::TreeView, public ToolBarObserver, public SearchEntryObserver, public AudioVideoFilterObserver, public PlaylistTreeViewObserver, public DBManGenreObserver, public TrackPlaylistEditWindowObserver {
    public:
      GenreTreeView();
      GenreTreeView(DBManager* dbmanager, Settings* settings);
      virtual ~GenreTreeView();
      
      //OBSERVER'S METHODS
      void registerGenreTreeViewObserver(GenreTreeViewObserver* observer);
      void removeGenreTreeViewObserver(GenreTreeViewObserver* observer);
      void notifyGenreTreeViewObserver(TreeViewSignals signal_type);
      
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
      
      void updateChangesInCoreGenres();
      void updateTrackPlaylistEditInfoSaved();
      
      //UTILITIES
      void refreshData();
    
    private:
      //CREATORS
      void createGenreTreeView();
      
      //SIGNAL HANDLERS
      void onSelectionChanged();
      
      void filterGenres();
      void onGenreRowModify(const Gtk::TreeModel::iterator& iter, Glib::ValueBase& value, int column_id);
      int onGenreNameCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter);
      
      //UTILITIES
      std::vector<Gtk::TreeRowReference> getSelectionRefs();
      
      //DATA MEMBERS
      std::vector<GenreTreeViewObserver*> genre_obs_vector;
      Glib::RefPtr<Gtk::TreeModelFilter> d_genre_filter;
      Glib::RefPtr<Gtk::TreeModelSort> d_genre_sort;
      Glib::RefPtr<Gtk::TreeModelFilter> d_genre_modify_filter;
      int d_visible_genres_count;
      sigc::connection genres_select_connection;
      std::map<int, std::vector<Gtk::TreeRowReference> > d_selected_store_map;
      
      DBManager* d_dbmanager;
      Settings* d_settings;
  };
}

#endif //INCLUDED_DINGO_GENRETREEVIEW_H
