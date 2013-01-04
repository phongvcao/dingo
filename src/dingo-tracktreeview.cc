/* 
 * dingo-tracktreeview.cc
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

#include "dingo-tracktreeview.h"
#include "dingo-statusbar.h"
#include "dingo-tracktreeviewobserver.h"
#include "dingo-dbmanager.h"
#include "dingo-playerengine.h"
#include "dingo-settings.h"
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

#ifndef INCLUDED_TIME_H
#define INCLUDED_TIME_H
#include <time.h>
#endif //INCLUDED_TIME_H

//CONSTRUCTORS - DESTRUCTOR
Dingo::TrackTreeView::TrackTreeView() {

}

Dingo::TrackTreeView::TrackTreeView(Dingo::DBManager* dbmanager, Dingo::PlayerEngine* playerengine, Dingo::Settings* settings, Dingo::StatusBar* statusbar) :
  Gtk::TreeView(),
  d_trackinfowindow(dbmanager, settings),
  d_multipleinfowindow(dbmanager, settings),
  d_trackplaylisteditwindow(dbmanager),
  add_to(_("Add to...")),
  delete_from(_("Delete from...")),
  cut_tracks(_("Cut Track(s)")),
  copy_tracks(_("Copy Track(s)")),
  paste_tracks(_("Paste Track(s)")),
  edit_info(_("Edit Single Track")),
  edit_all_info(_("Edit All Selected Tracks")),
  lyrics(_("Lyrics")),
  add_play_queue(_("Play Queue")),
  add_playlists(_("Playlist(s)...")),
  delete_database(_("Database")),
  delete_play_queue(_("Play Queue")),
  delete_hard_drive(_("Hard Drive")),
  delete_current_playlist(_("Current Playlist")),
  delete_playlists(_("Playlist(s)...")),
  d_pausing_image(DATADIR"/dingo/icons/16x16/dingo-pausing.png"), 
  d_playing_image(DATADIR"/dingo/icons/16x16/dingo-playing.png"),
  d_status_header_image(DATADIR"/dingo/icons/16x16/dingo-trackstatus.png")
{
  d_playerengine = playerengine;
  d_dbmanager = dbmanager;
  d_settings = settings;
  d_statusbar = statusbar;
  
  d_playing_track_id = 0;
  
  createTrackTreeView();
  createPopupMenu();
  createAddSubMenu();
  createDeleteSubMenu();
  
  d_trackinfowindow.registerTrackInfoWindowObserver(this);
}

Dingo::TrackTreeView::~TrackTreeView() {

}

//OBSERVER'S METHODS
void Dingo::TrackTreeView::registerTrackTreeViewObserver(Dingo::TrackTreeViewObserver* observer) {
  track_obs_vector.push_back(observer);
}

void Dingo::TrackTreeView::removeTrackTreeViewObserver(Dingo::TrackTreeViewObserver* observer) {
  std::vector<Dingo::TrackTreeViewObserver*>::iterator iter = std::find(track_obs_vector.begin(), track_obs_vector.end(), observer);
  
  if (iter != track_obs_vector.end()) {
    track_obs_vector.erase(iter);
  }
}

void Dingo::TrackTreeView::notifyTrackTreeViewObserver(Dingo::TreeViewSignals signal_type) {
  switch (signal_type) {
    case Dingo::TRACK_TREE_ROW_ACTIVATED: {
      for (std::vector<Dingo::TrackTreeViewObserver*>::iterator iter = track_obs_vector.begin(); iter != track_obs_vector.end(); ++iter) {
        (*iter)->updateTrackTreeRowActivated(cur_track_row_ref);
      }
      //std::cout << "Track Double Clicked!" << std::endl;
      break;
    }
    
    case Dingo::TRACK_TREE_ROW_DEACTIVATED: {
      for (std::vector<Dingo::TrackTreeViewObserver*>::iterator iter = track_obs_vector.begin(); iter != track_obs_vector.end(); ++iter) {
        (*iter)->updateTrackTreeRowDeactivated();
      }
      //std::cout << "Track Tree Row Deactivated!" << std::endl;
      break;
    }
    
    default: {
      std::cerr << "Undefined Dingo::TreeViewSignals!" << std::endl;
      break;
    }
  }
}

void Dingo::TrackTreeView::delegateTrackPlaylistEditWindowObserver(Dingo::TrackPlaylistEditWindowObserver* observer) {
  d_trackplaylisteditwindow.registerTrackPlaylistEditWindowObserver(observer);
}

//PUBLIC ACCESSORS
void Dingo::TrackTreeView::setColumnVisible(int treeview_column_id, bool is_visible) {
  get_column(treeview_column_id)->set_visible(is_visible);
}

bool Dingo::TrackTreeView::getIsAnyTrackSelected() {
  return !get_selection()->get_selected_rows().empty();
}

//INHERITED OBSERVER'S METHODS
void Dingo::TrackTreeView::updateHomeToggleActive() {
  refreshData();
}

void Dingo::TrackTreeView::updateNowPlayingToggleActive() {
  refreshData();
}

void Dingo::TrackTreeView::updatePlayQueueToggleActive() {
  refreshData();
}

void Dingo::TrackTreeView::updateSearchEntryChanged() {
  refreshData();
}

void Dingo::TrackTreeView::updateAudioFilterToggled() {
  refreshData();
}

void Dingo::TrackTreeView::updateVideoFilterToggled() {
  refreshData();
}

void Dingo::TrackTreeView::updateBlockSelection() {
  d_selected_store_map[d_dbmanager->getPreviousPlaylistID()] = getSelectionRefs();
}

void Dingo::TrackTreeView::updateRestoreSelectedRows() {
  Gtk::TreeModel::Children children = Dingo::DBManager::trackModel->children();
  
  for (Gtk::TreeModel::Children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    
    if (row[(*Dingo::DBManager::trackCR).trackID]) {
      row[(*Dingo::DBManager::trackCR).trackVisible] = true;
    }
  }

  get_selection()->unselect_all();
  
  std::vector<Gtk::TreeRowReference> refs_vector = d_selected_store_map[d_dbmanager->getCurrentPlaylistID()];
  
  for (std::vector<Gtk::TreeRowReference>::iterator ref_iter = refs_vector.begin(); ref_iter != refs_vector.end(); ++ref_iter) {
    get_selection()->select(d_track_sort->convert_child_path_to_path(d_track_filter->convert_child_path_to_path(ref_iter->get_path())));
  }
  
  refreshData();
}

void Dingo::TrackTreeView::updateUnblockSelection() {

}

void Dingo::TrackTreeView::updateGenresSelected() {
  refreshData();
}

void Dingo::TrackTreeView::updateArtistsSelected() {
  refreshData();
}

void Dingo::TrackTreeView::updateYearsSelected() {
  refreshData();
}

void Dingo::TrackTreeView::updateAlbumsSelected() {
  refreshData();
}

void Dingo::TrackTreeView::updateTrackPlaylistEditInfoSaved() {
  refreshData();
}

void Dingo::TrackTreeView::updateChangesInCoreTracks() {
  refreshData();
}

void Dingo::TrackTreeView::updateRepeatOff() {
  Gtk::TreeModel::iterator temp_iter = Dingo::DBManager::trackModel->get_iter(cur_track_row_ref.get_path());
  
  Gtk::TreeModel::iterator temp_top_iter = get_model()->get_iter(d_track_sort->convert_child_path_to_path(d_track_filter->convert_child_path_to_path(cur_track_row_ref.get_path())));
  
  Gtk::TreeModel::Row temp_row;
  Gtk::TreeModel::Row temp_top_row;
  
  Gtk::TreeModel::Path temp_top_path = get_model()->get_path(temp_top_iter);
  
  if (temp_iter) {
    unhighlightRow(temp_iter);
  }
  
  Gtk::TreeModel::Children children = get_model()->children();
  
  switch (d_playerengine->getPlayOrder()) {
    case Dingo::PLAYORDER_NORMAL: {
      temp_top_path.next();
      break;
    }
    
    case Dingo::PLAYORDER_SHUFFLE: {
      int top_path_int = -1;
    
      if (temp_top_path.to_string() != "") {
        top_path_int = Dingo::Utilities::stringToInt(temp_top_path.to_string());
      }
    
      int lower_bound = top_path_int + 1;
      int upper_bound = children.size() - 1;
      Glib::ustring new_top_path_str = "0";
      
      if (lower_bound == upper_bound - 1) {
        new_top_path_str = Dingo::Utilities::intToString(upper_bound);
      }
      
      else if (lower_bound == upper_bound) {
        new_top_path_str = "0";
      }
      
      else {
        new_top_path_str = Dingo::Utilities::intToString(Dingo::Utilities::randomInRange(lower_bound, upper_bound));
      }
    
      Gtk::TreeModel::Path new_temp_top_path(new_top_path_str);
    
      temp_top_path = new_temp_top_path;
      
      break;
    }
    
    default: {
      //std::cout << "Unknown Dingo::PlayOrder!" << std::endl;
      temp_top_path.next();
      break;
    }
  }
  
  temp_top_iter = get_model()->get_iter(temp_top_path);
  
  if (temp_top_iter) {
    temp_top_row = *temp_top_iter;
  
    temp_iter = Dingo::DBManager::trackModel->get_iter(d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(temp_top_path)));
  
    //save the newly played row to cur_track_row_ref
    Gtk::TreeRowReference temp_ref(Dingo::DBManager::trackModel, d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(temp_top_path)));
  
    cur_track_row_ref = temp_ref;
  
    d_dbmanager->setCurrentTrackRowRef(cur_track_row_ref);
  
    //play the file
    temp_row = *temp_iter;
  
    if ((d_playerengine->getStatus() == Dingo::ENGINE_PLAYING) || (d_playerengine->getStatus() == Dingo::ENGINE_PAUSED)) {
      d_playerengine->setStatus(Dingo::ENGINE_STOPPED);
    }
  
    d_playerengine->read(temp_row[(*Dingo::DBManager::trackCR).trackURI], temp_row[(*Dingo::DBManager::trackCR).trackMIMEType], temp_row[(*Dingo::DBManager::trackCR).trackID], temp_row[(*Dingo::DBManager::trackCR).trackSubtitleURI]);
  
    d_playing_track_id = temp_row[(*Dingo::DBManager::trackCR).trackID];
  
    //notify TrackTreeViewObserver
    notifyTrackTreeViewObserver(Dingo::TRACK_TREE_ROW_ACTIVATED);
  
    highlightRow(temp_iter);
  
    d_playerengine->play();
  
    d_playerengine->notifyEngineStatusObserver(Dingo::ENGINE_NEW_PLAYING);
  
    Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Metadata");
  }
  
  else {
    d_playerengine->stop();
  
    d_playing_track_id = 0;
  
    notifyTrackTreeViewObserver(Dingo::TRACK_TREE_ROW_DEACTIVATED);
  
    cur_track_row_ref = Gtk::TreeRowReference();
  
    d_dbmanager->setCurrentTrackRowRef(cur_track_row_ref);
  
    Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Metadata");
  }

  /* Gtk::TreeModel::iterator iter = Dingo::DBManager::trackModel->get_iter(cur_track_row_ref.get_path());
  
  unhighlightRow(iter);
  
  d_playerengine->stop();
  
  d_playerengine->setStatus(Dingo::ENGINE_STOPPED);
  
  d_playing_track_id = 0;
  
  notifyTrackTreeViewObserver(Dingo::TRACK_TREE_ROW_DEACTIVATED);
  
  cur_track_row_ref = Gtk::TreeRowReference();
  
  d_dbmanager->setCurrentTrackRowRef(cur_track_row_ref);
  
  Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Metadata"); */
}

