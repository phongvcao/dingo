#include "dingo-artisttreeview.h"
#include "dingo-statusbar.h"
#include "dingo-artisttreeviewobserver.h"
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
Dingo::ArtistTreeView::ArtistTreeView() {

}

Dingo::ArtistTreeView::ArtistTreeView(Dingo::DBManager* dbmanager, Dingo::Settings* settings, Dingo::StatusBar* statusbar) :
  Gtk::TreeView()
{
  d_dbmanager = dbmanager;
  d_settings = settings;
  d_statusbar = statusbar;
  
  d_visible_artists_count = 0;
  
  createArtistTreeView();
}

Dingo::ArtistTreeView::~ArtistTreeView() {

}
      
//OBSERVER'S METHODS
void Dingo::ArtistTreeView::registerArtistTreeViewObserver(Dingo::ArtistTreeViewObserver* observer) {
  artist_obs_vector.push_back(observer);
}

void Dingo::ArtistTreeView::removeArtistTreeViewObserver(Dingo::ArtistTreeViewObserver* observer) {
  std::vector<Dingo::ArtistTreeViewObserver*>::iterator iter = std::find(artist_obs_vector.begin(), artist_obs_vector.end(), observer);
  
  if (iter != artist_obs_vector.end()) {
    artist_obs_vector.erase(iter);
  }
}

void Dingo::ArtistTreeView::notifyArtistTreeViewObserver(Dingo::TreeViewSignals signal_type) {
  switch (signal_type) {
    case Dingo::ARTISTS_SELECTED: {
      for (std::vector<Dingo::ArtistTreeViewObserver*>::iterator iter = artist_obs_vector.begin(); iter != artist_obs_vector.end(); ++iter) {
        (*iter)->updateArtistsSelected();
      }
      //std::cout << "Artists Selected!" << std::endl;
      break;
    }
    
    default: {
      std::cerr << "Undefined Dingo::TreeViewSignals!" << std::endl;
      break;
    }
  }
}

//INHERITED OBSERVER'S METHODS
void Dingo::ArtistTreeView::updateHomeToggleActive() {
  refreshData();
}

void Dingo::ArtistTreeView::updateNowPlayingToggleActive() {
  refreshData();
}

void Dingo::ArtistTreeView::updatePlayQueueToggleActive() {
  refreshData();
}

void Dingo::ArtistTreeView::updateSearchEntryChanged() {
  refreshData();
}

void Dingo::ArtistTreeView::updateAudioFilterToggled() {
  refreshData();
}

void Dingo::ArtistTreeView::updateVideoFilterToggled() {
  refreshData();
}

void Dingo::ArtistTreeView::updateBlockSelection() {
  d_selected_store_map[d_dbmanager->getPreviousPlaylistID()] = getSelectionRefs();
  
  d_dbmanager->setCurrentArtistsID(d_selected_store_map[d_dbmanager->getCurrentPlaylistID()]);
  
  artists_select_connection.block();
}

void Dingo::ArtistTreeView::updateRestoreSelectedRows() {
  Gtk::TreeModel::Children children = Dingo::DBManager::artistModel->children();
  
  for (Gtk::TreeModel::Children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    
    if (row[(*Dingo::DBManager::artistCR).artistID]) {
      row[(*Dingo::DBManager::artistCR).artistVisible] = true;
    }
  }

  get_selection()->unselect_all();
  
  bool is_any_row_selected = false;
  
  std::vector<Gtk::TreeRowReference> refs_vector = d_selected_store_map[d_dbmanager->getCurrentPlaylistID()];
  
  for (std::vector<Gtk::TreeRowReference>::iterator ref_iter = refs_vector.begin(); ref_iter != refs_vector.end(); ++ref_iter) {
    get_selection()->select(d_artist_filter->convert_child_path_to_path(d_artist_sort->convert_child_path_to_path(d_artist_modify_filter->convert_child_path_to_path(ref_iter->get_path()))));
    
    is_any_row_selected = true;
  }
  
  if (!is_any_row_selected) {
    get_selection()->select(get_model()->children().begin());
  }
  
  refreshData();
}

