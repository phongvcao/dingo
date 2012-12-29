/* 
 * dingo-trackplaylisteditwindow.h
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

#ifndef INCLUDED_DINGO_TRACKPLAYLISTEDITWINDOW_H
#define INCLUDED_DINGO_TRACKPLAYLISTEDITWINDOW_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#include "dingo-header.h"
#include "dingo-dbmanplaylistobserver.h"

namespace Dingo {
  class DBManager;
  class TrackPlaylistEditWindowObserver;

  class TrackPlaylistEditWindow : public Gtk::Window, public DBManPlaylistObserver {
    public:
      TrackPlaylistEditWindow();
      TrackPlaylistEditWindow(DBManager* dbmanager);
      virtual ~TrackPlaylistEditWindow();
      
      //OBSERVER'S METHODS
      void registerTrackPlaylistEditWindowObserver(TrackPlaylistEditWindowObserver* observer);
      void removeTrackPlaylistEditWindowObserver(TrackPlaylistEditWindowObserver* observer);
      void notifyTrackPlaylistEditWindowObserver(TrackPlaylistEditWindowSignals signal_type);
      
      //INHERITED OBSERVER'S METHODS
      void updateChangesInCorePlaylists();
      
      //PUBLIC METHODS
      void setDisplayTracks(std::vector<Gtk::TreeRowReference> selected_row_ref_vector);
    
    private:
      //CREATORS
      void createIntroLabel();
      void createTrackPlaylistTreeView();
      void createButtonsHBox();
      
      //UTILITIES
      Glib::ustring convertToTrackTitleString(std::map<int, Glib::ustring> remaining_trackid_tracktitle_map);
      Glib::ustring getTracksAddingDeletingString(Gtk::TreeModel::iterator child_tree_iter);
      void setCheckBoxColumnHeaderCheckButtonStatus();
      void setTextBoldTrackCount(Gtk::TreeModel::iterator child_iter, int count_diff);
      
      void resetData();
      
      //SIGNAL HANDLERS
      int onPlaylistNameCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter);
      int onTrackChangesCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter);
      
      void onTrackPlaylistEditRowModify(const Gtk::TreeModel::iterator& iter, Glib::ValueBase& value, int column_id);
      void onTrackPlaylistEditCheckBoxToggled(Glib::ustring tree_path);
      void onTrackPlaylistTreeViewRowDoubleClicked(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
      void onCheckBoxColumnHeaderClicked();
      void onCheckBoxColumnHeaderCheckButtonToggled();
      bool onTrackPlaylistTreeViewQueryRowTooltip(int x, int y, bool is_tooltip_keyboard, const Glib::RefPtr<Gtk::Tooltip>& tooltip);
      void onCancelButtonClicked();
      void onSaveButtonClicked();
      void onSearchEntryChanged();
      
      void filterPlaylists();
      
      void onIconPressEvent(Gtk::EntryIconPosition icon_position, const GdkEventButton* event);
      bool onSearchEntryEnterNotifyEvent(GdkEventCrossing* event);
      bool onSearchEntryLeaveNotifyEvent(GdkEventCrossing* event);
    
      //DATA MEMBERS
      Gtk::VBox top_vbox;
      Gtk::HBox buttons_hbox;
      Gtk::ScrolledWindow d_scrolledwin;
      Gtk::TreeView d_trackplaylisttreeview;
      Gtk::Label d_intro_label;
      Gtk::Button d_cancel_button, d_save_button;
      Gtk::Entry d_search_entry;
      Gtk::CheckButton d_col_header_check;
      Gtk::HBox intro_separator_hbox, separator_hbox;
      
      Glib::RefPtr<Gtk::TreeModelFilter> d_trackplaylist_filter;
      Glib::RefPtr<Gtk::TreeModelFilter> d_trackplaylist_modify_filter;
      Glib::RefPtr<Gtk::TreeModelSort> d_trackplaylist_sort;
      
      Glib::ustring d_all_tracks_title_str;
      Gtk::CellRendererToggle* p_renderer;
      sigc::connection d_renderer_toggle_connection;
      sigc::connection d_col_header_check_connection;
      sigc::connection d_col_header_click_connection;
      std::map<int, Glib::ustring> d_all_trackid_tracktitle_map;
      std::map<int, std::vector<int> > d_playlistid_trackid_map;
      
      DBManager* d_dbmanager;
      std::vector<Dingo::TrackPlaylistEditWindowObserver*> trackplaylistedit_obs_vector;
  };
}

#endif //INCLUDED_DINGO_TRACKPLAYLISTEDITWINDOW_H
