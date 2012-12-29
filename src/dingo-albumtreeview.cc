/* 
 * dingo-albumtreeview.cc
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

#include "dingo-albumtreeview.h"
#include "dingo-statusbar.h"
#include "dingo-albumtreeviewobserver.h"
#include "dingo-dbmanager.h"
#include "dingo-settings.h"
#include "dingo-utilities.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

#ifndef INCLUDED_GLIBMM_I18N_H
#define INCLUDED_GLIBMM_I18N_H
#include <glibmm/i18n.h>
#endif //INCLUDED_GLIBMM_I18N_H

//CONSTRUCTORS - DESTRUCTOR
Dingo::AlbumTreeView::AlbumTreeView() {

}

Dingo::AlbumTreeView::AlbumTreeView(Dingo::DBManager* dbmanager, Dingo::Settings* settings, Dingo::StatusBar* statusbar) :
  Gtk::TreeView()
{
  d_dbmanager = dbmanager;
  d_settings = settings;
  d_statusbar = statusbar;
  
  d_visible_albums_count = 0;
  
  createAlbumTreeView();
}

Dingo::AlbumTreeView::~AlbumTreeView() {

}
      
//OBSERVER'S METHODS
void Dingo::AlbumTreeView::registerAlbumTreeViewObserver(Dingo::AlbumTreeViewObserver* observer) {
  album_obs_vector.push_back(observer);
}

void Dingo::AlbumTreeView::removeAlbumTreeViewObserver(Dingo::AlbumTreeViewObserver* observer) {
  std::vector<Dingo::AlbumTreeViewObserver*>::iterator iter = std::find(album_obs_vector.begin(), album_obs_vector.end(), observer);
  
  if (iter != album_obs_vector.end()) {
    album_obs_vector.erase(iter);
  }
}

void Dingo::AlbumTreeView::notifyAlbumTreeViewObserver(Dingo::TreeViewSignals signal_type) {
  switch (signal_type) {
    case Dingo::ALBUMS_SELECTED: {
      for (std::vector<Dingo::AlbumTreeViewObserver*>::iterator iter = album_obs_vector.begin(); iter != album_obs_vector.end(); ++iter) {
        (*iter)->updateAlbumsSelected();
      }
      //std::cout << "Albums Selected!" << std::endl;
      break;
    }
    
    default: {
      std::cerr << "Undefined Dingo::TreeViewSignals!" << std::endl;
      break;
    }
  }
}

//INHERITED OBSERVER'S METHODS
void Dingo::AlbumTreeView::updateHomeToggleActive() {
  refreshData();
}

void Dingo::AlbumTreeView::updateNowPlayingToggleActive() {
  refreshData();
}

void Dingo::AlbumTreeView::updatePlayQueueToggleActive() {
  refreshData();
}

void Dingo::AlbumTreeView::updateSearchEntryChanged() {
  refreshData();
}

void Dingo::AlbumTreeView::updateAudioFilterToggled() {
  refreshData();
}

void Dingo::AlbumTreeView::updateVideoFilterToggled() {
  refreshData();
}

void Dingo::AlbumTreeView::updateBlockSelection() {
  d_selected_store_map[d_dbmanager->getPreviousPlaylistID()] = getSelectionRefs();
  
  d_dbmanager->setCurrentAlbumsID(d_selected_store_map[d_dbmanager->getCurrentPlaylistID()]);
  
  albums_select_connection.block();
}

void Dingo::AlbumTreeView::updateRestoreSelectedRows() {
  Gtk::TreeModel::Children children = Dingo::DBManager::albumModel->children();
  
  for (Gtk::TreeModel::Children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    
    if (row[(*Dingo::DBManager::albumCR).albumID]) {
      row[(*Dingo::DBManager::albumCR).albumVisible] = true;
    }
  }

  get_selection()->unselect_all();
  
  bool is_any_row_selected = false;
  
  std::vector<Gtk::TreeRowReference> refs_vector = d_selected_store_map[d_dbmanager->getCurrentPlaylistID()];
  
  for (std::vector<Gtk::TreeRowReference>::iterator ref_iter = refs_vector.begin(); ref_iter != refs_vector.end(); ++ref_iter) {
    get_selection()->select(d_album_filter->convert_child_path_to_path(d_album_sort->convert_child_path_to_path(d_album_modify_filter->convert_child_path_to_path(ref_iter->get_path()))));
    
    is_any_row_selected = true;
  }
  
  if (!is_any_row_selected) {
    get_selection()->select(get_model()->children().begin());
  }
  
  refreshData();
}

void Dingo::AlbumTreeView::updateUnblockSelection() {
  albums_select_connection.unblock();
}

void Dingo::AlbumTreeView::updateGenresSelected() {
  refreshData();
}

void Dingo::AlbumTreeView::updateArtistsSelected() {
  refreshData();
}

void Dingo::AlbumTreeView::updateYearsSelected() {
  refreshData();
}

void Dingo::AlbumTreeView::updateChangesInCoreAlbums() {
  refreshData();
}

void Dingo::AlbumTreeView::updateTrackPlaylistEditInfoSaved() {
  refreshData();
}
    
//CREATORS
void Dingo::AlbumTreeView::createAlbumTreeView() {
  d_album_filter = Gtk::TreeModelFilter::create(Dingo::DBManager::albumModel);
  d_album_filter->set_visible_column((*Dingo::DBManager::albumCR).albumVisible);
  
  filterAlbums();  
  d_visible_albums_count = d_album_filter->children().size() - 1;
  
  d_album_sort = Gtk::TreeModelSort::create(d_album_filter);
  d_album_sort->set_sort_func((*Dingo::DBManager::albumCR).albumName, sigc::mem_fun(*this, &Dingo::AlbumTreeView::onAlbumNameCompare));
  d_album_sort->set_sort_column((*Dingo::DBManager::albumCR).albumName, Gtk::SORT_ASCENDING);
  
  d_album_modify_filter = Gtk::TreeModelFilter::create(d_album_sort);
  d_album_modify_filter->set_modify_func(*Dingo::DBManager::albumCR, sigc::mem_fun(*this, &Dingo::AlbumTreeView::onAlbumRowModify));
  
  set_model(d_album_modify_filter);
  set_rules_hint(true);
  
  Gtk::TreeView::Column* p_column = Gtk::manage(new Gtk::TreeView::Column(_("Album")));
  Gtk::CellRendererPixbuf* p_renderer_albumicon = Gtk::manage(new Gtk::CellRendererPixbuf());
  Gtk::CellRendererText* p_renderer_albumname = Gtk::manage(new Gtk::CellRendererText());
  
  p_column->pack_start(*p_renderer_albumicon, false);
  p_column->pack_start(*p_renderer_albumname);
  
  append_column(*p_column);
  
  p_column->add_attribute(p_renderer_albumname->property_markup(), (*Dingo::DBManager::albumCR).albumName);
  p_column->add_attribute(p_renderer_albumicon->property_pixbuf(), (*Dingo::DBManager::albumCR).albumIcon);
  p_column->set_expand(true);
  p_column->set_resizable(true);
  
  get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
  albums_select_connection = get_selection()->signal_changed().connect(sigc::mem_fun(*this, &Dingo::AlbumTreeView::onSelectionChanged));
  
  get_selection()->select(get_model()->children().begin());
}

//UTILITIES
void Dingo::AlbumTreeView::refreshData() {
  //if (!d_is_restoring_playlist_selection) {
    albums_select_connection.block();

    if (d_settings->getVisible("albumfilter")) {
      //d_album_filter->refilter();
      filterAlbums();
    
      d_visible_albums_count = get_model()->children().size() - 1;
  
      d_album_sort->set_sort_column((*Dingo::DBManager::albumCR).albumName, Gtk::SORT_ASCENDING);
  
      d_album_modify_filter->refilter();
    }
  
    albums_select_connection.unblock();
  //}
}
      
//SIGNAL HANDLERS
void Dingo::AlbumTreeView::onSelectionChanged() {
  d_dbmanager->setCurrentAlbumsID(getSelectionRefs());
  
  notifyAlbumTreeViewObserver(Dingo::ALBUMS_SELECTED);
}

void Dingo::AlbumTreeView::filterAlbums() {
  d_dbmanager->prepareQueryCountTracksInAlbum();
  
  Gtk::TreeModel::Children children = Dingo::DBManager::albumModel->children();
  
  //the first row of all treeviews should be visible
  for (Gtk::TreeModel::Children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    
    if (row[(*Dingo::DBManager::albumCR).albumID]) {
      row[(*Dingo::DBManager::albumCR).albumTracksCount] = d_dbmanager->getCountTracksInAlbum(row[(*Dingo::DBManager::albumCR).albumID]);
      row[(*Dingo::DBManager::albumCR).albumVisible] = row[(*Dingo::DBManager::albumCR).albumTracksCount];
    }
  }
  
  d_dbmanager->finalizeQueryCountTracksInAlbum();
}

void Dingo::AlbumTreeView::onAlbumRowModify(const Gtk::TreeModel::iterator& iter, Glib::ValueBase& value, int column_id) {
  Gtk::TreeModel::Row temp_row = *(d_album_filter->convert_iter_to_child_iter(d_album_sort->convert_iter_to_child_iter(d_album_modify_filter->convert_iter_to_child_iter(iter))));
  
  switch (column_id) {
    case (0): {
      Glib::Value<Glib::RefPtr<Gdk::Pixbuf> > valPixbuf;
      valPixbuf.init(Glib::Value<Glib::RefPtr<Gdk::Pixbuf> >::value_type());
        
      Glib::RefPtr<Gdk::Pixbuf> temp_cur_pixbuf = temp_row[(*Dingo::DBManager::albumCR).albumIcon];
        
      valPixbuf.set(temp_cur_pixbuf);
      value = valPixbuf;
      
      break;
    }
      
    case (1): {
      Glib::Value<int> valInt;
      valInt.init(Glib::Value<int>::value_type());
        
      int temp_album_id = temp_row[(*Dingo::DBManager::albumCR).albumID];
        
      valInt.set(temp_album_id);
      value = valInt;
      
      break;
    }
      
    case (2): {
      Glib::Value<Glib::ustring> valStr;
      valStr.init(Glib::Value<Glib::ustring>::value_type());
      Glib::ustring temp_cur_album_name = "";
        
      if (temp_row[(*Dingo::DBManager::albumCR).albumID] == 0) {
        temp_cur_album_name = Glib::ustring::compose(Dingo::Utilities::highlightString(_("All %1 Albums")), d_visible_albums_count);
      }
        
      else {
        temp_cur_album_name = temp_row[(*Dingo::DBManager::albumCR).albumName] + " (" + Dingo::Utilities::intToString(temp_row[(*Dingo::DBManager::albumCR).albumTracksCount]) + ")";
      }
      
      valStr.set(temp_cur_album_name);
      value = valStr;
      
      break;
    }
      
    default: {
      break;
    }
  }
}

int Dingo::AlbumTreeView::onAlbumNameCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter) {
  Gtk::TreeModel::Row temp_first_row = *first_iter;
  Gtk::TreeModel::Row temp_second_row = *second_iter;
  
  if (temp_first_row[(*Dingo::DBManager::albumCR).albumID] == 0) {
    return -1;
  }
  
  else if (temp_second_row[(*Dingo::DBManager::albumCR).albumID] == 0) {
    return 1;
  }
  
  Glib::ustring first_title_str = Dingo::Utilities::unhighlightString(temp_first_row[(*Dingo::DBManager::albumCR).albumName]);
  
  Glib::ustring second_title_str = Dingo::Utilities::unhighlightString(temp_second_row[(*Dingo::DBManager::albumCR).albumName]);
  
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

//UTILITIES
std::vector<Gtk::TreeRowReference> Dingo::AlbumTreeView::getSelectionRefs() {
  std::vector<Gtk::TreeModel::Path> temp_path_vector = get_selection()->get_selected_rows();
  
  std::vector<Gtk::TreeRowReference> temp_ref_vector;
  
  for (std::vector<Gtk::TreeModel::Path>::iterator path_iter = temp_path_vector.begin(); path_iter != temp_path_vector.end(); ++path_iter) {
    Gtk::TreeRowReference temp_ref(Dingo::DBManager::albumModel, d_album_filter->convert_path_to_child_path(d_album_sort->convert_path_to_child_path(d_album_modify_filter->convert_path_to_child_path(*path_iter))));
    
    Gtk::TreeModel::Row temp_row = *(Dingo::DBManager::albumModel->get_iter(d_album_filter->convert_path_to_child_path(d_album_sort->convert_path_to_child_path(d_album_modify_filter->convert_path_to_child_path(*path_iter)))));
    
    if (temp_row[(*Dingo::DBManager::albumCR).albumID] == 0) {
      temp_ref_vector.clear();
      break;
    }
    
    temp_ref_vector.push_back(temp_ref);
  }
  
  return temp_ref_vector;
}