void Dingo::TrackTreeView::updateRepeatSingle() {
  d_playerengine->replay();
}

void Dingo::TrackTreeView::updateRepeatPlaylist() {
  updateEngineTrackNext();
}

void Dingo::TrackTreeView::updateRepeatSection() {

}

void Dingo::TrackTreeView::updateRepeatSectionOff() {

}

void Dingo::TrackTreeView::updateEngineTrackNext() {
  Gtk::TreeModel::iterator temp_iter = Dingo::DBManager::trackModel->get_iter(cur_track_row_ref.get_path());
  
  Gtk::TreeModel::iterator temp_top_iter = get_model()->get_iter(d_track_sort->convert_child_path_to_path(d_track_filter->convert_child_path_to_path(cur_track_row_ref.get_path())));
  
  Gtk::TreeModel::Row temp_row;
  Gtk::TreeModel::Row temp_top_row;
  
  Gtk::TreeModel::Path temp_top_path = get_model()->get_path(temp_top_iter);
  
  if (temp_iter) {
    unhighlightRow(temp_iter);
  }
  
  Gtk::TreeModel::Children children = get_model()->children();
  
  switch (d_playerengine->getPlayOrder()) {
    case Dingo::PLAYORDER_NORMAL: {
      temp_top_path.next();
      break;
    }
    
    case Dingo::PLAYORDER_SHUFFLE: {
      int top_path_int = -1;
    
      if (temp_top_path.to_string() != "") {
        top_path_int = Dingo::Utilities::stringToInt(temp_top_path.to_string());
      }
    
      int lower_bound = top_path_int + 1;
      int upper_bound = children.size() - 1;
      Glib::ustring new_top_path_str = "0";
      
      if (lower_bound == upper_bound - 1) {
        new_top_path_str = Dingo::Utilities::intToString(upper_bound);
      }
      
      else if (lower_bound == upper_bound) {
        new_top_path_str = "0";
      }
      
      else {
        new_top_path_str = Dingo::Utilities::intToString(Dingo::Utilities::randomInRange(lower_bound, upper_bound));
      }
    
      Gtk::TreeModel::Path new_temp_top_path(new_top_path_str);
    
      temp_top_path = new_temp_top_path;
      
      break;
    }
    
    default: {
      //std::cout << "Unknown Dingo::PlayOrder!" << std::endl;
      temp_top_path.next();
      break;
    }
  }
  
  temp_top_iter = get_model()->get_iter(temp_top_path);
  
  if (temp_top_iter) {
    temp_top_row = *temp_top_iter;
  }
  
  else {
    temp_top_iter = children.begin();
    
    temp_top_row = *temp_top_iter;
    
    temp_top_path = get_model()->get_path(temp_top_iter);
  }
  
  temp_iter = Dingo::DBManager::trackModel->get_iter(d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(temp_top_path)));
  
  //save the newly played row to cur_track_row_ref
  Gtk::TreeRowReference temp_ref(Dingo::DBManager::trackModel, d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(temp_top_path)));
  
  cur_track_row_ref = temp_ref;
  
  d_dbmanager->setCurrentTrackRowRef(cur_track_row_ref);
  
  //play the file
  temp_row = *temp_iter;
  
  if ((d_playerengine->getStatus() == Dingo::ENGINE_PLAYING) || (d_playerengine->getStatus() == Dingo::ENGINE_PAUSED)) {
    d_playerengine->setStatus(Dingo::ENGINE_STOPPED);
  }
  
  d_playerengine->read(temp_row[(*Dingo::DBManager::trackCR).trackURI], temp_row[(*Dingo::DBManager::trackCR).trackMIMEType], temp_row[(*Dingo::DBManager::trackCR).trackID], temp_row[(*Dingo::DBManager::trackCR).trackSubtitleURI]);
  
  d_playing_track_id = temp_row[(*Dingo::DBManager::trackCR).trackID];
  
  //notify TrackTreeViewObserver
  notifyTrackTreeViewObserver(Dingo::TRACK_TREE_ROW_ACTIVATED);
  
  highlightRow(temp_iter);
  
  d_playerengine->play();
  
  d_playerengine->notifyEngineStatusObserver(Dingo::ENGINE_NEW_PLAYING);
  
  Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Metadata");
}

void Dingo::TrackTreeView::updateEngineTrackPrevious() {
  Gtk::TreeModel::iterator temp_iter = Dingo::DBManager::trackModel->get_iter(cur_track_row_ref.get_path());
  
  Gtk::TreeModel::iterator temp_top_iter = get_model()->get_iter(d_track_sort->convert_child_path_to_path(d_track_filter->convert_child_path_to_path(cur_track_row_ref.get_path())));
  
  Gtk::TreeModel::Row temp_row;
  Gtk::TreeModel::Row temp_top_row;
  
  Gtk::TreeModel::Path temp_top_path = get_model()->get_path(temp_top_iter);
  
  if (temp_iter) {
    unhighlightRow(temp_iter);
  }
  
  Gtk::TreeModel::Children children = get_model()->children();
  
  switch (d_playerengine->getPlayOrder()) {
    case Dingo::PLAYORDER_NORMAL: {
      temp_top_path.prev();
      break;
    }
    
    case Dingo::PLAYORDER_SHUFFLE: {
      int top_path_int = children.size();
    
      if (temp_top_path.to_string() != "") {
        top_path_int = Dingo::Utilities::stringToInt(temp_top_path.to_string());
      }
    
      int lower_bound = 0;
      int upper_bound = top_path_int - 1;
      Glib::ustring new_top_path_str = Dingo::Utilities::intToString(children.size() - 1);
      
      if (lower_bound == upper_bound - 1) {
        new_top_path_str = Dingo::Utilities::intToString(upper_bound);
      }
      
      else if (lower_bound == upper_bound) {
        new_top_path_str = Dingo::Utilities::intToString(children.size() - 1);//"0";
      }
      
      else {
        new_top_path_str = Dingo::Utilities::intToString(Dingo::Utilities::randomInRange(lower_bound, upper_bound));
      }
    
      Gtk::TreeModel::Path new_temp_top_path(new_top_path_str);
    
      temp_top_path = new_temp_top_path;
      
      break;
    }
    
    default: {
      //std::cout << "Unknown Dingo::PlayOrder!" << std::endl;
      temp_top_path.prev();
      break;
    }
  }
  
  temp_top_iter = get_model()->get_iter(temp_top_path);
  
  if (temp_top_iter) {
    temp_top_row = *temp_top_iter;
  }
  
  else {
    temp_top_iter = children.end();
    
    temp_top_row = *temp_top_iter;
    
    temp_top_path = get_model()->get_path(temp_top_iter);
  }
  
  temp_iter = Dingo::DBManager::trackModel->get_iter(d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(temp_top_path)));
  
  //save the newly played row to cur_track_row_ref
  Gtk::TreeRowReference temp_ref(Dingo::DBManager::trackModel, d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(temp_top_path)));
  
  cur_track_row_ref = temp_ref;
  
  d_dbmanager->setCurrentTrackRowRef(cur_track_row_ref);
  
  //play the file
  temp_row = *temp_iter;
  
  if ((d_playerengine->getStatus() == Dingo::ENGINE_PLAYING) || (d_playerengine->getStatus() == Dingo::ENGINE_PAUSED)) {
    d_playerengine->setStatus(Dingo::ENGINE_STOPPED);
  }
  
  d_playerengine->read(temp_row[(*Dingo::DBManager::trackCR).trackURI], temp_row[(*Dingo::DBManager::trackCR).trackMIMEType], temp_row[(*Dingo::DBManager::trackCR).trackID], temp_row[(*Dingo::DBManager::trackCR).trackSubtitleURI]);
  
  d_playing_track_id = temp_row[(*Dingo::DBManager::trackCR).trackID];
  
  //notify TrackTreeViewObserver
  notifyTrackTreeViewObserver(Dingo::TRACK_TREE_ROW_ACTIVATED);
  
  highlightRow(temp_iter);
  
  d_playerengine->play();
  
  d_playerengine->notifyEngineStatusObserver(Dingo::ENGINE_NEW_PLAYING);
  
  Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Metadata");
}

