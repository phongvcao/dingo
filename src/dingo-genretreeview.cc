/* 
 * dingo-genretreeview.cc
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

#include "dingo-genretreeview.h"
#include "dingo-genretreeviewobserver.h"
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
Dingo::GenreTreeView::GenreTreeView() {

}

Dingo::GenreTreeView::GenreTreeView(Dingo::DBManager* dbmanager, Dingo::Settings* settings) :
  Gtk::TreeView()
{
  d_dbmanager = dbmanager;
  d_settings = settings;
  
  d_visible_genres_count = 0;
  
  createGenreTreeView();
}

Dingo::GenreTreeView::~GenreTreeView() {

}
      
//OBSERVER'S METHODS
void Dingo::GenreTreeView::registerGenreTreeViewObserver(Dingo::GenreTreeViewObserver* observer) {
  genre_obs_vector.push_back(observer);
}

void Dingo::GenreTreeView::removeGenreTreeViewObserver(Dingo::GenreTreeViewObserver* observer) {
  std::vector<Dingo::GenreTreeViewObserver*>::iterator iter = std::find(genre_obs_vector.begin(), genre_obs_vector.end(), observer);
  
  if (iter != genre_obs_vector.end()) {
    genre_obs_vector.erase(iter);
  }
}

void Dingo::GenreTreeView::notifyGenreTreeViewObserver(Dingo::TreeViewSignals signal_type) {
  switch (signal_type) {
    case Dingo::GENRES_SELECTED: {
      for (std::vector<Dingo::GenreTreeViewObserver*>::iterator iter = genre_obs_vector.begin(); iter != genre_obs_vector.end(); ++iter) {
        (*iter)->updateGenresSelected();
      }
      //std::cout << "Genres Selected!" << std::endl;
      break;
    }
    
    default: {
      //std::cerr << "Undefined Dingo::TreeViewSignals!" << std::endl;
      break;
    }
  }
}
      
//INHERITED OBSERVER'S METHODS
void Dingo::GenreTreeView::updateHomeToggleActive() {
  refreshData();
}

void Dingo::GenreTreeView::updateNowPlayingToggleActive() {
  refreshData();
}

void Dingo::GenreTreeView::updatePlayQueueToggleActive() {
  refreshData();
}  
      
void Dingo::GenreTreeView::updateSearchEntryChanged() {
  refreshData();
}

void Dingo::GenreTreeView::updateAudioFilterToggled() {
  refreshData();
}

void Dingo::GenreTreeView::updateVideoFilterToggled() {
  refreshData();
}

void Dingo::GenreTreeView::updateBlockSelection() {
  d_selected_store_map[d_dbmanager->getPreviousPlaylistID()] = getSelectionRefs();
  
  d_dbmanager->setCurrentGenresID(d_selected_store_map[d_dbmanager->getCurrentPlaylistID()]);
  
  genres_select_connection.block();
}

void Dingo::GenreTreeView::updateRestoreSelectedRows() {
  Gtk::TreeModel::Children children = Dingo::DBManager::genreModel->children();
  
  for (Gtk::TreeModel::Children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    
    if (row[(*Dingo::DBManager::genreCR).genreID]) {
      row[(*Dingo::DBManager::genreCR).genreVisible] = true;
    }
  }

  get_selection()->unselect_all();
  
  bool is_any_row_selected = false;
  
  std::vector<Gtk::TreeRowReference> refs_vector = d_selected_store_map[d_dbmanager->getCurrentPlaylistID()];
  
  for (std::vector<Gtk::TreeRowReference>::iterator ref_iter = refs_vector.begin(); ref_iter != refs_vector.end(); ++ref_iter) {
    get_selection()->select(d_genre_filter->convert_child_path_to_path(d_genre_sort->convert_child_path_to_path(d_genre_modify_filter->convert_child_path_to_path(ref_iter->get_path()))));
    
    is_any_row_selected = true;
  }
  
  if (!is_any_row_selected) {
    get_selection()->select(get_model()->children().begin());
  }
  
  refreshData();
}

void Dingo::GenreTreeView::updateUnblockSelection() {
  genres_select_connection.unblock();
}
      
void Dingo::GenreTreeView::updateChangesInCoreGenres() {
  refreshData();
}

void Dingo::GenreTreeView::updateTrackPlaylistEditInfoSaved() {
  refreshData();
}
    
//CREATORS
void Dingo::GenreTreeView::createGenreTreeView() {
  d_genre_filter = Gtk::TreeModelFilter::create(Dingo::DBManager::genreModel);
  d_genre_filter->set_visible_column((*Dingo::DBManager::genreCR).genreVisible);
  
  filterGenres();
  d_visible_genres_count = d_genre_filter->children().size() - 1;
  
  d_genre_sort = Gtk::TreeModelSort::create(d_genre_filter);
  d_genre_sort->set_sort_func((*Dingo::DBManager::genreCR).genreName, sigc::mem_fun(*this, &Dingo::GenreTreeView::onGenreNameCompare));
  d_genre_sort->set_sort_column((*Dingo::DBManager::genreCR).genreName, Gtk::SORT_ASCENDING);
  
  d_genre_modify_filter = Gtk::TreeModelFilter::create(d_genre_sort);
  d_genre_modify_filter->set_modify_func(*Dingo::DBManager::genreCR, sigc::mem_fun(*this, &Dingo::GenreTreeView::onGenreRowModify));
  
  set_model(d_genre_modify_filter);
  set_rules_hint(true);
  
  Gtk::TreeView::Column* p_column = Gtk::manage(new Gtk::TreeView::Column(_("Genre")));
  Gtk::CellRendererPixbuf* p_renderer_genreicon = Gtk::manage(new Gtk::CellRendererPixbuf());
  Gtk::CellRendererText* p_renderer_genrename = Gtk::manage(new Gtk::CellRendererText());
  
  p_column->pack_start(*p_renderer_genreicon, false);
  p_column->pack_start(*p_renderer_genrename);
  
  append_column(*p_column);
  
  p_column->add_attribute(p_renderer_genrename->property_markup(), (*Dingo::DBManager::genreCR).genreName);
  p_column->add_attribute(p_renderer_genreicon->property_pixbuf(), (*Dingo::DBManager::genreCR).genreIcon);
  p_column->set_expand(true);
  p_column->set_resizable(true);
  
  get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
  genres_select_connection = get_selection()->signal_changed().connect(sigc::mem_fun(*this, &Dingo::GenreTreeView::onSelectionChanged));
  
  get_selection()->select(get_model()->children().begin());
}
      
//UTILITIES
void Dingo::GenreTreeView::refreshData() {
  genres_select_connection.block();

  if (d_settings->getVisible("genrefilter")) {
    filterGenres();
  
    d_visible_genres_count = get_model()->children().size() - 1;
  
    d_genre_sort->set_sort_column((*Dingo::DBManager::genreCR).genreName, Gtk::SORT_ASCENDING);
  
    d_genre_modify_filter->refilter();
  }
  
  genres_select_connection.unblock();
}
      
//SIGNAL HANDLERS
void Dingo::GenreTreeView::onSelectionChanged() {
  d_dbmanager->setCurrentGenresID(getSelectionRefs());
  
  notifyGenreTreeViewObserver(Dingo::GENRES_SELECTED);
}

void Dingo::GenreTreeView::filterGenres() {
  d_dbmanager->prepareQueryCountTracksInGenre();
  
  Gtk::TreeModel::Children children = Dingo::DBManager::genreModel->children();
  
  for (Gtk::TreeModel::Children::iterator iter = ++children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    
    row[(*Dingo::DBManager::genreCR).genreTracksCount] = d_dbmanager->getCountTracksInGenre(row[(*Dingo::DBManager::genreCR).genreID]);
    row[(*Dingo::DBManager::genreCR).genreVisible] = row[(*Dingo::DBManager::genreCR).genreTracksCount];
  }
  
  d_dbmanager->finalizeQueryCountTracksInGenre();
}

void Dingo::GenreTreeView::onGenreRowModify(const Gtk::TreeModel::iterator& iter, Glib::ValueBase& value, int column_id) {
  Gtk::TreeModel::Row temp_row = *(d_genre_filter->convert_iter_to_child_iter(d_genre_sort->convert_iter_to_child_iter(d_genre_modify_filter->convert_iter_to_child_iter(iter))));
  
  switch (column_id) {
    case (0): {
      Glib::Value<Glib::RefPtr<Gdk::Pixbuf> > valPixbuf;
      valPixbuf.init(Glib::Value<Glib::RefPtr<Gdk::Pixbuf> >::value_type());
        
      Glib::RefPtr<Gdk::Pixbuf> temp_cur_pixbuf = temp_row[(*Dingo::DBManager::genreCR).genreIcon];
        
      valPixbuf.set(temp_cur_pixbuf);
      value = valPixbuf;
        
      break;
    }
      
    case (1): {
      Glib::Value<int> valInt;
      valInt.init(Glib::Value<int>::value_type());
        
      int temp_genre_id = temp_row[(*Dingo::DBManager::genreCR).genreID];
        
      valInt.set(temp_genre_id);
      value = valInt;
      
      break;
    }
      
    case (2): {
      Glib::Value<Glib::ustring> valStr;
      valStr.init(Glib::Value<Glib::ustring>::value_type());
      Glib::ustring temp_cur_genre_name = "";
        
      if (temp_row[(*Dingo::DBManager::genreCR).genreID] == 0) {
        temp_cur_genre_name = Glib::ustring::compose(Dingo::Utilities::highlightString(_("All %1 Genres")), d_visible_genres_count);
      }
        
      else {
        temp_cur_genre_name = temp_row[(*Dingo::DBManager::genreCR).genreName] + " (" + Dingo::Utilities::intToString(temp_row[(*Dingo::DBManager::genreCR).genreTracksCount]) + ")";
      }
        
      valStr.set(temp_cur_genre_name);
      value = valStr;
      
      break;
    }
      
    default: {
      break;
    }
  }
}

int Dingo::GenreTreeView::onGenreNameCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter) {
  Gtk::TreeModel::Row temp_first_row = *first_iter;
  Gtk::TreeModel::Row temp_second_row = *second_iter;
  
  if (temp_first_row[(*Dingo::DBManager::genreCR).genreID] == 0) {
    return -1;
  }
  
  else if (temp_second_row[(*Dingo::DBManager::genreCR).genreID] == 0) {
    return 1;
  }
  
  Glib::ustring first_title_str = Dingo::Utilities::unhighlightString(temp_first_row[(*Dingo::DBManager::genreCR).genreName]);
  
  Glib::ustring second_title_str = Dingo::Utilities::unhighlightString(temp_second_row[(*Dingo::DBManager::genreCR).genreName]);
  
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
std::vector<Gtk::TreeRowReference> Dingo::GenreTreeView::getSelectionRefs() {
  std::vector<Gtk::TreeModel::Path> temp_path_vector = get_selection()->get_selected_rows();
  
  std::vector<Gtk::TreeRowReference> temp_ref_vector;
  
  for (std::vector<Gtk::TreeModel::Path>::iterator path_iter = temp_path_vector.begin(); path_iter != temp_path_vector.end(); ++path_iter) {
    Gtk::TreeRowReference temp_ref(Dingo::DBManager::genreModel, d_genre_filter->convert_path_to_child_path(d_genre_sort->convert_path_to_child_path(d_genre_modify_filter->convert_path_to_child_path(*path_iter))));
    
    Gtk::TreeModel::Row temp_row = *(Dingo::DBManager::genreModel->get_iter(d_genre_filter->convert_path_to_child_path(d_genre_sort->convert_path_to_child_path(d_genre_modify_filter->convert_path_to_child_path(*path_iter)))));
    
    if (temp_row[(*Dingo::DBManager::genreCR).genreID] == 0) {
      temp_ref_vector.clear();
      break;
    }
    
    temp_ref_vector.push_back(temp_ref);
  }
  
  return temp_ref_vector;
}
