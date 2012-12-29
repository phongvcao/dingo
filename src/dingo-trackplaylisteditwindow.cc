/* 
 * dingo-trackplaylisteditwindow.cc
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

#include "dingo-trackplaylisteditwindow.h"
#include "dingo-trackplaylisteditwindowobserver.h"
#include "dingo-dbmanager.h"
#include "dingo-utilities.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

#ifndef INCLUDED_GLIBMM_I18N_H
#define INCLUDED_GLIBMM_I18N_H
#include <glibmm/i18n.h>
#endif //INCLUDED_GLIBMM_I18N_H

//CONSTRUCTORS & DESTRUCTOR
Dingo::TrackPlaylistEditWindow::TrackPlaylistEditWindow() :
  Gtk::Window(Gtk::WINDOW_TOPLEVEL)
{

}

Dingo::TrackPlaylistEditWindow::TrackPlaylistEditWindow(Dingo::DBManager* dbmanager) :
  Gtk::Window(Gtk::WINDOW_TOPLEVEL),
  d_intro_label(_("Please check or uncheck the checkboxes to add or delete tracks to/from playlists:")),
  d_cancel_button(_("Cancel")),
  d_save_button(_("Save"))
{
  d_dbmanager = dbmanager;
  d_all_tracks_title_str = Dingo::Utilities::highlightString(_("Tracks to be added to this playlist:")) + " \n";
  
  createIntroLabel();
  createTrackPlaylistTreeView();
  createButtonsHBox();
  
  set_size_request(645, 400);
  set_default_size(645, 445);
  set_title(_("Manage Tracks' Membership"));
  set_deletable(false);
  set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
  
  add(top_vbox);
}

Dingo::TrackPlaylistEditWindow::~TrackPlaylistEditWindow() {

}

//OBSERVER'S METHODS
void Dingo::TrackPlaylistEditWindow::registerTrackPlaylistEditWindowObserver(Dingo::TrackPlaylistEditWindowObserver* observer) {
  trackplaylistedit_obs_vector.push_back(observer);
}

void Dingo::TrackPlaylistEditWindow::removeTrackPlaylistEditWindowObserver(Dingo::TrackPlaylistEditWindowObserver* observer) {
  std::vector<Dingo::TrackPlaylistEditWindowObserver*>::iterator iter = std::find(trackplaylistedit_obs_vector.begin(), trackplaylistedit_obs_vector.end(), observer);
  
  if (iter != trackplaylistedit_obs_vector.end()) {
    trackplaylistedit_obs_vector.erase(iter);
  }
}

void Dingo::TrackPlaylistEditWindow::notifyTrackPlaylistEditWindowObserver(Dingo::TrackPlaylistEditWindowSignals signal_type) {
  switch (signal_type) {
    case Dingo::TRACKPLAYLISTEDIT_INFO_SAVED: {
      for (std::vector<Dingo::TrackPlaylistEditWindowObserver*>::iterator iter = trackplaylistedit_obs_vector.begin(); iter != trackplaylistedit_obs_vector.end(); ++iter) {
        (*iter)->updateTrackPlaylistEditInfoSaved();
      }
      d_trackplaylist_modify_filter->refilter();
      //std::cout << "TrackPlaylistEditWindow Info Saved!" << std::endl;
      break;
    }
    
    default: {
      std::cerr << "Undefined Dingo::TrackPlaylistEditWindowSignals!" << std::endl;
      break;
    }
  }
}

//INHERITED OBSERVER'S METHODS
void Dingo::TrackPlaylistEditWindow::updateChangesInCorePlaylists() {
  filterPlaylists();

  d_trackplaylist_modify_filter->refilter();
}
      
//PUBLIC METHODS
void Dingo::TrackPlaylistEditWindow::setDisplayTracks(std::vector<Gtk::TreeRowReference> selected_row_ref_vector) {
  bool is_all_tracks_title_str_completed = false;
  
  if (d_renderer_toggle_connection.connected()) {
    d_renderer_toggle_connection.disconnect();
  }
  
  Gtk::TreeModel::Children temp_children = Dingo::DBManager::trackplaylistModel->children();
  
  //each time the outer for loop goes through we must acquire
  //the PlaylistID (temp_playlist_id variable below) & the std::vector<int> TrackIDs
  //to put into std::map<int, std::vector<int> > d_playlistid_trackid_map
  d_dbmanager->prepareQueryTrackInPlaylist();
  
  for (Gtk::TreeModel::Children::iterator temp_iter = temp_children.begin(); temp_iter != temp_children.end(); ++temp_iter) {
    bool is_one_track_in_playlist = false;
    bool is_one_track_not_in_playlist = false;
    std::vector<int> temp_track_id_vector;
    
    //this is the Gtk::TreeModel::Row pointing to PlaylistID
    Gtk::TreeModel::Row temp_row = *(temp_iter);
    
    int temp_playlist_id = temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistID];
    Glib::ustring temp_tracks_title_str = "";
    Glib::ustring final_tracks_title_str = "";
    
    //each time this inner loop goes through we must acquire
    //the std::vector<int> that contains TrackIDs of the PlaylistID acquired
    //when the outer PlaylistID went through.
    for (std::vector<Gtk::TreeRowReference>::iterator temp_ref_iter = selected_row_ref_vector.begin(); temp_ref_iter != selected_row_ref_vector.end(); ++temp_ref_iter) {
      //might generate error
      Gtk::TreeModel::Row temp_ref_row = *(Dingo::DBManager::trackModel->get_iter(temp_ref_iter->get_path()));
      
      int temp_track_id = temp_ref_row[(*Dingo::DBManager::trackCR).trackID];
      Glib::ustring temp_track_title = temp_ref_row[(*Dingo::DBManager::trackCR).trackTitle];
      
      if (d_dbmanager->getTrackInPlaylist(temp_track_id, temp_playlist_id)) {
        is_one_track_in_playlist = true;
        temp_tracks_title_str += temp_track_title + " | "; 
        temp_track_id_vector.push_back(temp_track_id);
      }
      
      if (!is_one_track_not_in_playlist) {
        if (!d_dbmanager->getTrackInPlaylist(temp_track_id, temp_playlist_id)) {
          is_one_track_not_in_playlist = true;
        }
      }
      
      if (!is_all_tracks_title_str_completed) {
        d_all_tracks_title_str += temp_track_title + " | ";
        d_all_trackid_tracktitle_map.insert(std::pair<int, Glib::ustring>(temp_track_id, temp_track_title));
      }
    }
    
    is_all_tracks_title_str_completed = true;
    d_playlistid_trackid_map.insert(std::pair<int, std::vector<int> >(temp_playlist_id, temp_track_id_vector));
    
    if ((is_one_track_in_playlist) && (is_one_track_not_in_playlist)) {
      //some tracks are in the playlist
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox] = false;
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistent] = true;
      
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxOriginal] = false;
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistentOriginal] = true;
      
      final_tracks_title_str = Glib::ustring::compose(Dingo::Utilities::highlightString(_("Tracks in this Playlist:")) + " \n%1", temp_tracks_title_str);
    }
    
    else if ((!is_one_track_in_playlist) && (is_one_track_not_in_playlist)) {
      //no tracks are in the playlist
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox] = false;
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistent] = false;
      
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxOriginal] = false;
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistentOriginal] = false;
      
      final_tracks_title_str = Glib::ustring::compose(Dingo::Utilities::highlightString(_("Tracks in this Playlist:")) + " \n%1", _("None"));
    }
    
    else if ((is_one_track_in_playlist) && (!is_one_track_not_in_playlist)) {
      //all tracks are in the playlist
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox] = true;
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistent] = false;
      
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxOriginal] = true;
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistentOriginal] = false;
      
      final_tracks_title_str = Glib::ustring::compose(Dingo::Utilities::highlightString(_("Tracks in this Playlist:")) + " \n%1", temp_tracks_title_str);
    }
    
    else if ((!is_one_track_in_playlist) && (!is_one_track_not_in_playlist)) {
      //cannot happen
      //std::cout << "Error at Dingo::TrackPlaylistEditWIndow::setSelectedTracks()!" << std::endl;
    }
    
    temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackChanges] = final_tracks_title_str;
  }
  
  d_dbmanager->finalizeQueryTrackInPlaylist();
  
  if (!d_renderer_toggle_connection.connected()) {
    d_renderer_toggle_connection = p_renderer->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onTrackPlaylistEditCheckBoxToggled));
  }
  
  setCheckBoxColumnHeaderCheckButtonStatus();
}
    
//CREATORS
void Dingo::TrackPlaylistEditWindow::createIntroLabel() {
  top_vbox.pack_start(d_intro_label, Gtk::PACK_SHRINK);
  
  intro_separator_hbox.set_size_request(7, 7);
  top_vbox.pack_start(intro_separator_hbox, Gtk::PACK_SHRINK);
}

void Dingo::TrackPlaylistEditWindow::createTrackPlaylistTreeView() {
  filterPlaylists();
  
  d_trackplaylist_modify_filter = Gtk::TreeModelFilter::create(Dingo::DBManager::trackplaylistModel);
  d_trackplaylist_modify_filter->set_modify_func(*Dingo::DBManager::trackplaylistCR, sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onTrackPlaylistEditRowModify));
  
  d_trackplaylist_filter = Gtk::TreeModelFilter::create(d_trackplaylist_modify_filter);
  d_trackplaylist_filter->set_visible_column((*Dingo::DBManager::trackplaylistCR).trackplaylistVisible);

  d_trackplaylist_sort = Gtk::TreeModelSort::create(d_trackplaylist_filter);
  d_trackplaylist_sort->set_sort_func((*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistName, sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onPlaylistNameCompare));
  d_trackplaylist_sort->set_sort_func((*Dingo::DBManager::trackplaylistCR).trackplaylistTrackChanges, sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onTrackChangesCompare));
  
  d_trackplaylisttreeview.set_model(d_trackplaylist_sort);
  d_trackplaylisttreeview.set_rules_hint(true);
  
  d_trackplaylisttreeview.append_column_editable(_("Toggle"), (*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox);
  
  Gtk::CellRendererText *p_renderer_playlistname = Gtk::manage(new Gtk::CellRendererText());
  d_trackplaylisttreeview.append_column(_("Playlist"), *p_renderer_playlistname);
  
  Gtk::CellRendererText *p_renderer_changes = Gtk::manage(new Gtk::CellRendererText());
  d_trackplaylisttreeview.append_column(_("Changes"), *p_renderer_changes);
  
  d_trackplaylisttreeview.signal_query_tooltip().connect(sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onTrackPlaylistTreeViewQueryRowTooltip));
  d_trackplaylisttreeview.set_has_tooltip(true);
  d_trackplaylisttreeview.signal_row_activated().connect(sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onTrackPlaylistTreeViewRowDoubleClicked));
  
  p_renderer = (Gtk::CellRendererToggle*) d_trackplaylisttreeview.get_column_cell_renderer(0);
  d_renderer_toggle_connection = p_renderer->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onTrackPlaylistEditCheckBoxToggled));
  
  d_col_header_check.set_inconsistent(true);
  d_col_header_check.show();
  d_col_header_check_connection = d_col_header_check.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onCheckBoxColumnHeaderCheckButtonToggled));
  
  Gtk::TreeViewColumn* p_column = d_trackplaylisttreeview.get_column(0);
  if (p_column) {
    p_column->add_attribute(p_renderer->property_inconsistent(), (*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistent);
    d_col_header_click_connection = p_column->signal_clicked().connect(sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onCheckBoxColumnHeaderClicked));
    p_column->set_widget(d_col_header_check);
    p_column->set_expand(false);
    p_column->set_resizable(false);
    p_column->set_clickable(true);
  }
  
  p_column = d_trackplaylisttreeview.get_column(1);
  if (p_column) {
    p_column->add_attribute(p_renderer_playlistname->property_markup(), (*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistName);
    p_column->set_sort_column((*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistName);
    p_column->set_sort_indicator(true);
    p_column->set_expand(true);
    p_column->set_clickable(true);
    p_column->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
    p_column->set_resizable(true);
    p_column->set_fixed_width(180);
  }
  
  p_column = d_trackplaylisttreeview.get_column(2);
  if (p_column) {
    p_column->add_attribute(p_renderer_changes->property_markup(), (*Dingo::DBManager::trackplaylistCR).trackplaylistTrackChanges);
    p_renderer_changes->property_wrap_mode() = Pango::WRAP_WORD;
    p_column->set_sort_column((*Dingo::DBManager::trackplaylistCR).trackplaylistTrackChanges);
    p_column->set_sort_indicator(true);
    p_column->set_expand(true);
    p_column->set_clickable(true);
    p_column->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
    p_column->set_resizable(true);
    p_column->set_fixed_width(405);
  }
  
  d_trackplaylist_sort->set_sort_column(3, Gtk::SORT_ASCENDING);
  
  d_scrolledwin.add(d_trackplaylisttreeview);
  d_scrolledwin.set_shadow_type(Gtk::SHADOW_OUT);
  
  top_vbox.pack_start(d_scrolledwin, Gtk::PACK_EXPAND_WIDGET);
  
  separator_hbox.set_size_request(12, 12);
  top_vbox.pack_start(separator_hbox, Gtk::PACK_SHRINK);
  
  top_vbox.set_border_width(8);
}

void Dingo::TrackPlaylistEditWindow::createButtonsHBox() {
  d_search_entry.signal_changed().connect(sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onSearchEntryChanged));
  d_save_button.signal_clicked().connect(sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onSaveButtonClicked));
  d_cancel_button.signal_clicked().connect(sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onCancelButtonClicked));
  
  d_search_entry.set_size_request(250, 30);
  d_save_button.set_size_request(85, 30);
  d_cancel_button.set_size_request(85, 30);
  
  d_search_entry.set_icon_from_stock(Gtk::Stock::FIND, Gtk::ENTRY_ICON_PRIMARY);
  d_search_entry.set_placeholder_text(_("Search Playlists..."));
  
  d_search_entry.signal_icon_press().connect(sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onIconPressEvent));
  d_search_entry.signal_enter_notify_event().connect(sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onSearchEntryEnterNotifyEvent));
  d_search_entry.signal_leave_notify_event().connect(sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onSearchEntryLeaveNotifyEvent));
  
  buttons_hbox.pack_start(d_search_entry, Gtk::PACK_SHRINK);
  buttons_hbox.pack_end(d_save_button, Gtk::PACK_SHRINK);
  buttons_hbox.pack_end(d_cancel_button, Gtk::PACK_SHRINK, 6);
  
  top_vbox.pack_start(buttons_hbox, Gtk::PACK_SHRINK);
}

//UTILITIES
Glib::ustring Dingo::TrackPlaylistEditWindow::convertToTrackTitleString(std::map<int, Glib::ustring> remaining_trackid_tracktitle_map) {
  Glib::ustring temp_track_title_string = "";
  
  for (std::map<int, Glib::ustring>::iterator temp_iter = remaining_trackid_tracktitle_map.begin(); temp_iter != remaining_trackid_tracktitle_map.end(); ++temp_iter) {
    temp_track_title_string += temp_iter->second + " | ";
  }
  
  temp_track_title_string.erase(temp_track_title_string.size() - 1);
  
  return temp_track_title_string;
}

Glib::ustring Dingo::TrackPlaylistEditWindow::getTracksAddingDeletingString(Gtk::TreeModel::iterator child_tree_iter) {
  Gtk::TreeModel::Row temp_child_row = *child_tree_iter;
  bool checkbox_inconsistent = temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistent];
  bool checkbox_status = temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox];
  bool checkbox_inconsistent_original = temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistentOriginal];
  bool checkbox_status_original = temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxOriginal];
  Glib::ustring return_string;

  //if some tracks are in first & now all tracks are in
  //=> addition
  if ((checkbox_inconsistent_original) && (!checkbox_inconsistent) && (checkbox_status)) {
    Glib::ustring return_string_tracktitles = "";
  
    std::vector<int> temp_duplicate_trackid_vector = d_playlistid_trackid_map[temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistID]];
    
    std::map<int, Glib::ustring> temp_all_trackid_tracktitle_map = d_all_trackid_tracktitle_map;
    
    for (std::vector<int>::iterator iter = temp_duplicate_trackid_vector.begin(); iter != temp_duplicate_trackid_vector.end(); ++iter) {
      std::map<int, Glib::ustring>::iterator found_all_trackid_iter = temp_all_trackid_tracktitle_map.find(*iter);
    
      if (found_all_trackid_iter != temp_all_trackid_tracktitle_map.end()) {
        temp_all_trackid_tracktitle_map.erase(found_all_trackid_iter);
      }
    }
  
    for (std::map<int, Glib::ustring>::iterator map_iter = temp_all_trackid_tracktitle_map.begin(); map_iter != temp_all_trackid_tracktitle_map.end(); ++map_iter) {
      return_string_tracktitles += map_iter->second + " | ";
    }
    
    //store the changes in trackcount to temp_child_row
    temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges] = temp_all_trackid_tracktitle_map.size();
    return_string = Glib::ustring::compose(Dingo::Utilities::highlightString(Glib::ustring(_("Tracks to be added to this playlist:")) + " (%1)") + " \n%2", temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges], return_string_tracktitles);
  }
  
  //if some tracks are in first & now no tracks are in
  //=> deletion
  else if ((checkbox_inconsistent_original) && (!checkbox_inconsistent) && (!checkbox_status)) {
    Glib::ustring return_string_tracktitles = "";
    
    std::vector<int> temp_duplicate_trackid_vector = d_playlistid_trackid_map[temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistID]];
    
    for (std::vector<int>::iterator temp_iter = temp_duplicate_trackid_vector.begin(); temp_iter != temp_duplicate_trackid_vector.end(); ++temp_iter) {
      return_string_tracktitles += d_all_trackid_tracktitle_map[*temp_iter] + " | ";
    }
    
    //store the changes in trackcount to temp_child_row
    temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges] = temp_duplicate_trackid_vector.size() * -1;
    return_string = Glib::ustring::compose(Dingo::Utilities::highlightString(Glib::ustring(_("Tracks to be deleted from this playlist:")) + " (%1)") + " \n%2", temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges], return_string_tracktitles);
  }
  
  //if all tracks are in first & now no tracks are in
  //=> deletion
  else if ((!checkbox_inconsistent_original) && (checkbox_status_original) && (!checkbox_status)) {
    Glib::ustring return_string_tracktitles = "";
    
    for (std::map<int, Glib::ustring>::iterator map_iter = d_all_trackid_tracktitle_map.begin(); map_iter != d_all_trackid_tracktitle_map.end(); ++map_iter) {
      return_string_tracktitles += map_iter->second + " | ";
    }
    
    //store the changes in trackcount to temp_child_row
    temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges] = d_all_trackid_tracktitle_map.size() * -1;
    return_string = Glib::ustring::compose(Dingo::Utilities::highlightString(Glib::ustring(_("Tracks to be deleted from this playlist:")) + " (%1)") + " \n%2", temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges], return_string_tracktitles);
  }
  
  //if no tracks are in first & now all tracks are in
  //=> addition
  else if ((!checkbox_inconsistent_original) && (!checkbox_status_original) && (checkbox_status)) {
    Glib::ustring return_string_tracktitles = "";
    
    for (std::map<int, Glib::ustring>::iterator map_iter = d_all_trackid_tracktitle_map.begin(); map_iter != d_all_trackid_tracktitle_map.end(); ++map_iter) {
      return_string_tracktitles += map_iter->second + " | ";
    }
    
    //store the changes in trackcount to temp_child_row
    temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges] = d_all_trackid_tracktitle_map.size();
    return_string = Glib::ustring::compose(Dingo::Utilities::highlightString(Glib::ustring(_("Tracks to be added to this playlist:")) + " (%1)") + " \n%2",temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges], return_string_tracktitles);
  }
  
  //if all tracks are in first and now all tracks are still in
  //=> no changes
  else if ((!checkbox_inconsistent_original) && (checkbox_status_original) && (checkbox_status)) {
    Glib::ustring return_string_tracktitles = "";
    
    std::vector<int> temp_duplicate_trackid_vector = d_playlistid_trackid_map[temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistID]];
    
    for (std::vector<int>::iterator temp_iter = temp_duplicate_trackid_vector.begin(); temp_iter != temp_duplicate_trackid_vector.end(); ++temp_iter) {
      return_string_tracktitles += d_all_trackid_tracktitle_map[*temp_iter] + " | ";
      
      //store the changes in trackcount to temp_child_row
      temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges] = 0;
    }
    
    return_string = Glib::ustring::compose(Dingo::Utilities::highlightString(_("Tracks in this Playlist:")) + " \n%1", return_string_tracktitles);
  }
  
  else {
    return_string = Glib::ustring::compose(Dingo::Utilities::highlightString(_("Tracks in this Playlist:")) + " \n%1", _("None"));
    
    //store the changes in trackcount to temp_child_row
    temp_child_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges] = 0;
  }
  
  return return_string;
}

void Dingo::TrackPlaylistEditWindow::setCheckBoxColumnHeaderCheckButtonStatus() {
  bool is_one_box_active = false;
  bool is_one_box_not_active = false;
  bool is_one_box_inconsistent = false;
  
  //first disconnect column header & d_col_header_check from their signals
  if (d_col_header_check_connection.connected()) {
    d_col_header_check_connection.disconnect();
  }
  
  if (d_col_header_click_connection.connected()) {
    d_col_header_click_connection.disconnect();
  }
  
  Gtk::TreeModel::Children temp_children = Dingo::DBManager::trackplaylistModel->children();
  
  for (Gtk::TreeModel::Children::iterator temp_iter = temp_children.begin(); temp_iter != temp_children.end(); ++temp_iter) {
    Gtk::TreeModel::Row temp_row = *(temp_iter);
    
    if (temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistent]) {
      if (!is_one_box_inconsistent) {
        is_one_box_inconsistent = true;
      }
    }
    
    else {
      if (temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox]) {
        if (!is_one_box_active) {
          is_one_box_active = true;          
        }
      }
      
      else {
        if (!is_one_box_not_active) {
          is_one_box_not_active = true;
        }
      }
    }
  }
  
  //when there are no empty & inconsistent boxes
  //=> column header checkbox "active"
  //=> !is_one_box_not_active && !is_one_box_inconsistent
  if ((!is_one_box_not_active) && (!is_one_box_inconsistent)) {
    d_col_header_check.set_active(true);
    d_col_header_check.set_inconsistent(false);
  }
  
  //when there are no inconsistent & active boxes
  //=> column header checkbox "not active"
  //=> !is_one_box_active && !is_one_box_inconsistent
  else if ((!is_one_box_active) && (!is_one_box_inconsistent)) {
    d_col_header_check.set_active(false);
    d_col_header_check.set_inconsistent(false);
  }
  
  //when there are at least 1 inconsistent box
  //OR at least 1 active box and 1 empty box
  //=> column header checkbox "inconsistent" ("empty" underneath)
  //=> (is_one_box_active && is_one_box_not_active) || (is_one_box_inconsistent)
  else if (((is_one_box_active) && (is_one_box_not_active)) || (is_one_box_inconsistent)) {
    d_col_header_check.set_active(false);
    d_col_header_check.set_inconsistent(true);
  }
  
  //if none of the above conditions is true
  //then make the column header checkbox empty
  else {
    d_col_header_check.set_active(false);
    d_col_header_check.set_inconsistent(false);
  }
  
  //reconnect all signal handlers
  if (!d_col_header_check_connection.connected()) {
    d_col_header_check_connection = d_col_header_check.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onCheckBoxColumnHeaderCheckButtonToggled));
  }
  
  if (!d_col_header_click_connection.connected()) {
    Gtk::TreeViewColumn* p_column = d_trackplaylisttreeview.get_column(0);
  
    d_col_header_click_connection = p_column->signal_clicked().connect(sigc::mem_fun(*this, &Dingo::TrackPlaylistEditWindow::onCheckBoxColumnHeaderClicked));
  }
}
      
void Dingo::TrackPlaylistEditWindow::resetData() {
  d_all_tracks_title_str = "";
  
  d_all_trackid_tracktitle_map.clear();
  d_playlistid_trackid_map.clear();
  
  //go over all trackplaylistTrackCountChanges' rows to set changes to 0
  Gtk::TreeModel::Children temp_children = Dingo::DBManager::trackplaylistModel->children();
  
  for (Gtk::TreeModel::Children::iterator temp_iter = temp_children.begin(); temp_iter != temp_children.end(); ++temp_iter) {
    Gtk::TreeModel::Row temp_row = *(temp_iter);
    
    temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges] = 0;
  }
}
      
//SIGNAL HANDLERS
int Dingo::TrackPlaylistEditWindow::onPlaylistNameCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter) {
  Gtk::TreeModel::Row temp_first_row = *(d_trackplaylist_modify_filter->convert_iter_to_child_iter(d_trackplaylist_filter->convert_iter_to_child_iter(first_iter)));
  Gtk::TreeModel::Row temp_second_row = *(d_trackplaylist_modify_filter->convert_iter_to_child_iter(d_trackplaylist_filter->convert_iter_to_child_iter(second_iter)));
  
  Glib::ustring first_title_str = Dingo::Utilities::unhighlightString(temp_first_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistName]);
  
  Glib::ustring second_title_str = Dingo::Utilities::unhighlightString(temp_second_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistName]);
  
  if (first_title_str < second_title_str) {
    return -1;
  }
  
  else if (first_title_str > second_title_str) {
    return 1;
  }
  
  else {
    return 0;
  }
}

int Dingo::TrackPlaylistEditWindow::onTrackChangesCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter) {
  Gtk::TreeModel::Row temp_first_row = *first_iter;
  Gtk::TreeModel::Row temp_second_row = *second_iter;
  
  int first_playlist_count_changes = temp_first_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges];
  int second_playlist_count_changes = temp_second_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges];
  
  if (first_playlist_count_changes < second_playlist_count_changes) {
    return 1;
  }
  
  else if (first_playlist_count_changes > second_playlist_count_changes) {
    return -1;
  }
  
  else {
    return 0;
  }
}

void Dingo::TrackPlaylistEditWindow::onTrackPlaylistEditRowModify(const Gtk::TreeModel::iterator& iter, Glib::ValueBase& value, int column_id) {
  Gtk::TreeModel::Row temp_row = *(d_trackplaylist_modify_filter->convert_iter_to_child_iter(iter));
  
  switch (column_id) {
    case (0): {
      Glib::Value<int> valInt;
      valInt.init(Glib::Value<int>::value_type());
    
      int temp_cur_playlist_id = temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistID];
      
      valInt.set(temp_cur_playlist_id);
      value = valInt;
    
      break;
    }
    
    case (1): {
      Glib::Value<bool> valBool;
      valBool.init(Glib::Value<bool>::value_type());
      
      bool temp_cur_checkbox_status = temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox];
      
      //std::cout << "Checkbox Status: " << temp_cur_checkbox_status << std::endl;
      
      valBool.set(temp_cur_checkbox_status);
      value = valBool;
    
      break;
    }
    
    case (2): {
      Glib::Value<bool> valBool;
      valBool.init(Glib::Value<bool>::value_type());
      
      bool temp_cur_inconsistent_status = temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistent];
    
      valBool.set(temp_cur_inconsistent_status);
      value = valBool;
    
      break;
    }
    
    case (3): {
      Glib::Value<Glib::ustring> valStr;
      valStr.init(Glib::Value<Glib::ustring>::value_type());
    
      Glib::ustring temp_cur_playlist_name = temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistName] + " (" + Dingo::Utilities::intToString(temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountOriginal] + temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges]) + ")";
      
      //if some tracks are in first & now all tracks are in
      //=> addition
      bool checkbox_inconsistent = temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistent];
      bool checkbox_status = temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox];
      bool checkbox_inconsistent_original = temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistentOriginal];
      bool checkbox_status_original = temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxOriginal];
      
      if ((checkbox_inconsistent_original) && (!checkbox_inconsistent) && (checkbox_status)) {
        temp_cur_playlist_name = Dingo::Utilities::highlightString(temp_cur_playlist_name);
      }
      
      //if some tracks are in first & now no tracks are in
      //=> deletion
      else if ((checkbox_inconsistent_original) && (!checkbox_inconsistent) && (!checkbox_status)) {
        temp_cur_playlist_name = Dingo::Utilities::highlightString(temp_cur_playlist_name);
      }
      
      //if all tracks are in first & now no tracks are in
      //=> deletion
      else if ((!checkbox_inconsistent_original) && (checkbox_status_original) && (!checkbox_status)) {
        temp_cur_playlist_name = Dingo::Utilities::highlightString(temp_cur_playlist_name);
      }
      
      //if no tracks are in first & now all tracks are in
      //=> addition
      else if ((!checkbox_inconsistent_original) && (!checkbox_status_original) && (checkbox_status)) {
        temp_cur_playlist_name = Dingo::Utilities::highlightString(temp_cur_playlist_name);
      }
      
      //if nothings has changed
      //=> make the row normal
      else {
        temp_cur_playlist_name = Dingo::Utilities::unhighlightString(temp_cur_playlist_name);
      }
      
      valStr.set(temp_cur_playlist_name);
      value = valStr;
    
      break;
    }
    
    case (4): {
      Glib::Value<Glib::ustring> valStr;
      valStr.init(Glib::Value<Glib::ustring>::value_type());
    
      Glib::ustring temp_cur_track_title = temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackChanges];
    
      valStr.set(temp_cur_track_title);
      value = valStr;
    
      break;
    }
    
    case (8): {
      Glib::Value<int> valInt;
      valInt.init(Glib::Value<int>::value_type());
      
      int temp_cur_track_count_changes = temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges];
      
      valInt.set(temp_cur_track_count_changes);
      value = valInt;
      
      break;
    }
    
    case (9): {
      Glib::Value<bool> valBool;
      valBool.init(Glib::Value<bool>::value_type());
      
      bool temp_cur_visible = temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistVisible];
    
      valBool.set(temp_cur_visible);
      value = valBool;
    
      break;
    
      break;
    }
    
    default: {
      break;
    }
  }
}

void Dingo::TrackPlaylistEditWindow::onTrackPlaylistEditCheckBoxToggled(Glib::ustring tree_path) {
  Gtk::TreeModel::Row temp_row = *(d_trackplaylist_modify_filter->convert_iter_to_child_iter(d_trackplaylist_filter->convert_iter_to_child_iter(d_trackplaylist_sort->convert_iter_to_child_iter(d_trackplaylisttreeview.get_model()->get_iter(tree_path)))));
  
  if (temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistent]) {
    temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistent] = false;
    
    temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackChanges] = getTracksAddingDeletingString(d_trackplaylist_modify_filter->convert_iter_to_child_iter(d_trackplaylist_filter->convert_iter_to_child_iter(d_trackplaylist_sort->convert_iter_to_child_iter(d_trackplaylisttreeview.get_model()->get_iter(tree_path)))));
  }
  
  else {
    if (temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox]) {
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackChanges] = getTracksAddingDeletingString(d_trackplaylist_modify_filter->convert_iter_to_child_iter(d_trackplaylist_filter->convert_iter_to_child_iter(d_trackplaylist_sort->convert_iter_to_child_iter(d_trackplaylisttreeview.get_model()->get_iter(tree_path)))));
    }
    
    else {
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackChanges] = getTracksAddingDeletingString(d_trackplaylist_modify_filter->convert_iter_to_child_iter(d_trackplaylist_filter->convert_iter_to_child_iter(d_trackplaylist_sort->convert_iter_to_child_iter(d_trackplaylisttreeview.get_model()->get_iter(tree_path)))));
    }
  }
  
  d_trackplaylist_modify_filter->refilter();
  setCheckBoxColumnHeaderCheckButtonStatus();
}

void Dingo::TrackPlaylistEditWindow::onTrackPlaylistTreeViewRowDoubleClicked(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column) {
  Gtk::TreeModel::Row temp_row = *(d_trackplaylist_modify_filter->convert_iter_to_child_iter(d_trackplaylist_filter->convert_iter_to_child_iter(d_trackplaylist_sort->convert_iter_to_child_iter(d_trackplaylisttreeview.get_model()->get_iter(path)))));
  
  bool cur_checkbox_value = temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox];
  
  temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox] = !cur_checkbox_value;
  
  onTrackPlaylistEditCheckBoxToggled(path.to_string());
}

void Dingo::TrackPlaylistEditWindow::onCheckBoxColumnHeaderClicked() {
  if (d_col_header_check.get_active()) {
    d_col_header_check.set_active(false);
  }
  
  else {
    d_col_header_check.set_active(true);
  }
}

void Dingo::TrackPlaylistEditWindow::onCheckBoxColumnHeaderCheckButtonToggled() {
  Gtk::TreeModel::Children temp_children = Dingo::DBManager::trackplaylistModel->children();
  
  if (d_col_header_check.get_inconsistent()) {
    for (Gtk::TreeModel::Children::iterator temp_iter = temp_children.begin(); temp_iter != temp_children.end(); ++temp_iter) {
      Gtk::TreeModel::Row temp_row = *(temp_iter);
      
      d_col_header_check.set_inconsistent(false);
      
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBoxInconsistent] = false;
      
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox] = true;
      
      temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackChanges] = getTracksAddingDeletingString(temp_iter);
    }
  }
  
  else {
    if (d_col_header_check.get_active()) {
      for (Gtk::TreeModel::Children::iterator temp_iter = temp_children.begin(); temp_iter != temp_children.end(); ++temp_iter) {
        Gtk::TreeModel::Row temp_row = *(temp_iter);
        
        temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox] = true;
        
        temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackChanges] = getTracksAddingDeletingString(temp_iter);
      }
    }
    
    else {
      for (Gtk::TreeModel::Children::iterator temp_iter = temp_children.begin(); temp_iter != temp_children.end(); ++temp_iter) {
        Gtk::TreeModel::Row temp_row = *(temp_iter);
        
        temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistCheckBox] = false;
        
        temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackChanges] = getTracksAddingDeletingString(temp_iter);
      }
    }
  }
}

bool Dingo::TrackPlaylistEditWindow::onTrackPlaylistTreeViewQueryRowTooltip(int x, int y, bool is_tooltip_keyboard, const Glib::RefPtr<Gtk::Tooltip>& tooltip) {
  Gtk::TreeModel::Path temp_path;
  Gtk::TreeViewColumn temp_column;
  int bx, by;
  
  d_trackplaylisttreeview.convert_widget_to_bin_window_coords(x, y, bx, by);
  
  if (!d_trackplaylisttreeview.get_path_at_pos(bx, by, temp_path)) {
    return false;
  }
  
  Gtk::TreeModel::iterator temp_iter = d_trackplaylisttreeview.get_model()->get_iter(temp_path);
  
  if (!temp_iter) {
    return false;
  }
  
  Gtk::TreeModel::Row temp_row = *temp_iter;
  
  Glib::ustring tooltip_str = temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackChanges];
  
  if (tooltip_str.empty()) {
    return false;
  }
  
  tooltip->set_markup(tooltip_str);
  
  d_trackplaylisttreeview.set_tooltip_row(tooltip, temp_path);
  
  return true;
}

void Dingo::TrackPlaylistEditWindow::onCancelButtonClicked() {
  resetData();
  
  hide();
}

void Dingo::TrackPlaylistEditWindow::onSaveButtonClicked() {
  Gtk::TreeModel::Children temp_children = Dingo::DBManager::trackplaylistModel->children();
  
  for (Gtk::TreeModel::Children::iterator temp_iter = temp_children.begin(); temp_iter != temp_children.end(); ++temp_iter) {
    Gtk::TreeModel::Row temp_row = *(temp_iter);

    //if trackcountchanges > 0 
    //=> addition
    if (temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges] > 0) {
      std::vector<int> temp_original_trackid_vector = d_playlistid_trackid_map[temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistID]];
      
      //goes through d_all_trackid_tracktitle_map
      //  if you cannot find map_iter->first inside
      //  temp_original_trackid_vector
      //    add map_iter->first into the currently
      //    looped playlist
      
      std::vector<int> gonna_be_added_trackids_vector;
      
      for (std::map<int, Glib::ustring>::iterator map_iter = d_all_trackid_tracktitle_map.begin(); map_iter != d_all_trackid_tracktitle_map.end(); ++map_iter) {
        std::vector<int>::iterator found_original_trackid = std::find(temp_original_trackid_vector.begin(), temp_original_trackid_vector.end(), map_iter->first);
        
        if (found_original_trackid == temp_original_trackid_vector.end()) {
          gonna_be_added_trackids_vector.push_back(map_iter->first);
        }
      }
      
      d_dbmanager->addTracksToNormalPlaylist(gonna_be_added_trackids_vector, temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistID]);
    }
    
    //if trackcountchanges < 0
    //=> deletion
    else if (temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountChanges] < 0) {
      std::vector<int> temp_original_trackid_vector = d_playlistid_trackid_map[temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistID]];
      
      std::vector<int> gonna_be_deleted_trackids_vector;
      
      for (std::vector<int>::iterator vector_iter = temp_original_trackid_vector.begin(); vector_iter != temp_original_trackid_vector.end(); ++vector_iter) {
        gonna_be_deleted_trackids_vector.push_back(*vector_iter);
      }
      
      d_dbmanager->deleteTracksFromNormalPlaylist(gonna_be_deleted_trackids_vector, temp_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistID]);
    }
  }
  
  notifyTrackPlaylistEditWindowObserver(Dingo::TRACKPLAYLISTEDIT_INFO_SAVED);
  
  resetData();
  
  hide();
}

void Dingo::TrackPlaylistEditWindow::onSearchEntryChanged() {
  if (!d_search_entry.get_text().empty()) {
    d_search_entry.set_icon_from_stock(Gtk::Stock::CLEAR, Gtk::ENTRY_ICON_SECONDARY);
  }
  
  else {
    d_search_entry.unset_icon(Gtk::ENTRY_ICON_SECONDARY);
  }
  
  filterPlaylists();
  
  int temp_sort_column_id = 3;
  Gtk::SortType temp_sort_type = Gtk::SORT_ASCENDING;
  
  d_trackplaylist_sort->get_sort_column_id(temp_sort_column_id, temp_sort_type);
    
  d_trackplaylist_sort->set_sort_column(temp_sort_column_id, temp_sort_type);
  
  d_trackplaylist_modify_filter->refilter();
}

void Dingo::TrackPlaylistEditWindow::filterPlaylists() {
  Gtk::TreeModel::Children trackplaylist_children = Dingo::DBManager::trackplaylistModel->children();
  
  d_dbmanager->prepareQueryCountTracksInPlaylist();
  
  for (Gtk::TreeModel::Children::iterator trackplaylist_iter = trackplaylist_children.begin(); trackplaylist_iter != trackplaylist_children.end(); ++trackplaylist_iter) {
    Gtk::TreeModel::Row trackplaylist_row = *trackplaylist_iter;
    
    trackplaylist_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackCountOriginal] = d_dbmanager->getCountTracksInPlaylist(trackplaylist_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistID]);
    
    Glib::ustring playlist_name = Dingo::Utilities::toLowerCase(Dingo::Utilities::unhighlightString(trackplaylist_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistPlaylistName]));
  
    Glib::ustring playlist_changes = Dingo::Utilities::toLowerCase(Dingo::Utilities::removeFirstHighlightedSubStr(trackplaylist_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistTrackChanges]));
  
    Glib::ustring::size_type playlist_name_entry_text_pos = playlist_name.find(Dingo::Utilities::toLowerCase(d_search_entry.get_text()));
  
    if (playlist_name_entry_text_pos != Glib::ustring::npos) {  
      trackplaylist_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistVisible] = true;
    
      continue;
    }
  
    Glib::ustring::size_type playlist_changes_entry_text_pos = playlist_changes.find(Dingo::Utilities::toLowerCase(d_search_entry.get_text()));
    
    if (playlist_changes_entry_text_pos != Glib::ustring::npos) {
      trackplaylist_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistVisible] = true;
    
      continue;
    }
  
    trackplaylist_row[(*Dingo::DBManager::trackplaylistCR).trackplaylistVisible] = false;
  }
  
  d_dbmanager->finalizeQueryCountTracksInPlaylist();
}
      
void Dingo::TrackPlaylistEditWindow::onIconPressEvent(Gtk::EntryIconPosition icon_position, const GdkEventButton* event) {
  if ((icon_position == Gtk::ENTRY_ICON_SECONDARY) && (event->button == 1) && (event->type == GDK_BUTTON_PRESS)) {
    d_search_entry.set_text("");
  }
}

bool Dingo::TrackPlaylistEditWindow::onSearchEntryEnterNotifyEvent(GdkEventCrossing* event) {
  d_search_entry.get_window()->set_cursor(Gdk::Cursor::create(Gdk::HAND1));
}

bool Dingo::TrackPlaylistEditWindow::onSearchEntryLeaveNotifyEvent(GdkEventCrossing* event) {
  d_search_entry.get_window()->set_cursor(Gdk::Cursor::create(Gdk::LEFT_PTR));
}