void Dingo::TrackTreeView::updateEngineTrackCurrent() {
  switch (d_playerengine->getStatus()) {
    case Dingo::ENGINE_PLAYING: {
      d_playerengine->pause();
      
      if (cur_track_row_ref.is_valid()) {
        Gtk::TreeModel::Row temp_cur_row = *(Dingo::DBManager::trackModel->get_iter(cur_track_row_ref.get_path()));
      
        temp_cur_row[(*Dingo::DBManager::trackCR).trackStatus] = d_pausing_image.get_pixbuf();
      }
      
      break;
    }
    
    case Dingo::ENGINE_PAUSED: {
      d_playerengine->play();
      
      if (cur_track_row_ref.is_valid()) {
        Gtk::TreeModel::Row temp_cur_row = *(Dingo::DBManager::trackModel->get_iter(cur_track_row_ref.get_path()));
      
        temp_cur_row[(*Dingo::DBManager::trackCR).trackStatus] = d_playing_image.get_pixbuf();
      }
      
      break;
    }
    
    case Dingo::ENGINE_STOPPED: {
      std::vector<Gtk::TreeModel::Path> temp_selected_path_vector = get_selection()->get_selected_rows();
      
      Gtk::TreeModel::Path temp_cur_selected_path;
      
      if (temp_selected_path_vector.size() == 0) {
        Gtk::TreeModel::Children children = get_model()->children();
        
        if (children.begin()) {
          temp_cur_selected_path = get_model()->get_path(children.begin());
        }
      }
      
      else {
        temp_cur_selected_path = temp_selected_path_vector[0];
      }
      
      if (!temp_cur_selected_path.empty()) {
        //get the current track's Gtk::TreePath & convert it to the underlying model's Gtk::TreePath      
        get_selection()->select(temp_cur_selected_path);
    
        //create a Gtk::TreeRowReference for the current underlying model's Gtk::TreePath & save the Gtk::TreeRowReference
        //to cur_track_row_ref
        Gtk::TreeRowReference temp_track_row_ref(Dingo::DBManager::trackModel, d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(temp_cur_selected_path)));
      
        cur_track_row_ref = temp_track_row_ref;
  
        d_dbmanager->setCurrentTrackRowRef(cur_track_row_ref);
      
        //notify TrackTreeViewObserver
        notifyTrackTreeViewObserver(Dingo::TRACK_TREE_ROW_ACTIVATED);
      
        //select the current temp_path
        //have to convert to the underlying model's path because the get_selection() belongs to the original model
        Gtk::TreeModel::iterator temp_cur_track_iter = Dingo::DBManager::trackModel->get_iter(d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(temp_cur_selected_path)));
      
        Gtk::TreeModel::Row temp_row = *temp_cur_track_iter;
        
        d_playerengine->read(temp_row[(*Dingo::DBManager::trackCR).trackURI], temp_row[(*Dingo::DBManager::trackCR).trackMIMEType], temp_row[(*Dingo::DBManager::trackCR).trackID], temp_row[(*Dingo::DBManager::trackCR).trackSubtitleURI]);
        
        d_playing_track_id = temp_row[(*Dingo::DBManager::trackCR).trackID];
      
        highlightRow(temp_cur_track_iter);
        
        d_playerengine->play();
  
        d_playerengine->notifyEngineStatusObserver(Dingo::ENGINE_NEW_PLAYING);
        
        Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Metadata");
      }
        
      break;
    }
    
    default: {
      break;
    }
  }
}

void Dingo::TrackTreeView::updateGetNextTrackInfo() {
  Gtk::TreeModel::iterator temp_edit_iter = get_model()->get_iter(d_track_sort->convert_child_path_to_path(d_track_filter->convert_child_path_to_path(cur_edit_row_ref.get_path())));
  
  //first unselect the currently selected rows
  if (temp_edit_iter) {
    get_selection()->unselect(temp_edit_iter);
  }
  
  if (cur_track_row_ref.is_valid()) {
    Gtk::TreeModel::iterator temp_cur_track_iter = get_model()->get_iter(d_track_sort->convert_child_path_to_path(d_track_filter->convert_child_path_to_path(cur_track_row_ref.get_path())));
    get_selection()->unselect(temp_cur_track_iter);
  }

  Gtk::TreeModel::Row temp_edit_row;
  
  //then we can move to the next row
  if (cur_next_edit_row_ref.is_valid()) {
    temp_edit_iter = get_model()->get_iter(d_track_sort->convert_child_path_to_path(d_track_filter->convert_child_path_to_path(cur_next_edit_row_ref.get_path())));
  }
  
  else {
    ++temp_edit_iter;
  
    if (temp_edit_iter) {
      temp_edit_row = *temp_edit_iter;
    }
  
    else {
      Gtk::TreeModel::Children children = get_model()->children();
    
      temp_edit_iter = children.begin();
    
      temp_edit_row = *temp_edit_iter;
    }
  }
  
  //save the newly edit row to cur_edit_row_ref
  Gtk::TreeRowReference temp_edit_row_ref(Dingo::DBManager::trackModel, d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(get_model()->get_path(temp_edit_iter))));
  
  cur_edit_row_ref = temp_edit_row_ref;
  
  //save next_edit_row_ref & prev_edit_row_ref 
  saveNextTrackRowRef();
  savePreviousTrackRowRef();
  
  //select the cur_edit_row_ref
  get_selection()->select(temp_edit_iter);
  
  //set DisplayTrack of d_trackinfowindow to the cur_edit_row_ref
  d_trackinfowindow.setDisplayTrack(cur_edit_row_ref);
}

void Dingo::TrackTreeView::updateGetPreviousTrackInfo() {
  Gtk::TreeModel::iterator temp_edit_iter = get_model()->get_iter(d_track_sort->convert_child_path_to_path(d_track_filter->convert_child_path_to_path(cur_edit_row_ref.get_path())));
  
  //first unselect the currently selected rows
  if (temp_edit_iter) {
    get_selection()->unselect(temp_edit_iter);
  }
  
  if (cur_track_row_ref.is_valid()) {
    Gtk::TreeModel::iterator temp_cur_track_iter = get_model()->get_iter(d_track_sort->convert_child_path_to_path(d_track_filter->convert_child_path_to_path(cur_track_row_ref.get_path())));
    get_selection()->unselect(temp_cur_track_iter);
  }

  Gtk::TreeModel::Row temp_edit_row;

  //then we can move to the next row
  if (cur_prev_edit_row_ref.is_valid()) {
    temp_edit_iter = get_model()->get_iter(d_track_sort->convert_child_path_to_path(d_track_filter->convert_child_path_to_path(cur_prev_edit_row_ref.get_path())));
  }
  
  else {  
    --temp_edit_iter;
  
    if (temp_edit_iter) {
      temp_edit_row = *temp_edit_iter;
    }
  
    else {
      Gtk::TreeModel::Children children = get_model()->children();
    
      temp_edit_iter = children.begin();
    
      temp_edit_row = *temp_edit_iter;
    }
  }
  
  //save the newly edit row to cur_edit_row_ref
  Gtk::TreeRowReference temp_edit_row_ref(Dingo::DBManager::trackModel, d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(get_model()->get_path(temp_edit_iter))));
  
  cur_edit_row_ref = temp_edit_row_ref;
  
  //save next_edit_row_ref & prev_edit_row_ref 
  saveNextTrackRowRef();
  savePreviousTrackRowRef();
  
  //select the cur_edit_row_ref
  get_selection()->select(temp_edit_iter);
  
  //set DisplayTrack of d_trackinfowindow to the cur_edit_row_ref
  d_trackinfowindow.setDisplayTrack(cur_edit_row_ref);
}

void Dingo::TrackTreeView::updateSettingsRead() {
  //set_sort_column_id() & set_sort_type()
  d_track_sort->set_sort_column(d_settings->getSortColumnID(), d_settings->getSortType());
  
  //set columns' widths
  get_column(2)->set_fixed_width(d_settings->getWidth("tracktitle"));
  get_column(3)->set_fixed_width(d_settings->getWidth("trackgenre"));
  get_column(4)->set_fixed_width(d_settings->getWidth("trackartist"));
  get_column(5)->set_fixed_width(d_settings->getWidth("trackalbum"));
  get_column(6)->set_fixed_width(d_settings->getWidth("trackyear"));
}

void Dingo::TrackTreeView::updateSettingsWrite() {
  int temp_sort_column_id = 3;
  Gtk::SortType temp_sort_type = Gtk::SORT_ASCENDING;
  
  d_track_sort->get_sort_column_id(temp_sort_column_id, temp_sort_type);

  //write sort_column_id & sort_type
  d_settings->setSortColumnID(temp_sort_column_id);
  d_settings->setSortType(temp_sort_type);
  
  //write columns' widths
  d_settings->setWidth("tracktitle", get_column(2)->get_fixed_width());
  d_settings->setWidth("trackgenre", get_column(3)->get_fixed_width());
  d_settings->setWidth("trackartist", get_column(4)->get_fixed_width());
  d_settings->setWidth("trackalbum", get_column(5)->get_fixed_width());
  d_settings->setWidth("trackyear", get_column(6)->get_fixed_width());
}

