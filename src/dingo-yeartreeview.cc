#include "dingo-yeartreeview.h"
#include "dingo-yeartreeviewobserver.h"
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
Dingo::YearTreeView::YearTreeView() {

}

Dingo::YearTreeView::YearTreeView(Dingo::DBManager* dbmanager, Dingo::Settings* settings) :
  Gtk::TreeView()
{
  d_dbmanager = dbmanager;
  d_settings = settings;
  
  d_visible_years_count = 0;
  
  createYearTreeView();
}

Dingo::YearTreeView::~YearTreeView() {

}
      
//OBSERVER'S METHODS
void Dingo::YearTreeView::registerYearTreeViewObserver(Dingo::YearTreeViewObserver* observer) {
  year_obs_vector.push_back(observer);
}

void Dingo::YearTreeView::removeYearTreeViewObserver(Dingo::YearTreeViewObserver* observer) {
  std::vector<Dingo::YearTreeViewObserver*>::iterator iter = std::find(year_obs_vector.begin(), year_obs_vector.end(), observer);
  
  if (iter != year_obs_vector.end()) {
    year_obs_vector.erase(iter);
  }
}

void Dingo::YearTreeView::notifyYearTreeViewObserver(Dingo::TreeViewSignals signal_type) {
  switch (signal_type) {
    case Dingo::YEARS_SELECTED: {
      for (std::vector<Dingo::YearTreeViewObserver*>::iterator iter = year_obs_vector.begin(); iter != year_obs_vector.end(); ++iter) {
        (*iter)->updateYearsSelected();
      }
      //std::cout << "Years Selected!" << std::endl;
      break;
    }
    
    default: {
      //std::cerr << "Undefined Dingo::TreeViewSignals!" << std::endl;
      break;
    }
  }
}
      
//INHERITED OBSERVER'S METHODS
void Dingo::YearTreeView::updateHomeToggleActive() {
  refreshData();
}

void Dingo::YearTreeView::updateNowPlayingToggleActive() {
  refreshData();
}

void Dingo::YearTreeView::updatePlayQueueToggleActive() {
  refreshData();
}
      
void Dingo::YearTreeView::updateSearchEntryChanged() {
  refreshData();
}

void Dingo::YearTreeView::updateAudioFilterToggled() {
  refreshData();
}

void Dingo::YearTreeView::updateVideoFilterToggled() {
  refreshData();
}

void Dingo::YearTreeView::updateBlockSelection() {
  d_selected_store_map[d_dbmanager->getPreviousPlaylistID()] = getSelectionRefs();
  
  d_dbmanager->setCurrentYearsID(d_selected_store_map[d_dbmanager->getCurrentPlaylistID()]);
  
  years_select_connection.block();
}

void Dingo::YearTreeView::updateRestoreSelectedRows() {
  Gtk::TreeModel::Children children = Dingo::DBManager::yearModel->children();
  
  for (Gtk::TreeModel::Children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    
    if (row[(*Dingo::DBManager::yearCR).yearID]) {
      row[(*Dingo::DBManager::yearCR).yearVisible] = true;
    }
  }

  get_selection()->unselect_all();
  
  bool is_any_row_selected = false;
  
  std::vector<Gtk::TreeRowReference> refs_vector = d_selected_store_map[d_dbmanager->getCurrentPlaylistID()];
  
  for (std::vector<Gtk::TreeRowReference>::iterator ref_iter = refs_vector.begin(); ref_iter != refs_vector.end(); ++ref_iter) {
    get_selection()->select(d_year_filter->convert_child_path_to_path(d_year_sort->convert_child_path_to_path(d_year_modify_filter->convert_child_path_to_path(ref_iter->get_path()))));
    
    is_any_row_selected = true;
  }
  
  if (!is_any_row_selected) {
    get_selection()->select(get_model()->children().begin());
  }
  
  refreshData();
}

void Dingo::YearTreeView::updateUnblockSelection() {
  years_select_connection.unblock();
}

void Dingo::YearTreeView::updateGenresSelected() {
  refreshData();
}