void Dingo::ArtistTreeView::updateUnblockSelection() {
  artists_select_connection.unblock();
}

void Dingo::ArtistTreeView::updateGenresSelected() {
  refreshData();
}

void Dingo::ArtistTreeView::updateChangesInCoreArtists() {
  refreshData();
}

void Dingo::ArtistTreeView::updateTrackPlaylistEditInfoSaved() {
  refreshData();
}
    
//CREATORS
void Dingo::ArtistTreeView::createArtistTreeView() {
  d_artist_filter = Gtk::TreeModelFilter::create(Dingo::DBManager::artistModel);
  d_artist_filter->set_visible_column((*Dingo::DBManager::artistCR).artistVisible);
  
  filterArtists();
  d_visible_artists_count = d_artist_filter->children().size() - 1;
  
  d_artist_sort = Gtk::TreeModelSort::create(d_artist_filter);
  d_artist_sort->set_sort_func((*Dingo::DBManager::artistCR).artistName, sigc::mem_fun(*this, &Dingo::ArtistTreeView::onArtistNameCompare));
  d_artist_sort->set_sort_column((*Dingo::DBManager::artistCR).artistName, Gtk::SORT_ASCENDING);
  
  d_artist_modify_filter = Gtk::TreeModelFilter::create(d_artist_sort);
  d_artist_modify_filter->set_modify_func(*Dingo::DBManager::artistCR, sigc::mem_fun(*this, &Dingo::ArtistTreeView::onArtistRowModify));
  
  set_model(d_artist_modify_filter);
  set_rules_hint(true);
  
  Gtk::TreeView::Column* p_column = Gtk::manage(new Gtk::TreeView::Column(_("Artist")));
  Gtk::CellRendererPixbuf* p_renderer_artisticon = Gtk::manage(new Gtk::CellRendererPixbuf());
  Gtk::CellRendererText* p_renderer_artistname = Gtk::manage(new Gtk::CellRendererText());
  
  p_column->pack_start(*p_renderer_artisticon, false);
  p_column->pack_start(*p_renderer_artistname);
  
  append_column(*p_column);
  
  p_column->add_attribute(p_renderer_artistname->property_markup(), (*Dingo::DBManager::artistCR).artistName);
  p_column->add_attribute(p_renderer_artisticon->property_pixbuf(), (*Dingo::DBManager::artistCR).artistIcon);
  p_column->set_expand(true);
  p_column->set_resizable(true);
  
  get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
  artists_select_connection = get_selection()->signal_changed().connect(sigc::mem_fun(*this, &Dingo::ArtistTreeView::onSelectionChanged));
  
  get_selection()->select(get_model()->children().begin());
}

//UTILITIES
void Dingo::ArtistTreeView::refreshData() {
  artists_select_connection.block();

  if (d_settings->getVisible("artistfilter")) {
    filterArtists();
  
    d_visible_artists_count = get_model()->children().size() - 1;
  
    d_artist_sort->set_sort_column((*Dingo::DBManager::artistCR).artistName, Gtk::SORT_ASCENDING);
  
    d_artist_modify_filter->refilter();
  }
  
  artists_select_connection.unblock();
}

//SIGNAL HANDLERS
void Dingo::ArtistTreeView::onSelectionChanged() {
  d_dbmanager->setCurrentArtistsID(getSelectionRefs());
  
  notifyArtistTreeViewObserver(Dingo::ARTISTS_SELECTED);
}

void Dingo::ArtistTreeView::filterArtists() {
  d_dbmanager->prepareQueryCountTracksInArtist();
  
  Gtk::TreeModel::Children children = Dingo::DBManager::artistModel->children();
  
  for (Gtk::TreeModel::Children::iterator iter = ++children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    
    row[(*Dingo::DBManager::artistCR).artistTracksCount] = d_dbmanager->getCountTracksInArtist(row[(*Dingo::DBManager::artistCR).artistID]);
    row[(*Dingo::DBManager::artistCR).artistVisible] = row[(*Dingo::DBManager::artistCR).artistTracksCount];
  }
  
  d_dbmanager->finalizeQueryCountTracksInArtist();
}