//CREATORS
void Dingo::TrackTreeView::createTrackTreeView() {
  d_track_filter = Gtk::TreeModelFilter::create(Dingo::DBManager::trackModel);
  
  d_track_filter->set_visible_column((*Dingo::DBManager::trackCR).trackVisible);
  
  d_statusbar->push(d_dbmanager->getPlaylistInfo());
  
  d_track_sort = Gtk::TreeModelSort::create(d_track_filter);
  
  set_model(d_track_sort);
  set_rules_hint(true);
  
  filterTracks();
  
  d_status_header_image.show();
  
  //create "Status" column
  Gtk::CellRendererPixbuf* p_renderer_trackstatus = Gtk::manage(new Gtk::CellRendererPixbuf());
  append_column(_("Status"), *p_renderer_trackstatus);
  
  //create "Track" column
  Gtk::CellRendererText* p_renderer_tracknumber = Gtk::manage(new Gtk::CellRendererText());
  append_column(_("Track"), *p_renderer_tracknumber);
  
  //create "Title" column
  Gtk::CellRendererText* p_renderer_tracktitle = Gtk::manage(new Gtk::CellRendererText());
  append_column(_("Title"), *p_renderer_tracktitle);
  
  //create "Genre" column
  Gtk::CellRendererText* p_renderer_trackgenre = Gtk::manage(new Gtk::CellRendererText());
  append_column(_("Genre"), *p_renderer_trackgenre);
  
  //create "Artist" column
  Gtk::CellRendererText* p_renderer_trackartist = Gtk::manage(new Gtk::CellRendererText());
  append_column(_("Artist"), *p_renderer_trackartist);
  
  //create "Album" column
  Gtk::CellRendererText* p_renderer_trackalbum = Gtk::manage(new Gtk::CellRendererText());
  append_column(_("Album"), *p_renderer_trackalbum);
  
  //create "Year" column
  Gtk::CellRendererText* p_renderer_trackyear = Gtk::manage(new Gtk::CellRendererText());
  append_column(_("Year"), *p_renderer_trackyear);
  
  //create "Time" column
  Gtk::CellRendererText* p_renderer_tracktime = Gtk::manage(new Gtk::CellRendererText());
  append_column(_("Time"), *p_renderer_tracktime);
  
  //create "Ext." column
  Gtk::CellRendererText* p_renderer_trackfileextension = Gtk::manage(new Gtk::CellRendererText());
  append_column(_("Ext."), *p_renderer_trackfileextension);
  
  //create "Quality" column
  Gtk::CellRendererText* p_renderer_trackbitrate = Gtk::manage(new Gtk::CellRendererText());
  append_column(_("Quality"), *p_renderer_trackbitrate);
  
  Gtk::TreeView::Column* p_column = get_column(0);
  if (p_column) {
    p_column->add_attribute(p_renderer_trackstatus->property_pixbuf(), (*Dingo::DBManager::trackCR).trackStatus);
    p_column->set_widget(d_status_header_image);
    p_column->set_expand(false);
    p_column->set_resizable(false);
  }
  
  p_column = get_column(1);
  if (p_column) {
    p_column->add_attribute(p_renderer_tracknumber->property_markup(), (*Dingo::DBManager::trackCR).trackNumber);
    p_column->set_expand(false);
    p_column->set_resizable(false);
  }
    
  p_column = get_column(2);
  if (p_column) {
    p_column->add_attribute(p_renderer_tracktitle->property_markup(), (*Dingo::DBManager::trackCR).trackTitle);
    p_column->set_sort_column((*Dingo::DBManager::trackCR).trackTitle);
    d_track_sort->set_sort_func((*Dingo::DBManager::trackCR).trackTitle, sigc::mem_fun(*this, &Dingo::TrackTreeView::onTrackTitleCompare));
    p_column->set_sort_indicator(true);
    p_column->set_expand(true);
    p_column->set_clickable(true);
    p_column->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
    p_column->set_resizable(true);
  }
  
  p_column = get_column(3);
  if (p_column) {
    p_column->add_attribute(p_renderer_trackgenre->property_markup(), (*Dingo::DBManager::trackCR).trackGenre);
    p_column->set_sort_column((*Dingo::DBManager::trackCR).trackGenre);
    d_track_sort->set_sort_func((*Dingo::DBManager::trackCR).trackGenre, sigc::mem_fun(*this, &Dingo::TrackTreeView::onTrackGenreCompare));
    p_column->set_sort_indicator(true);
    p_column->set_expand(true);
    p_column->set_clickable(true);
    p_column->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
    p_column->set_resizable(true);
  }
  
  p_column = get_column(4);
  if (p_column) {
    p_column->add_attribute(p_renderer_trackartist->property_markup(), (*Dingo::DBManager::trackCR).trackArtist);
    p_column->set_sort_column((*Dingo::DBManager::trackCR).trackArtist);
    d_track_sort->set_sort_func((*Dingo::DBManager::trackCR).trackArtist, sigc::mem_fun(*this, &Dingo::TrackTreeView::onTrackArtistCompare));
    p_column->set_sort_indicator(true);
    p_column->set_expand(true);
    p_column->set_clickable(true);
    p_column->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
    p_column->set_resizable(true);
  }
  
  p_column = get_column(5);
  if (p_column) {
    p_column->add_attribute(p_renderer_trackalbum->property_markup(), (*Dingo::DBManager::trackCR).trackAlbum);
    p_column->set_sort_column((*Dingo::DBManager::trackCR).trackAlbum);
    d_track_sort->set_sort_func((*Dingo::DBManager::trackCR).trackAlbum, sigc::mem_fun(*this, &Dingo::TrackTreeView::onTrackAlbumCompare));
    p_column->set_sort_indicator(true);
    p_column->set_expand(true);
    p_column->set_clickable(true);
    p_column->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
    p_column->set_resizable(true);
  }
  
  p_column = get_column(6);
  if (p_column) {
    p_column->add_attribute(p_renderer_trackyear->property_markup(), (*Dingo::DBManager::trackCR).trackYear);
    p_column->set_sort_column((*Dingo::DBManager::trackCR).trackYear);
    d_track_sort->set_sort_func((*Dingo::DBManager::trackCR).trackYear, sigc::mem_fun(*this, &Dingo::TrackTreeView::onTrackYearCompare));
    p_column->set_sort_indicator(true);
    p_column->set_expand(true);
    p_column->set_clickable(true);
    p_column->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
    p_column->set_resizable(true);
  }
  
  p_column = get_column(7);
  if (p_column) {
    p_column->add_attribute(p_renderer_tracktime->property_markup(), (*Dingo::DBManager::trackCR).trackTime);
    p_column->set_expand(false);
    p_column->set_resizable(false);
  }
  
  p_column = get_column(8);
  if (p_column) {
    p_column->add_attribute(p_renderer_trackfileextension->property_markup(), (*Dingo::DBManager::trackCR).trackFileExtension);
    p_column->set_sort_column((*Dingo::DBManager::trackCR).trackFileExtension);
    d_track_sort->set_sort_func((*Dingo::DBManager::trackCR).trackFileExtension, sigc::mem_fun(*this, &Dingo::TrackTreeView::onTrackFileExtensionCompare));
    p_column->set_sort_indicator(true);
    p_column->set_expand(false);
    p_column->set_clickable(true);
    p_column->set_resizable(false);
  }
  
  p_column = get_column(9);
  if (p_column) {
    p_column->add_attribute(p_renderer_trackbitrate->property_markup(), (*Dingo::DBManager::trackCR).trackBitrate);
    p_column->set_sort_column((*Dingo::DBManager::trackCR).trackBitrate);
    d_track_sort->set_sort_func((*Dingo::DBManager::trackCR).trackBitrate, sigc::mem_fun(*this, &Dingo::TrackTreeView::onTrackBitrateCompare));
    p_column->set_sort_indicator(true);
    p_column->set_expand(false);
    p_column->set_clickable(true);
    p_column->set_resizable(false);
  }
  
  get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
  signal_row_activated().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onTreeViewRowDoubleClicked));
}

void Dingo::TrackTreeView::createPopupMenu() {
  track_popup_menu.append(add_to);
  track_popup_menu.append(delete_from);
  track_popup_menu.append(*(Gtk::manage(new Gtk::SeparatorMenuItem())));
  track_popup_menu.append(cut_tracks);
  track_popup_menu.append(copy_tracks);
  track_popup_menu.append(paste_tracks);
  track_popup_menu.append(*(Gtk::manage(new Gtk::SeparatorMenuItem())));
  track_popup_menu.append(edit_info);
  track_popup_menu.append(edit_all_info);
  track_popup_menu.append(*(Gtk::manage(new Gtk::SeparatorMenuItem())));
  track_popup_menu.append(lyrics);
  
  cut_tracks.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onCutTracksActivated));
  copy_tracks.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onCopyTracksActivated));
  paste_tracks.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onPasteTracksActivated));
  edit_info.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onEditInfoActivated));
  edit_all_info.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onEditAllInfoActivated));
  lyrics.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onLyricsActivated));
  
  track_popup_menu.show_all_children();
}

void Dingo::TrackTreeView::createAddSubMenu() {
  add_submenu.append(add_play_queue);
  add_submenu.append(*(Gtk::manage(new Gtk::SeparatorMenuItem())));
  add_submenu.append(add_playlists);
  
  add_play_queue.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onAddToPlayQueueActivated));
  add_playlists.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onAddToNormalPlaylistActivated));
  
  add_to.set_submenu(add_submenu);
  add_to.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onAddToSubMenuActivated));
  
  add_submenu.show_all();
}

void Dingo::TrackTreeView::createDeleteSubMenu() {
  delete_submenu.append(delete_database);
  delete_submenu.append(delete_play_queue);
  delete_submenu.append(*(Gtk::manage(new Gtk::SeparatorMenuItem())));
  delete_submenu.append(delete_hard_drive);
  delete_submenu.append(*(Gtk::manage(new Gtk::SeparatorMenuItem())));
  delete_submenu.append(delete_playlists);
  delete_submenu.append(*(Gtk::manage(new Gtk::SeparatorMenuItem())));
  delete_submenu.append(delete_current_playlist);
  
  delete_database.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onDeleteFromDatabaseActivated));
  delete_play_queue.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onDeleteFromPlayQueueActivated));
  delete_hard_drive.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onDeleteFromHardDriveActivated));
  delete_playlists.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onDeleteFromNormalPlaylistActivated));
  delete_current_playlist.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onDeleteFromCurrentPlaylistActivated));
  
  delete_from.set_submenu(delete_submenu);
  delete_from.signal_activate().connect(sigc::mem_fun(*this, &Dingo::TrackTreeView::onDeleteFromSubMenuActivated));
  
  delete_submenu.show_all_children();
}

//SIGNAL HANDLERS
void Dingo::TrackTreeView::onTreeViewRowDoubleClicked(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column) {
  Gtk::TreeModel::iterator cur_track_iter;

  if (cur_track_row_ref) {
    cur_track_iter = Dingo::DBManager::trackModel->get_iter(cur_track_row_ref.get_path());
  }
  
  else {
    cur_track_iter = Dingo::DBManager::trackModel->children().begin();
  }

  if (cur_track_iter) {
    unhighlightRow(cur_track_iter);
  }
  
  cur_track_iter = Dingo::DBManager::trackModel->get_iter(d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(path)));
  
  //save the the current row to cur_track_row_ref
  Gtk::TreeRowReference temp_track_row_ref(Dingo::DBManager::trackModel, d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(path)));
  
  cur_track_row_ref = temp_track_row_ref;
  
  d_dbmanager->setCurrentTrackRowRef(cur_track_row_ref);
  
  //play the file
  Gtk::TreeModel::Row temp_row = *cur_track_iter;
  
  if ((d_playerengine->getStatus() == Dingo::ENGINE_PLAYING) || (d_playerengine->getStatus() == Dingo::ENGINE_PAUSED)) {
    d_playerengine->setStatus(Dingo::ENGINE_STOPPED);
  }
  //the error is here in the read() step
  d_playerengine->read(temp_row[(*Dingo::DBManager::trackCR).trackURI], temp_row[(*Dingo::DBManager::trackCR).trackMIMEType], temp_row[(*Dingo::DBManager::trackCR).trackID], temp_row[(*Dingo::DBManager::trackCR).trackSubtitleURI]);
  
  d_playing_track_id = temp_row[(*Dingo::DBManager::trackCR).trackID];
  
  //notify TrackTreeViewObserver
  notifyTrackTreeViewObserver(Dingo::TRACK_TREE_ROW_ACTIVATED);
  
  //highlight the new row
  highlightRow(cur_track_iter);
  
  d_playerengine->play();
  
  d_playerengine->notifyEngineStatusObserver(Dingo::ENGINE_NEW_PLAYING);
  
  Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Metadata");
}