void Dingo::YearTreeView::updateArtistsSelected() {
  refreshData();
}
      
void Dingo::YearTreeView::updateChangesInCoreYears() {
  refreshData();
}

void Dingo::YearTreeView::updateTrackPlaylistEditInfoSaved() {
  refreshData();
}
    
//CREATORS
void Dingo::YearTreeView::createYearTreeView() {
  d_year_filter = Gtk::TreeModelFilter::create(Dingo::DBManager::yearModel);
  d_year_filter->set_visible_column((*Dingo::DBManager::yearCR).yearVisible);
  
  filterYears();
  d_visible_years_count = d_year_filter->children().size() - 1;
  
  d_year_sort = Gtk::TreeModelSort::create(d_year_filter);
  d_year_sort->set_sort_func((*Dingo::DBManager::yearCR).yearName, sigc::mem_fun(*this, &Dingo::YearTreeView::onYearNameCompare));
  d_year_sort->set_sort_column((*Dingo::DBManager::yearCR).yearName, Gtk::SORT_ASCENDING);
  
  d_year_modify_filter = Gtk::TreeModelFilter::create(d_year_sort);
  d_year_modify_filter->set_modify_func(*Dingo::DBManager::yearCR, sigc::mem_fun(*this, &Dingo::YearTreeView::onYearRowModify));
  
  set_model(d_year_modify_filter);
  set_rules_hint(true);
  
  Gtk::TreeView::Column* p_column = Gtk::manage(new Gtk::TreeView::Column(_("Year")));
  Gtk::CellRendererPixbuf* p_renderer_yearicon = Gtk::manage(new Gtk::CellRendererPixbuf());
  Gtk::CellRendererText* p_renderer_yearname = Gtk::manage(new Gtk::CellRendererText());
  
  p_column->pack_start(*p_renderer_yearicon, false);
  p_column->pack_start(*p_renderer_yearname);
  
  append_column(*p_column);
  
  p_column->add_attribute(p_renderer_yearname->property_markup(), (*Dingo::DBManager::yearCR).yearName);
  p_column->add_attribute(p_renderer_yearicon->property_pixbuf(), (*Dingo::DBManager::yearCR).yearIcon);
  p_column->set_expand(true);
  p_column->set_resizable(true);
  
  get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
  years_select_connection = get_selection()->signal_changed().connect(sigc::mem_fun(*this, &Dingo::YearTreeView::onSelectionChanged));
  
  get_selection()->select(get_model()->children().begin());
}
      
//UTILITIES
void Dingo::YearTreeView::refreshData() {
  years_select_connection.block();

  if (d_settings->getVisible("yearfilter")) {
    filterYears();
  
    d_visible_years_count = get_model()->children().size() - 1;
  
    d_year_sort->set_sort_column((*Dingo::DBManager::yearCR).yearName, Gtk::SORT_ASCENDING);
  
    d_year_modify_filter->refilter();
  }
  
  years_select_connection.unblock();
}
      
//SIGNAL HANDLERS
void Dingo::YearTreeView::onSelectionChanged() {
  d_dbmanager->setCurrentYearsID(getSelectionRefs());
  
  notifyYearTreeViewObserver(Dingo::YEARS_SELECTED);
}

void Dingo::YearTreeView::filterYears() {
  d_dbmanager->prepareQueryCountTracksInYear();
  
  Gtk::TreeModel::Children children = Dingo::DBManager::yearModel->children();
  
  for (Gtk::TreeModel::Children::iterator iter = ++children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    
    row[(*Dingo::DBManager::yearCR).yearTracksCount] = d_dbmanager->getCountTracksInYear(row[(*Dingo::DBManager::yearCR).yearID]);
    row[(*Dingo::DBManager::yearCR).yearVisible] = row[(*Dingo::DBManager::yearCR).yearTracksCount];
  }
  
  d_dbmanager->finalizeQueryCountTracksInYear();
}