void Dingo::ArtistTreeView::onArtistRowModify(const Gtk::TreeModel::iterator& iter, Glib::ValueBase& value, int column_id) {
  Gtk::TreeModel::Row temp_row = *(d_artist_filter->convert_iter_to_child_iter(d_artist_sort->convert_iter_to_child_iter(d_artist_modify_filter->convert_iter_to_child_iter(iter))));
  
  switch (column_id) {
    case (0): {
      Glib::Value<Glib::RefPtr<Gdk::Pixbuf> > valPixbuf;
      valPixbuf.init(Glib::Value<Glib::RefPtr<Gdk::Pixbuf> >::value_type());
        
      Glib::RefPtr<Gdk::Pixbuf> temp_cur_pixbuf = temp_row[(*Dingo::DBManager::artistCR).artistIcon];
        
      valPixbuf.set(temp_cur_pixbuf);
      value = valPixbuf;
      
      break;
    }
      
    case (1): {
      Glib::Value<int> valInt;
      valInt.init(Glib::Value<int>::value_type());
        
      int temp_artist_id = temp_row[(*Dingo::DBManager::artistCR).artistID];
        
      valInt.set(temp_artist_id);
      value = valInt;
      
      break;
    }
      
    case (2): {
      Glib::Value<Glib::ustring> valStr;
      valStr.init(Glib::Value<Glib::ustring>::value_type());
      Glib::ustring temp_cur_artist_name = "";
        
      if (temp_row[(*Dingo::DBManager::artistCR).artistID] == 0) {
        temp_cur_artist_name = Glib::ustring::compose(Dingo::Utilities::highlightString(_("All %1 Artists")), d_visible_artists_count);
      }
        
      else {
        temp_cur_artist_name = temp_row[(*Dingo::DBManager::artistCR).artistName] + " (" + Dingo::Utilities::intToString(temp_row[(*Dingo::DBManager::artistCR).artistTracksCount]) + ")";
      }
      
      valStr.set(temp_cur_artist_name);
      value = valStr;
      
      break;
    }
      
    default: {
      break;
    }
  }
}

int Dingo::ArtistTreeView::onArtistNameCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter) {
  Gtk::TreeModel::Row temp_first_row = *first_iter;
  Gtk::TreeModel::Row temp_second_row = *second_iter;
  
  if (temp_first_row[(*Dingo::DBManager::artistCR).artistID] == 0) {
    return -1;
  }
  
  else if (temp_second_row[(*Dingo::DBManager::artistCR).artistID] == 0) {
    return 1;
  }
  
  Glib::ustring first_title_str = Dingo::Utilities::unhighlightString(temp_first_row[(*Dingo::DBManager::artistCR).artistName]);
  
  Glib::ustring second_title_str = Dingo::Utilities::unhighlightString(temp_second_row[(*Dingo::DBManager::artistCR).artistName]);
  
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
std::vector<Gtk::TreeRowReference> Dingo::ArtistTreeView::getSelectionRefs() {
  std::vector<Gtk::TreeModel::Path> temp_path_vector = get_selection()->get_selected_rows();
  
  std::vector<Gtk::TreeRowReference> temp_ref_vector;
  
  for (std::vector<Gtk::TreeModel::Path>::iterator path_iter = temp_path_vector.begin(); path_iter != temp_path_vector.end(); ++path_iter) {
    Gtk::TreeRowReference temp_ref(Dingo::DBManager::artistModel, d_artist_filter->convert_path_to_child_path(d_artist_sort->convert_path_to_child_path(d_artist_modify_filter->convert_path_to_child_path(*path_iter))));
    
    Gtk::TreeModel::Row temp_row = *(Dingo::DBManager::artistModel->get_iter(d_artist_filter->convert_path_to_child_path(d_artist_sort->convert_path_to_child_path(d_artist_modify_filter->convert_path_to_child_path(*path_iter)))));
    
    if (temp_row[(*Dingo::DBManager::artistCR).artistID] == 0) {
      temp_ref_vector.clear();
      break;
    }
  
    temp_ref_vector.push_back(temp_ref);
  }
  
  return temp_ref_vector;
}