void Dingo::TrackTreeView::onEditInfoActivated() {
  std::vector<Gtk::TreeModel::Path> temp_path_vector = get_selection()->get_selected_rows();
  
  Gtk::TreeRowReference temp_edit_row_ref(Dingo::DBManager::trackModel, d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(temp_path_vector[0])));
  
  cur_edit_row_ref = temp_edit_row_ref;
  
  //save next_edit_row_ref & prev_edit_row_ref 
  saveNextTrackRowRef();
  savePreviousTrackRowRef();
  
  get_selection()->select(temp_path_vector[0]);
  
  d_trackinfowindow.set_transient_for(*(d_dbmanager->getWindow()));
  
  d_trackinfowindow.show_all();
  
  d_trackinfowindow.setDisplayTrack(cur_edit_row_ref);
  
  d_trackinfowindow.setCurrentPage(0);
}

void Dingo::TrackTreeView::onEditAllInfoActivated() {
  std::vector<Gtk::TreeModel::Path> temp_path_vector = get_selection()->get_selected_rows();
  
  for (std::vector<Gtk::TreeModel::Path>::iterator iter = temp_path_vector.begin(); iter != temp_path_vector.end(); ++iter) {
    Gtk::TreeRowReference temp_edit_row_ref(Dingo::DBManager::trackModel, d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(*iter)));
    
    cur_edit_row_vector.push_back(temp_edit_row_ref);
  }
  
  d_multipleinfowindow.setDisplayTracks(cur_edit_row_vector);
  
  cur_edit_row_vector.clear();
  
  d_multipleinfowindow.set_transient_for(*(d_dbmanager->getWindow()));
  
  d_multipleinfowindow.show_all();
}

void Dingo::TrackTreeView::onLyricsActivated() {
  onEditInfoActivated();
  
  d_trackinfowindow.setCurrentPage(2);
}

void Dingo::TrackTreeView::onAddToSubMenuActivated() {
  if (d_dbmanager->getCurrentPlaylistID() == 1) {
    add_play_queue.set_sensitive(false);
  }
  
  else {
    add_play_queue.set_sensitive(true);
  }
}

void Dingo::TrackTreeView::onAddToPlayQueueActivated() {
  //no need to refilter PlayQueueModel
  std::vector<Gtk::TreeModel::Path> selected_path_vector = get_selection()->get_selected_rows();
  std::vector<int> selected_tracks_id;
  
  for (std::vector<Gtk::TreeModel::Path>::iterator path_iter = selected_path_vector.begin(); path_iter != selected_path_vector.end(); ++path_iter) {
    Gtk::TreeModel::iterator child_iter = Dingo::DBManager::trackModel->get_iter(d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(*path_iter)));
  
    Gtk::TreeModel::Row row = *child_iter;
    
    selected_tracks_id.push_back(row[(*Dingo::DBManager::trackCR).trackID]);
  }
  
  d_dbmanager->addTracksToPlayQueue(selected_tracks_id);
  
  //here you just add to the database - not the treeview. You still have to refilter this treeview
  if (d_dbmanager->getCurrentPlaylistID() == 1) {
    refreshData();
  }
}

void Dingo::TrackTreeView::onAddToNormalPlaylistActivated() {
  std::vector<Gtk::TreeModel::Path> selected_path_vector = get_selection()->get_selected_rows();
  std::vector<Gtk::TreeRowReference> selected_row_ref_vector;
  
  for (std::vector<Gtk::TreeModel::Path>::iterator path_iter = selected_path_vector.begin(); path_iter != selected_path_vector.end(); ++path_iter) {
    Gtk::TreeRowReference temp_selected_row_ref(Dingo::DBManager::trackModel, d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(*path_iter)));
    
    selected_row_ref_vector.push_back(temp_selected_row_ref);
  }
  
  d_trackplaylisteditwindow.setDisplayTracks(selected_row_ref_vector);
  
  d_trackplaylisteditwindow.set_transient_for(*(d_dbmanager->getWindow()));
  
  d_trackplaylisteditwindow.show_all();
}

void Dingo::TrackTreeView::onDeleteFromSubMenuActivated() {
  if (d_dbmanager->getCurrentPlaylistID() == 0) {
    delete_current_playlist.set_sensitive(false);
  }
  
  else {
    delete_current_playlist.set_sensitive(true);
  }
}

void Dingo::TrackTreeView::onDeleteFromDatabaseActivated() {
  Gtk::Image question_image(Gtk::Stock::DIALOG_QUESTION, Gtk::ICON_SIZE_DIALOG);
  Gtk::Label del_from_db_label(_("Remove selected track(s) from the Database?"));
  Gtk::Grid top_grid;
  
  top_grid.attach(question_image, 0, 0, 2, 2);
  top_grid.attach(del_from_db_label, 2, 0, 1, 1);
  
  top_grid.set_row_spacing(7);
  top_grid.set_column_spacing(10);
  top_grid.set_border_width(5);
  
  Gtk::Dialog dialog("", true);
  
  Gtk::Box* p_vbox = dialog.get_content_area();
  
  p_vbox->pack_start(top_grid, Gtk::PACK_EXPAND_WIDGET);
  
  dialog.add_button(_("No"), Gtk::RESPONSE_NO);
  dialog.add_button(_("Yes"), Gtk::RESPONSE_YES);
  
  dialog.set_default_response(Gtk::RESPONSE_YES);
  
  dialog.show_all_children();
  
  dialog.set_transient_for(*(d_dbmanager->getWindow()));
  
  int result = dialog.run();
  
  dialog.hide();
  
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  switch (result) {
    case Gtk::RESPONSE_YES: {
      //std::cout << "Remove Media from Database Dialog: Yes button clicked!" << std::endl;
      
      std::vector<Gtk::TreeModel::Path> selected_path_vector = get_selection()->get_selected_rows();
      
      std::vector<Gtk::TreeRowReference> selected_ref_vector;
      
      std::vector<int> selected_trackids_vector;
      
      for (std::vector<Gtk::TreeModel::Path>::iterator path_iter = selected_path_vector.begin(); path_iter != selected_path_vector.end(); ++path_iter) {
        Gtk::TreeModel::Path temp_path = d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(*path_iter));
        
        Gtk::TreeModel::iterator temp_iter = Dingo::DBManager::trackModel->get_iter(temp_path);
        
        Gtk::TreeModel::Row temp_row = *temp_iter;
      
        selected_trackids_vector.push_back(temp_row[(*Dingo::DBManager::trackCR).trackID]);
      
        Gtk::TreeRowReference temp_ref(Dingo::DBManager::trackModel, temp_path);
      
        selected_ref_vector.push_back(temp_ref);
        
        if (d_playing_track_id == temp_row[(*Dingo::DBManager::trackCR).trackID]) {
          d_playerengine->stop();
          
          d_playing_track_id = 0;
          
          notifyTrackTreeViewObserver(Dingo::TRACK_TREE_ROW_DEACTIVATED);
          
          Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Metadata");
        }
      }
      
      for (std::vector<Gtk::TreeRowReference>::iterator ref_iter = selected_ref_vector.begin(); ref_iter != selected_ref_vector.end(); ++ref_iter) {
        Gtk::TreeModel::Path temp_path = ref_iter->get_path();
        
        Gtk::TreeModel::iterator temp_iter = Dingo::DBManager::trackModel->get_iter(temp_path);
        
        Dingo::DBManager::trackModel->erase(temp_iter);
      }
      
      d_dbmanager->deleteTracksFromDatabase(selected_trackids_vector);
  
      if (d_settings->getVisible("playlistbrowser")) {
        d_dbmanager->notifyDBManPlaylistObserver(Dingo::CHANGES_IN_CORE_PLAYLISTS);
      }
  
      if (d_settings->getVisible("genrefilter")) {
        d_dbmanager->notifyDBManGenreObserver(Dingo::CHANGES_IN_CORE_GENRES);
      }
  
      if (d_settings->getVisible("artistfilter")) {
        d_dbmanager->notifyDBManArtistObserver(Dingo::CHANGES_IN_CORE_ARTISTS);
      }
  
      if (d_settings->getVisible("yearfilter")) {
        d_dbmanager->notifyDBManYearObserver(Dingo::CHANGES_IN_CORE_YEARS);
      }
  
      if (d_settings->getVisible("albumfilter")) {
        d_dbmanager->notifyDBManAlbumObserver(Dingo::CHANGES_IN_CORE_ALBUMS);
      }
      
      break;
    }
    
    case Gtk::RESPONSE_NO: {
      //std::cout << "Remove Media from Database Dialog: No button clicked!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Remove Media from Database Dialog: Unknown button clicked!" << std::endl;
      break;
    }
  }
}