void Dingo::YearTreeView::onYearRowModify(const Gtk::TreeModel::iterator& iter, Glib::ValueBase& value, int column_id) {
  Gtk::TreeModel::Row temp_row = *(d_year_filter->convert_iter_to_child_iter(d_year_sort->convert_iter_to_child_iter(d_year_modify_filter->convert_iter_to_child_iter(iter))));
  
  switch (column_id) {
    case (0): {
      Glib::Value<Glib::RefPtr<Gdk::Pixbuf> > valPixbuf;
      valPixbuf.init(Glib::Value<Glib::RefPtr<Gdk::Pixbuf> >::value_type());
        
      Glib::RefPtr<Gdk::Pixbuf> temp_cur_pixbuf = temp_row[(*Dingo::DBManager::yearCR).yearIcon];
        
      valPixbuf.set(temp_cur_pixbuf);
      value = valPixbuf;
      
      break;
    }
      
    case (1): {
      Glib::Value<int> valInt;
      valInt.init(Glib::Value<int>::value_type());
        
      int temp_year_id = temp_row[(*Dingo::DBManager::yearCR).yearID];
        
      valInt.set(temp_year_id);
      value = valInt;
      
      break;
    }
      
    case (2): {
      Glib::Value<Glib::ustring> valStr;
      valStr.init(Glib::Value<Glib::ustring>::value_type());
      Glib::ustring temp_cur_year_name = "";
        
      if (temp_row[(*Dingo::DBManager::yearCR).yearID] == 0) {
        temp_cur_year_name = Glib::ustring::compose(Dingo::Utilities::highlightString(_("All %1 Years")), d_visible_years_count);
      }
        
      else {
        temp_cur_year_name = temp_row[(*Dingo::DBManager::yearCR).yearName] + " (" + Dingo::Utilities::intToString(temp_row[(*Dingo::DBManager::yearCR).yearTracksCount]) + ")";
      }
      
      valStr.set(temp_cur_year_name);
      value = valStr;
      
      break;
    }
      
    default: {
      break;
    }
  }
}

int Dingo::YearTreeView::onYearNameCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter) {
  Gtk::TreeModel::Row temp_first_row = *first_iter;
  Gtk::TreeModel::Row temp_second_row = *second_iter;
  
  if (temp_first_row[(*Dingo::DBManager::yearCR).yearID] == 0) {
    return -1;
  }
  
  else if (temp_second_row[(*Dingo::DBManager::yearCR).yearID] == 0) {
    return 1;
  }
  
  Glib::ustring first_title_str = Dingo::Utilities::unhighlightString(temp_first_row[(*Dingo::DBManager::yearCR).yearName]);
  
  Glib::ustring second_title_str = Dingo::Utilities::unhighlightString(temp_second_row[(*Dingo::DBManager::yearCR).yearName]);
  
  if (first_title_str < second_title_str) {
    return 1;
  }
  
  else if (first_title_str > second_title_str) {
    return -1;
  }
  
  else {
    return 0;
  }
}

//UTILITIES
std::vector<Gtk::TreeRowReference> Dingo::YearTreeView::getSelectionRefs() {
  std::vector<Gtk::TreeModel::Path> temp_path_vector = get_selection()->get_selected_rows();
  
  std::vector<Gtk::TreeRowReference> temp_ref_vector;
  
  for (std::vector<Gtk::TreeModel::Path>::iterator path_iter = temp_path_vector.begin(); path_iter != temp_path_vector.end(); ++path_iter) {
    Gtk::TreeRowReference temp_ref(Dingo::DBManager::yearModel, d_year_filter->convert_path_to_child_path(d_year_sort->convert_path_to_child_path(d_year_modify_filter->convert_path_to_child_path(*path_iter))));
    
    Gtk::TreeModel::Row temp_row = *(Dingo::DBManager::yearModel->get_iter(d_year_filter->convert_path_to_child_path(d_year_sort->convert_path_to_child_path(d_year_modify_filter->convert_path_to_child_path(*path_iter)))));
    
    if (temp_row[(*Dingo::DBManager::yearCR).yearID] == 0) {
      temp_ref_vector.clear();
      break;
    }
    
    temp_ref_vector.push_back(temp_ref);
  }
  
  return temp_ref_vector;
}