void Dingo::TrackTreeView::onDeleteFromPlayQueueActivated() {
  Gtk::Image question_image(Gtk::Stock::DIALOG_QUESTION, Gtk::ICON_SIZE_DIALOG);
  Gtk::Label del_from_playqueue_label(_("Remove selected track(s) from Play Queue?"));
  Gtk::Grid top_grid;
  
  top_grid.attach(question_image, 0, 0, 2, 2);
  top_grid.attach(del_from_playqueue_label, 2, 0, 1, 1);
  
  top_grid.set_row_spacing(7);
  top_grid.set_column_spacing(10);
  top_grid.set_border_width(5);
  
  Gtk::Dialog dialog("", true);
  
  Gtk::Box* p_vbox = dialog.get_content_area();
  
  p_vbox->pack_start(top_grid, Gtk::PACK_EXPAND_WIDGET);
  
  dialog.add_button(_("No"), Gtk::RESPONSE_NO);
  dialog.add_button(_("Yes"), Gtk::RESPONSE_YES);
  
  dialog.set_default_response(Gtk::RESPONSE_YES);
  
  dialog.show_all_children();
  
  dialog.set_transient_for(*(d_dbmanager->getWindow()));
  
  int result = dialog.run();
  
  dialog.hide();
  
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  switch (result) {
    case Gtk::RESPONSE_YES: {
      //std::cout << "Remove Media from Play Queue Dialog: Yes button clicked!" << std::endl;
      
      std::vector<Gtk::TreeModel::Path> selected_path_vector = get_selection()->get_selected_rows();
      
      std::vector<int> selected_trackids_vector;
      
      for (std::vector<Gtk::TreeModel::Path>::iterator path_iter = selected_path_vector.begin(); path_iter != selected_path_vector.end(); ++path_iter) {
        Gtk::TreeModel::iterator temp_iter = Dingo::DBManager::trackModel->get_iter(d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(*path_iter)));
        
        Gtk::TreeModel::Row temp_row = *temp_iter;
        
        selected_trackids_vector.push_back(temp_row[(*Dingo::DBManager::trackCR).trackID]);
      }
      
      d_dbmanager->deleteTracksFromPlayQueue(selected_trackids_vector);
  
      if (d_settings->getVisible("genrefilter")) {
        d_dbmanager->notifyDBManGenreObserver(Dingo::CHANGES_IN_CORE_GENRES);
      }
  
      if (d_settings->getVisible("artistfilter")) {
        d_dbmanager->notifyDBManArtistObserver(Dingo::CHANGES_IN_CORE_ARTISTS);
      }
  
      if (d_settings->getVisible("yearfilter")) {
        d_dbmanager->notifyDBManYearObserver(Dingo::CHANGES_IN_CORE_YEARS);
      }
  
      if (d_settings->getVisible("albumfilter")) {
        d_dbmanager->notifyDBManAlbumObserver(Dingo::CHANGES_IN_CORE_ALBUMS);
      }
      
      if (d_dbmanager->getCurrentPlaylistID() == 1) {
        refreshData();
      }
      
      break;
    }
    
    case Gtk::RESPONSE_NO: {
      //std::cout << "Remove Media from Play Queue Dialog: No button clicked!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Remove Media from Play Queue Dialog: Unknown button clicked!" << std::endl;
      break;
    }
  }
}

void Dingo::TrackTreeView::onDeleteFromHardDriveActivated() {
  Gtk::Image question_image(Gtk::Stock::DIALOG_QUESTION, Gtk::ICON_SIZE_DIALOG);
  Gtk::Label del_from_harddrive_label(_("Delete selected track(s) from Hard Drive?"));
  Gtk::Grid top_grid;
  
  top_grid.attach(question_image, 0, 0, 2, 2);
  top_grid.attach(del_from_harddrive_label, 2, 0, 1, 1);
  
  top_grid.set_row_spacing(7);
  top_grid.set_column_spacing(10);
  top_grid.set_border_width(5);
  
  Gtk::Dialog dialog("", true);
  
  Gtk::Box* p_vbox = dialog.get_content_area();
  
  p_vbox->pack_start(top_grid, Gtk::PACK_EXPAND_WIDGET);
  
  dialog.add_button(_("No"), Gtk::RESPONSE_NO);
  dialog.add_button(_("Yes"), Gtk::RESPONSE_YES);
  
  dialog.set_default_response(Gtk::RESPONSE_YES);
  
  dialog.show_all_children();
  
  dialog.set_transient_for(*(d_dbmanager->getWindow()));
  
  int result = dialog.run();
  
  dialog.hide();
  
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  switch (result) {
    case Gtk::RESPONSE_YES: {
      //std::cout << "Delete Media from Hard Drive Dialog: Yes button clicked!" << std::endl;
      
      std::vector<Gtk::TreeModel::Path> selected_path_vector = get_selection()->get_selected_rows();
      
      std::vector<Gtk::TreeRowReference> selected_ref_vector;
      
      std::vector<int> selected_trackids_vector;
      
      for (std::vector<Gtk::TreeModel::Path>::iterator path_iter = selected_path_vector.begin(); path_iter != selected_path_vector.end(); ++path_iter) {
        Gtk::TreeModel::Path temp_path = d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(*path_iter));
        
        Gtk::TreeModel::iterator temp_iter = Dingo::DBManager::trackModel->get_iter(temp_path);
        
        Gtk::TreeModel::Row temp_row = *temp_iter;
        
        selected_trackids_vector.push_back(temp_row[(*Dingo::DBManager::trackCR).trackID]);
      
        Gtk::TreeRowReference temp_ref(Dingo::DBManager::trackModel, temp_path);
      
        selected_ref_vector.push_back(temp_ref);
        
        if (d_playing_track_id == temp_row[(*Dingo::DBManager::trackCR).trackID]) {
          d_playerengine->stop();
          
          d_playing_track_id = 0;
          
          notifyTrackTreeViewObserver(Dingo::TRACK_TREE_ROW_DEACTIVATED);
          
          Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Metadata");
        }
      }
      
      for (std::vector<Gtk::TreeRowReference>::iterator ref_iter = selected_ref_vector.begin(); ref_iter != selected_ref_vector.end(); ++ref_iter) {
        Gtk::TreeModel::Path temp_path = ref_iter->get_path();
        
        Gtk::TreeModel::iterator temp_iter = Dingo::DBManager::trackModel->get_iter(temp_path);
        
        Dingo::DBManager::trackModel->erase(temp_iter);
      }
      
      d_dbmanager->deleteTracksFromHardDrive(selected_trackids_vector);
  
      if (d_settings->getVisible("playlistbrowser")) {
        d_dbmanager->notifyDBManPlaylistObserver(Dingo::CHANGES_IN_CORE_PLAYLISTS);
      }
  
      if (d_settings->getVisible("genrefilter")) {
        d_dbmanager->notifyDBManGenreObserver(Dingo::CHANGES_IN_CORE_GENRES);
      }
  
      if (d_settings->getVisible("artistfilter")) {
        d_dbmanager->notifyDBManArtistObserver(Dingo::CHANGES_IN_CORE_ARTISTS);
      }
  
      if (d_settings->getVisible("yearfilter")) {
        d_dbmanager->notifyDBManYearObserver(Dingo::CHANGES_IN_CORE_YEARS);
      }
  
      if (d_settings->getVisible("albumfilter")) {
        d_dbmanager->notifyDBManAlbumObserver(Dingo::CHANGES_IN_CORE_ALBUMS);
      }
      
      break;
    }
    
    case Gtk::RESPONSE_NO: {
      //std::cout << "Delete Media from Hard Drive Dialog: No button clicked!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Delete Media from Hard Drive Dialog: Unknown button clicked!" << std::endl;
      break;
    }
  }
}

void Dingo::TrackTreeView::onDeleteFromCurrentPlaylistActivated() {
  Gtk::Image question_image(Gtk::Stock::DIALOG_QUESTION, Gtk::ICON_SIZE_DIALOG);
  Gtk::Label del_from_curpls_label(_("Remove selected track(s) from Current Playlist?"));
  Gtk::Grid top_grid;
  
  top_grid.attach(question_image, 0, 0, 2, 2);
  top_grid.attach(del_from_curpls_label, 2, 0, 1, 1);
  
  top_grid.set_row_spacing(7);
  top_grid.set_column_spacing(10);
  top_grid.set_border_width(5);
  
  Gtk::Dialog dialog("", true);
  
  Gtk::Box* p_vbox = dialog.get_content_area();
  
  p_vbox->pack_start(top_grid, Gtk::PACK_EXPAND_WIDGET);

  dialog.add_button(_("No"), Gtk::RESPONSE_NO);
  dialog.add_button(_("Yes"), Gtk::RESPONSE_YES);
  
  dialog.set_default_response(Gtk::RESPONSE_YES);
  
  dialog.show_all_children();
  
  dialog.set_transient_for(*(d_dbmanager->getWindow()));
  
  int result = dialog.run();
  
  dialog.hide();
  
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  switch (result) {
    case Gtk::RESPONSE_YES: {
      //std::cout << "Remove Media from Current Playlist Dialog: Yes button clicked!" << std::endl;
      
      std::vector<Gtk::TreeModel::Path> selected_path_vector = get_selection()->get_selected_rows();
      
      std::vector<int> selected_trackids_vector;
      
      for (std::vector<Gtk::TreeModel::Path>::iterator path_iter = selected_path_vector.begin(); path_iter != selected_path_vector.end(); ++path_iter) {
        Gtk::TreeModel::Row temp_row = *(Dingo::DBManager::trackModel->get_iter(d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(*path_iter))));
        
        selected_trackids_vector.push_back(temp_row[(*Dingo::DBManager::trackCR).trackID]);
      }
      
      d_dbmanager->deleteTracksFromNormalPlaylist(selected_trackids_vector, d_dbmanager->getCurrentPlaylistID());
      
      if (d_dbmanager->getCurrentPlaylistID() == 1) {
        refreshData();
      }
      
      if (d_settings->getVisible("playlistbrowser")) {
        d_dbmanager->notifyDBManPlaylistObserver(Dingo::CHANGES_IN_CORE_PLAYLISTS);
      }
  
      if (d_settings->getVisible("genrefilter")) {
        d_dbmanager->notifyDBManGenreObserver(Dingo::CHANGES_IN_CORE_GENRES);
      }
  
      if (d_settings->getVisible("artistfilter")) {
        d_dbmanager->notifyDBManArtistObserver(Dingo::CHANGES_IN_CORE_ARTISTS);
      }
  
      if (d_settings->getVisible("yearfilter")) {
        d_dbmanager->notifyDBManYearObserver(Dingo::CHANGES_IN_CORE_YEARS);
      }
  
      if (d_settings->getVisible("albumfilter")) {
        d_dbmanager->notifyDBManAlbumObserver(Dingo::CHANGES_IN_CORE_ALBUMS);
      }
      
      if (d_settings->getVisible("trackbrowser")) {
        d_dbmanager->notifyDBManTrackObserver(Dingo::CHANGES_IN_CORE_TRACKS);
      }
      
      break;
    }
    
    case Gtk::RESPONSE_NO: {
      //std::cout << "Remove Media from Current Playlist Dialog: No button clicked!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Remove Media from Current Playlist Dialog: Unknown button clicked!" << std::endl;
      break;
    }
  }
}

void Dingo::TrackTreeView::onDeleteFromNormalPlaylistActivated() {
  std::vector<Gtk::TreeModel::Path> selected_path_vector = get_selection()->get_selected_rows();
  std::vector<Gtk::TreeRowReference> selected_row_ref_vector;
  
  for (std::vector<Gtk::TreeModel::Path>::iterator path_iter = selected_path_vector.begin(); path_iter != selected_path_vector.end(); ++path_iter) {
    Gtk::TreeRowReference temp_selected_row_ref(Dingo::DBManager::trackModel, d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(*path_iter)));
    
    selected_row_ref_vector.push_back(temp_selected_row_ref);
  }
  
  d_trackplaylisteditwindow.setDisplayTracks(selected_row_ref_vector);
  
  d_trackplaylisteditwindow.show_all();
}

void Dingo::TrackTreeView::onCutTracksActivated() {
  std::vector<Gtk::TreeModel::Path> selected_path_vector = get_selection()->get_selected_rows();
  std::vector<int> temp_track_id_vector;
  
  for (std::vector<Gtk::TreeModel::Path>::iterator path_iter = selected_path_vector.begin(); path_iter != selected_path_vector.end(); ++path_iter) {
    Gtk::TreeModel::Row temp_selected_row = *(Dingo::DBManager::trackModel->get_iter(d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(*path_iter))));
    
    temp_track_id_vector.push_back(temp_selected_row[(*Dingo::DBManager::trackCR).trackID]);
  }
  
  d_dbmanager->cutTracks(temp_track_id_vector);
  
  if (d_settings->getVisible("playlistbrowser")) {
    d_dbmanager->notifyDBManPlaylistObserver(Dingo::CHANGES_IN_CORE_PLAYLISTS);
  }
  
  if (d_settings->getVisible("genrefilter")) {
    d_dbmanager->notifyDBManGenreObserver(Dingo::CHANGES_IN_CORE_GENRES);
  }
  
  if (d_settings->getVisible("artistfilter")) {
    d_dbmanager->notifyDBManArtistObserver(Dingo::CHANGES_IN_CORE_ARTISTS);
  }
  
  if (d_settings->getVisible("yearfilter")) {
    d_dbmanager->notifyDBManYearObserver(Dingo::CHANGES_IN_CORE_YEARS);
  }
  
  if (d_settings->getVisible("albumfilter")) {
    d_dbmanager->notifyDBManAlbumObserver(Dingo::CHANGES_IN_CORE_ALBUMS);
  }
  
  if (d_settings->getVisible("trackbrowser")) {
    d_dbmanager->notifyDBManTrackObserver(Dingo::CHANGES_IN_CORE_TRACKS);
  }
}

void Dingo::TrackTreeView::onCopyTracksActivated() {
  std::vector<Gtk::TreeModel::Path> selected_path_vector = get_selection()->get_selected_rows();
  std::vector<int> temp_track_id_vector;
  
  for (std::vector<Gtk::TreeModel::Path>::iterator path_iter = selected_path_vector.begin(); path_iter != selected_path_vector.end(); ++path_iter) {
    Gtk::TreeModel::Row temp_selected_row = *(Dingo::DBManager::trackModel->get_iter(d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(*path_iter))));
    
    temp_track_id_vector.push_back(temp_selected_row[(*Dingo::DBManager::trackCR).trackID]);
  }
  
  d_dbmanager->copyTracks(temp_track_id_vector);
}

void Dingo::TrackTreeView::onPasteTracksActivated() {
  d_dbmanager->pasteTracks();
  
  if (d_settings->getVisible("playlistbrowser")) {
    d_dbmanager->notifyDBManPlaylistObserver(Dingo::CHANGES_IN_CORE_PLAYLISTS);
  }
  
  if (d_settings->getVisible("genrefilter")) {
    d_dbmanager->notifyDBManGenreObserver(Dingo::CHANGES_IN_CORE_GENRES);
  }
  
  if (d_settings->getVisible("artistfilter")) {
    d_dbmanager->notifyDBManArtistObserver(Dingo::CHANGES_IN_CORE_ARTISTS);
  }
  
  if (d_settings->getVisible("yearfilter")) {
    d_dbmanager->notifyDBManYearObserver(Dingo::CHANGES_IN_CORE_YEARS);
  }
  
  if (d_settings->getVisible("albumfilter")) {
    d_dbmanager->notifyDBManAlbumObserver(Dingo::CHANGES_IN_CORE_ALBUMS);
  }
  
  if (d_settings->getVisible("trackbrowser")) {
    d_dbmanager->notifyDBManTrackObserver(Dingo::CHANGES_IN_CORE_TRACKS);
  }
}

void Dingo::TrackTreeView::filterTracks() {
  d_dbmanager->prepareQueryShouldTrackBeVisible();
  
  Gtk::TreeModel::Children children = Dingo::DBManager::trackModel->children();
  
  for (Gtk::TreeModel::Children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    
    row[(*Dingo::DBManager::trackCR).trackVisible] = d_dbmanager->getShouldTrackBeVisible(row[(*Dingo::DBManager::trackCR).trackID]);
  }
  
  d_dbmanager->finalizeQueryShouldTrackBeVisible();
}

bool Dingo::TrackTreeView::on_button_press_event(GdkEventButton* event) {
  if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3)) {
    track_popup_menu.popup(event->button, event->time);
    
    if (!getIsAnyTrackSelected()) {
      cut_tracks.set_sensitive(false);
      copy_tracks.set_sensitive(false);
      edit_info.set_sensitive(false);
      edit_all_info.set_sensitive(false);
      add_to.set_sensitive(false);
      delete_from.set_sensitive(false);
      lyrics.set_sensitive(false);
      
      if (d_dbmanager->getCurrentPlaylistID() == 0) {
        paste_tracks.set_sensitive(false);
      }
    
      else {
        paste_tracks.set_sensitive(true);
      }
    }
    
    else {
      if (d_dbmanager->getCurrentPlaylistID() == 0) {
        cut_tracks.set_sensitive(false);
        paste_tracks.set_sensitive(false);
      }
      
      else {
        cut_tracks.set_sensitive(true);
        paste_tracks.set_sensitive(true);
      }
    
      copy_tracks.set_sensitive(true);
      edit_info.set_sensitive(true);
      edit_all_info.set_sensitive(true);
      add_to.set_sensitive(true);
      delete_from.set_sensitive(true);
      lyrics.set_sensitive(true);
    }
    
    if (!d_dbmanager->getIsAnyTrackInClipboard()) {
      paste_tracks.set_sensitive(false);
    }
    
    else {
      paste_tracks.set_sensitive(true);
    }
    
    return false;
  }
  
  return Gtk::TreeView::on_button_press_event(event);
}

int Dingo::TrackTreeView::onTrackTitleCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter) {
  Gtk::TreeModel::Row temp_first_row = *first_iter;
  Gtk::TreeModel::Row temp_second_row = *second_iter;
  
  Glib::ustring first_title_str = Dingo::Utilities::unhighlightString(temp_first_row[(*Dingo::DBManager::trackCR).trackTitle]);
  
  Glib::ustring second_title_str = Dingo::Utilities::unhighlightString(temp_second_row[(*Dingo::DBManager::trackCR).trackTitle]);
  
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

int Dingo::TrackTreeView::onTrackGenreCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter) {
  Gtk::TreeModel::Row temp_first_row = *first_iter;
  Gtk::TreeModel::Row temp_second_row = *second_iter;
  
  Glib::ustring first_genre_str = Dingo::Utilities::unhighlightString(temp_first_row[(*Dingo::DBManager::trackCR).trackGenre]);
  
  Glib::ustring second_genre_str = Dingo::Utilities::unhighlightString(temp_second_row[(*Dingo::DBManager::trackCR).trackGenre]);
  
  if (first_genre_str < second_genre_str) {
    return -1;
  }
  
  else if (first_genre_str > second_genre_str) {
    return 1;
  }
  
  else {
    return 0;
  }
}

int Dingo::TrackTreeView::onTrackArtistCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter) {
  Gtk::TreeModel::Row temp_first_row = *first_iter;
  Gtk::TreeModel::Row temp_second_row = *second_iter;
  
  Glib::ustring first_artist_str = Dingo::Utilities::unhighlightString(temp_first_row[(*Dingo::DBManager::trackCR).trackArtist]);
  
  Glib::ustring second_artist_str = Dingo::Utilities::unhighlightString(temp_second_row[(*Dingo::DBManager::trackCR).trackArtist]);
  
  if (first_artist_str < second_artist_str) {
    return -1;
  }
  
  else if (first_artist_str > second_artist_str) {
    return 1;
  }
  
  else {
    return 0;
  }
}

int Dingo::TrackTreeView::onTrackAlbumCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter) {
  Gtk::TreeModel::Row temp_first_row = *first_iter;
  Gtk::TreeModel::Row temp_second_row = *second_iter;
  
  Glib::ustring first_album_str = Dingo::Utilities::unhighlightString(temp_first_row[(*Dingo::DBManager::trackCR).trackAlbum]);
  
  Glib::ustring second_album_str = Dingo::Utilities::unhighlightString(temp_second_row[(*Dingo::DBManager::trackCR).trackAlbum]);
  
  if (first_album_str < second_album_str) {
    return -1;
  }
  
  else if (first_album_str > second_album_str) {
    return 1;
  }
  
  else {
    return 0;
  }
}

int Dingo::TrackTreeView::onTrackYearCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter) {
  Gtk::TreeModel::Row temp_first_row = *first_iter;
  Gtk::TreeModel::Row temp_second_row = *second_iter;
  
  Glib::ustring first_year_str = Dingo::Utilities::unhighlightString(temp_first_row[(*Dingo::DBManager::trackCR).trackYear]);
  
  Glib::ustring second_year_str = Dingo::Utilities::unhighlightString(temp_second_row[(*Dingo::DBManager::trackCR).trackYear]);
  
  if (first_year_str < second_year_str) {
    return 1;
  }
  
  else if (first_year_str > second_year_str) {
    return -1;
  }
  
  else {
    return 0;
  }
}

int Dingo::TrackTreeView::onTrackFileExtensionCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter) {
  Gtk::TreeModel::Row temp_first_row = *first_iter;
  Gtk::TreeModel::Row temp_second_row = *second_iter;
  
  Glib::ustring first_fileextension_str = Dingo::Utilities::unhighlightString(temp_first_row[(*Dingo::DBManager::trackCR).trackFileExtension]);
  
  Glib::ustring second_fileextension_str = Dingo::Utilities::unhighlightString(temp_second_row[(*Dingo::DBManager::trackCR).trackFileExtension]);
  
  if (first_fileextension_str < second_fileextension_str) {
    return -1;
  }
  
  else if (first_fileextension_str > second_fileextension_str) {
    return 1;
  }
  
  else {
    return 0;
  }
}

int Dingo::TrackTreeView::onTrackBitrateCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter) {
  Gtk::TreeModel::Row temp_first_row = *first_iter;
  Gtk::TreeModel::Row temp_second_row = *second_iter;
  
  Glib::ustring first_bitrate_str = Dingo::Utilities::unhighlightString(temp_first_row[(*Dingo::DBManager::trackCR).trackBitrate]);
  
  Glib::ustring second_bitrate_str = Dingo::Utilities::unhighlightString(temp_second_row[(*Dingo::DBManager::trackCR).trackBitrate]);
  
  if (Dingo::Utilities::bitrateStringToInt(first_bitrate_str) < Dingo::Utilities::bitrateStringToInt(second_bitrate_str)) {
    return 1;
  }
  
  else if (Dingo::Utilities::bitrateStringToInt(first_bitrate_str) > Dingo::Utilities::bitrateStringToInt(second_bitrate_str)) {
    return -1;
  }
  
  else {
    return 0;
  }
}

//PUBLIC ACCESSORS
void Dingo::TrackTreeView::activateRow(Gtk::TreeRowReference row_ref) {
  Gtk::TreeModel::Path tree_path = d_track_sort->convert_child_path_to_path(d_track_filter->convert_child_path_to_path(row_ref.get_path()));
  
  Gtk::TreeView::row_activated(tree_path, *(get_column(0)));
}

bool Dingo::TrackTreeView::activateRow(Glib::ustring track_uri) {
  Gtk::TreeModel::Children children = get_model()->children();
  
  for (Gtk::TreeModel::Children::iterator children_iter = children.begin(); children_iter != children.end(); ++children_iter) {
    Gtk::TreeModel::Row row = *children_iter;
    
    if (row[(*Dingo::DBManager::trackCR).trackURI] == track_uri) {
      Gtk::TreeView::row_activated(get_model()->get_path(children_iter), *(get_column(0)));
      
      return true;
    }
  }
  
  return false;
}

void Dingo::TrackTreeView::activateCurrentPlaylist() {
  Gtk::TreeModel::iterator iter = get_model()->children().begin();

  if (iter) {
    Gtk::TreeModel::Path tree_path = get_model()->get_path(iter);
  
    Gtk::TreeView::row_activated(tree_path, *(get_column(0)));
  }
  
  else {
    if ((d_playerengine->getStatus() == Dingo::ENGINE_PLAYING) || (d_playerengine->getStatus() == Dingo::ENGINE_PAUSED)) {
      d_playerengine->stop();
      
      notifyTrackTreeViewObserver(Dingo::TRACK_TREE_ROW_DEACTIVATED);
    }
  }
}

std::vector<Gtk::TreeRowReference> Dingo::TrackTreeView::getSelectionRefs() {
  std::vector<Gtk::TreeModel::Path> selected_path_vector = get_selection()->get_selected_rows();
  
  std::vector<Gtk::TreeRowReference> temp_ref_vector;
  
  for (std::vector<Gtk::TreeModel::Path>::iterator path_iter = selected_path_vector.begin(); path_iter != selected_path_vector.end(); ++path_iter) {
    Gtk::TreeRowReference temp_ref(Dingo::DBManager::trackModel, d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(*path_iter)));
    
    temp_ref_vector.push_back(temp_ref);
  }
  
  return temp_ref_vector;
}

//UTILITIES
void Dingo::TrackTreeView::saveNextTrackRowRef() {
  Gtk::TreeModel::iterator temp_edit_iter = get_model()->get_iter(d_track_sort->convert_child_path_to_path(d_track_filter->convert_child_path_to_path(cur_edit_row_ref.get_path())));
  
  ++temp_edit_iter;
  
  if (!temp_edit_iter) {
    Gtk::TreeModel::Children children = get_model()->children();
    
    temp_edit_iter = children.begin();
  }
  
  //save the newly edit row to cur_next_edit_row_ref
  Gtk::TreeRowReference temp_next_edit_row_ref(Dingo::DBManager::trackModel, d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(get_model()->get_path(temp_edit_iter))));
  
  cur_next_edit_row_ref = temp_next_edit_row_ref;
}

void Dingo::TrackTreeView::savePreviousTrackRowRef() {
  Gtk::TreeModel::iterator temp_edit_iter = get_model()->get_iter(d_track_sort->convert_child_path_to_path(d_track_filter->convert_child_path_to_path(cur_edit_row_ref.get_path())));
  
  --temp_edit_iter;
  
  if (!temp_edit_iter) {
    Gtk::TreeModel::Children children = get_model()->children();
    
    temp_edit_iter = children.begin();
  }
  
  //save the newly edit row to cur_edit_row_ref
  Gtk::TreeRowReference temp_prev_edit_row_ref(Dingo::DBManager::trackModel, d_track_filter->convert_path_to_child_path(d_track_sort->convert_path_to_child_path(get_model()->get_path(temp_edit_iter))));
  
  cur_prev_edit_row_ref = temp_prev_edit_row_ref;
}

void Dingo::TrackTreeView::highlightRow(Gtk::TreeModel::iterator iter) {
  Gtk::TreeModel::Row temp_row = *iter;
  
  temp_row[(*Dingo::DBManager::trackCR).trackStatus] = d_playing_image.get_pixbuf();
  
  temp_row[(*Dingo::DBManager::trackCR).trackNumber] = Dingo::Utilities::highlightString(temp_row[(*Dingo::DBManager::trackCR).trackNumber]);
  temp_row[(*Dingo::DBManager::trackCR).trackTitle] = Dingo::Utilities::highlightString(temp_row[(*Dingo::DBManager::trackCR).trackTitle]);
  temp_row[(*Dingo::DBManager::trackCR).trackGenre] = Dingo::Utilities::highlightString(temp_row[(*Dingo::DBManager::trackCR).trackGenre]);
  temp_row[(*Dingo::DBManager::trackCR).trackArtist] = Dingo::Utilities::highlightString(temp_row[(*Dingo::DBManager::trackCR).trackArtist]);
  temp_row[(*Dingo::DBManager::trackCR).trackAlbum] = Dingo::Utilities::highlightString(temp_row[(*Dingo::DBManager::trackCR).trackAlbum]);
  temp_row[(*Dingo::DBManager::trackCR).trackYear] = Dingo::Utilities::highlightString(temp_row[(*Dingo::DBManager::trackCR).trackYear]);
  temp_row[(*Dingo::DBManager::trackCR).trackTime] = Dingo::Utilities::highlightString(temp_row[(*Dingo::DBManager::trackCR).trackTime]);
  temp_row[(*Dingo::DBManager::trackCR).trackFileExtension] = Dingo::Utilities::highlightString(temp_row[(*Dingo::DBManager::trackCR).trackFileExtension]);
  temp_row[(*Dingo::DBManager::trackCR).trackBitrate] = Dingo::Utilities::highlightString(temp_row[(*Dingo::DBManager::trackCR).trackBitrate]);
}

void Dingo::TrackTreeView::unhighlightRow(Gtk::TreeModel::iterator iter) {
  Gtk::TreeModel::Row temp_row = *iter;
    
  Glib::RefPtr<Gdk::Pixbuf> temp_empty_trackStatus;
    
  temp_row[(*Dingo::DBManager::trackCR).trackStatus] = temp_empty_trackStatus;
  
  temp_row[(*Dingo::DBManager::trackCR).trackNumber] = Dingo::Utilities::unhighlightString(temp_row[(*Dingo::DBManager::trackCR).trackNumber]);
  temp_row[(*Dingo::DBManager::trackCR).trackTitle] = Dingo::Utilities::unhighlightString(temp_row[(*Dingo::DBManager::trackCR).trackTitle]);
  temp_row[(*Dingo::DBManager::trackCR).trackGenre] = Dingo::Utilities::unhighlightString(temp_row[(*Dingo::DBManager::trackCR).trackGenre]);
  temp_row[(*Dingo::DBManager::trackCR).trackArtist] = Dingo::Utilities::unhighlightString(temp_row[(*Dingo::DBManager::trackCR).trackArtist]);
  temp_row[(*Dingo::DBManager::trackCR).trackAlbum] = Dingo::Utilities::unhighlightString(temp_row[(*Dingo::DBManager::trackCR).trackAlbum]);
  temp_row[(*Dingo::DBManager::trackCR).trackYear] = Dingo::Utilities::unhighlightString(temp_row[(*Dingo::DBManager::trackCR).trackYear]);
  temp_row[(*Dingo::DBManager::trackCR).trackTime] = Dingo::Utilities::unhighlightString(temp_row[(*Dingo::DBManager::trackCR).trackTime]);
  temp_row[(*Dingo::DBManager::trackCR).trackFileExtension] = Dingo::Utilities::unhighlightString(temp_row[(*Dingo::DBManager::trackCR).trackFileExtension]);
  temp_row[(*Dingo::DBManager::trackCR).trackBitrate] = Dingo::Utilities::unhighlightString(temp_row[(*Dingo::DBManager::trackCR).trackBitrate]);
}

void Dingo::TrackTreeView::refreshData() {
  if (d_settings->getVisible("trackbrowser")) {
    int temp_sort_column_id = 3;
    Gtk::SortType temp_sort_type = Gtk::SORT_ASCENDING;
    
    filterTracks();
  
    d_track_sort->get_sort_column_id(temp_sort_column_id, temp_sort_type);
    
    d_track_sort->set_sort_column(temp_sort_column_id, temp_sort_type);
  
    d_statusbar->push(d_dbmanager->getPlaylistInfo());
  }
}
