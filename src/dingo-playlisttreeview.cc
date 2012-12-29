/* 
 * dingo-playlisttreeview.cc
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

#include "dingo-playlisttreeview.h"
#include "dingo-playlisttreeviewobserver.h"
#include "dingo-dbmanager.h"
#include "dingo-settings.h"
#include "dingo-statusbar.h"
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

Dingo::PlaylistTreeView::PlaylistTreeView() {

}

Dingo::PlaylistTreeView::PlaylistTreeView(Dingo::DBManager* dbmanager, Dingo::Settings* settings, Dingo::StatusBar* statusbar) :
  Gtk::TreeView(),
  playlist_new(*(Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU))), _("New Empty Playlist...")),
  playlist_import(*(Gtk::manage(new Gtk::Image(Gtk::Stock::ADD, Gtk::ICON_SIZE_MENU))), _("Import From File(s)...")),
  playlist_rename(*(Gtk::manage(new Gtk::Image(Gtk::Stock::EDIT, Gtk::ICON_SIZE_MENU))), _("Rename Selected Playlist")),
  playlist_export(*(Gtk::manage(new Gtk::Image(Gtk::Stock::JUMP_TO, Gtk::ICON_SIZE_MENU))), _("Export To File...")),
  playlist_delete(*(Gtk::manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_MENU))), _("Delete Selected Playlist"))
{
  d_dbmanager = dbmanager;
  d_statusbar = statusbar;
  d_settings = settings;
  
  is_playlist_name_filtered = false;
  is_started_up = false;
  d_last_import_folder_uri = Glib::filename_to_uri(Glib::get_home_dir());
  d_last_export_folder_uri = Glib::filename_to_uri(Glib::get_home_dir());
  
  createPlaylistTreeView();
  createPlaylistTreeMenu();
}

Dingo::PlaylistTreeView::~PlaylistTreeView() {

}
      
//OBSERVER'S METHODS
void Dingo::PlaylistTreeView::registerPlaylistTreeViewObserver(Dingo::PlaylistTreeViewObserver* observer) {
  playlist_obs_vector.push_back(observer);
}

void Dingo::PlaylistTreeView::removePlaylistTreeViewObserver(Dingo::PlaylistTreeViewObserver* observer) {
  std::vector<Dingo::PlaylistTreeViewObserver*>::iterator iter = std::find(playlist_obs_vector.begin(), playlist_obs_vector.end(), observer);
  
  if (iter != playlist_obs_vector.end()) {
    playlist_obs_vector.erase(iter);
  }
}

void Dingo::PlaylistTreeView::notifyPlaylistTreeViewObserver(Dingo::TreeViewSignals signal_type) {
  switch (signal_type) {
    case Dingo::PLAYLIST_SELECTED: {
      for (std::vector<Dingo::PlaylistTreeViewObserver*>::iterator iter = playlist_obs_vector.begin(); iter != playlist_obs_vector.end(); ++iter) {
        (*iter)->updateBlockSelection();
      }
    
      for (std::vector<Dingo::PlaylistTreeViewObserver*>::iterator iter = playlist_obs_vector.begin(); iter != playlist_obs_vector.end(); ++iter) {
        (*iter)->updateRestoreSelectedRows();
      }
      
      for (std::vector<Dingo::PlaylistTreeViewObserver*>::iterator iter = playlist_obs_vector.begin(); iter != playlist_obs_vector.end(); ++iter) {
        (*iter)->updateUnblockSelection();
      }
      d_dbmanager->setPreviousPlaylistID(d_dbmanager->getCurrentPlaylistID());
      //std::cout << "Playlist Selected!" << std::endl;
      break;
    }
    
    default: {
      std::cerr << "Undefined Dingo::TreeViewSignals" << std::endl;
      break;
    }
  }
}

//INHERITED OBSERVER'S METHODS
void Dingo::PlaylistTreeView::updateChangesInCorePlaylists() {
  refreshData();
}

void Dingo::PlaylistTreeView::updateTrackPlaylistEditInfoSaved() {
  refreshData();
}

//CREATORS
void Dingo::PlaylistTreeView::createPlaylistTreeView() {
  d_playlist_sort = Gtk::TreeModelSort::create(Dingo::DBManager::playlistModel);
  d_playlist_sort->set_sort_func((*Dingo::DBManager::playlistCR).playlistName, sigc::mem_fun(*this, &Dingo::PlaylistTreeView::onPlaylistNameCompare));
  
  filterPlaylists();
  
  d_playlist_modify_filter = Gtk::TreeModelFilter::create(d_playlist_sort);
  d_playlist_modify_filter->set_modify_func(*Dingo::DBManager::playlistCR, sigc::mem_fun(*this, &Dingo::PlaylistTreeView::onPlaylistRowModify));
  d_playlist_sort->set_sort_column((*Dingo::DBManager::playlistCR).playlistName, Gtk::SORT_ASCENDING);

  set_model(d_playlist_modify_filter);
  set_rules_hint(true);
  
  Gtk::TreeView::Column* p_column = Gtk::manage(new Gtk::TreeView::Column(_("Playlist")));
  Gtk::CellRendererPixbuf* p_renderer_playlisticon = Gtk::manage(new Gtk::CellRendererPixbuf());
  Gtk::CellRendererText* p_renderer_playlistname = Gtk::manage(new Gtk::CellRendererText());
  
  p_column->pack_start(*p_renderer_playlisticon, false);
  p_column->pack_start(*p_renderer_playlistname);
  
  append_column(*p_column);
  
  p_column->add_attribute(p_renderer_playlistname->property_markup(), (*Dingo::DBManager::playlistCR).playlistName);
  p_column->add_attribute(p_renderer_playlisticon->property_pixbuf(), (*Dingo::DBManager::playlistCR).playlistIcon);
  p_column->set_expand(true);
  p_column->set_resizable(true);
  
  get_selection()->set_mode(Gtk::SELECTION_SINGLE);
  get_selection()->signal_changed().connect(sigc::mem_fun(*this, &Dingo::PlaylistTreeView::onSelectionChanged));
  
  get_selection()->select(get_model()->children().begin());
  
  is_started_up = true;
}

void Dingo::PlaylistTreeView::createPlaylistTreeMenu() {
  playlist_menu.append(playlist_new);
  playlist_menu.append(playlist_import);
  playlist_menu.append(*(Gtk::manage(new Gtk::SeparatorMenuItem())));
  playlist_menu.append(playlist_rename);
  playlist_menu.append(playlist_export);
  playlist_menu.append(*(Gtk::manage(new Gtk::SeparatorMenuItem())));
  playlist_menu.append(playlist_delete);
  
  playlist_new.signal_activate().connect(sigc::mem_fun(*this, &Dingo::PlaylistTreeView::onNewPlaylistActivated));
  playlist_import.signal_activate().connect(sigc::mem_fun(*this, &Dingo::PlaylistTreeView::onImportPlaylistActivated));
  playlist_rename.signal_activate().connect(sigc::mem_fun(*this, &Dingo::PlaylistTreeView::onRenamePlaylistActivated));
  playlist_export.signal_activate().connect(sigc::mem_fun(*this, &Dingo::PlaylistTreeView::onExportPlaylistActivated));
  playlist_delete.signal_activate().connect(sigc::mem_fun(*this, &Dingo::PlaylistTreeView::onDeletePlaylistActivated));
  
  playlist_menu.show_all_children();
}

//SIGNAL HANDLERS
void Dingo::PlaylistTreeView::onSelectionChanged() {
  Gtk::TreeModel::iterator temp_iter = d_playlist_sort->convert_iter_to_child_iter(d_playlist_modify_filter->convert_iter_to_child_iter(get_selection()->get_selected()));
  
  Gtk::TreeModel::Row row = *temp_iter;
  
  int temp_playlist_id = row[(*Dingo::DBManager::playlistCR).playlistID];
  
  d_dbmanager->setCurrentPlaylistID(temp_playlist_id);
  
  if (d_dbmanager->getPreviousPlaylistID() == -1) {
    d_dbmanager->setPreviousPlaylistID(temp_playlist_id);
  }
  
  notifyPlaylistTreeViewObserver(Dingo::PLAYLIST_SELECTED);
  
  if (is_started_up) {
    Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYLISTS, "ActivePlaylist");
  }
}

void Dingo::PlaylistTreeView::filterPlaylists() {
  Gtk::TreeModel::Children playlist_children = Dingo::DBManager::playlistModel->children();
  
  //get Total Tracks Count for the first row
  Gtk::TreeModel::Row playlist_first_row = *(playlist_children.begin());
  playlist_first_row[(*Dingo::DBManager::playlistCR).playlistTracksCount] = Dingo::DBManager::trackModel->children().size();

  d_dbmanager->prepareQueryCountTracksInPlaylist();
  
  for (Gtk::TreeModel::Children::iterator iter = ++playlist_children.begin(); iter != playlist_children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    
    row[(*Dingo::DBManager::playlistCR).playlistTracksCount] = d_dbmanager->getCountTracksInPlaylist(row[(*Dingo::DBManager::playlistCR).playlistID]);
  }
  
  d_dbmanager->finalizeQueryCountTracksInPlaylist();
}

void Dingo::PlaylistTreeView::onPlaylistRowModify(const Gtk::TreeModel::iterator& iter, Glib::ValueBase& value, int column_id) {
  Gtk::TreeModel::Row temp_row = *(d_playlist_sort->convert_iter_to_child_iter(d_playlist_modify_filter->convert_iter_to_child_iter(iter)));
      
  switch (column_id) {
    case (0): {
      Glib::Value<Glib::RefPtr<Gdk::Pixbuf> > valPixbuf;
      valPixbuf.init(Glib::Value<Glib::RefPtr<Gdk::Pixbuf> >::value_type());
        
      Glib::RefPtr<Gdk::Pixbuf> temp_cur_pixbuf = temp_row[(*Dingo::DBManager::playlistCR).playlistIcon];
        
      valPixbuf.set(temp_cur_pixbuf);
      value = valPixbuf;
          
      break;
    }   
     
    case (1): {
      Glib::Value<int> valInt;
      valInt.init(Glib::Value<int>::value_type());
        
      int temp_playlist_id = temp_row[(*Dingo::DBManager::playlistCR).playlistID];
        
      valInt.set(temp_playlist_id);
      value = valInt;
        
      break;
    }
      
    case (2): {
      Glib::Value<Glib::ustring> valStr;
      valStr.init(Glib::Value<Glib::ustring>::value_type());
      Glib::ustring temp_cur_playlist_name = "";
        
      if (temp_row[(*Dingo::DBManager::playlistCR).playlistID] == 0) {
        temp_cur_playlist_name = Glib::ustring::compose(Dingo::Utilities::highlightString(_("All %1 Tracks")), temp_row[(*Dingo::DBManager::playlistCR).playlistTracksCount]);
      }
        
      else {        
        temp_cur_playlist_name = temp_row[(*Dingo::DBManager::playlistCR).playlistName] + " (" + Dingo::Utilities::intToString(temp_row[(*Dingo::DBManager::playlistCR).playlistTracksCount]) + ")";
      }
        
      valStr.set(temp_cur_playlist_name);
      value = valStr;
        
      break;
    }
    
    default: {
      break;
    }
  }
}

int Dingo::PlaylistTreeView::onPlaylistNameCompare(const Gtk::TreeModel::iterator& first_iter, const Gtk::TreeModel::iterator& second_iter) {
  Gtk::TreeModel::Row temp_first_row = *first_iter;
  Gtk::TreeModel::Row temp_second_row = *second_iter;
  
  if (temp_first_row[(*Dingo::DBManager::playlistCR).playlistID] == 0) {
    return -1;
  }
  
  else if (temp_second_row[(*Dingo::DBManager::playlistCR).playlistID] == 0) {
    return 1;
  }
  
  Glib::ustring first_title_str = Dingo::Utilities::unhighlightString(temp_first_row[(*Dingo::DBManager::playlistCR).playlistName]);
  
  Glib::ustring second_title_str = Dingo::Utilities::unhighlightString(temp_second_row[(*Dingo::DBManager::playlistCR).playlistName]);
  
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

void Dingo::PlaylistTreeView::onNewPlaylistActivated() {
  Gtk::Image new_playlist_image(DATADIR"/dingo/icons/scalable/dingo-playlist-new-scalable.svg");
  Gtk::Label new_playlist_label(_("Please enter a name for the new playlist:"));
  Gtk::Entry new_playlist_entry;
  Gtk::Grid top_grid;
  
  new_playlist_entry.set_text(_("New Playlist"));
  
  top_grid.attach(new_playlist_image, 0, 0, 2, 2);
  top_grid.attach(new_playlist_label, 2, 0, 1, 1);
  top_grid.attach(new_playlist_entry, 2, 1, 1, 1);
  
  top_grid.set_row_spacing(7);
  top_grid.set_column_spacing(10);
  top_grid.set_border_width(5);
  
  new_playlist_entry.set_hexpand(true);
  new_playlist_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  
  Gtk::Dialog dialog("", true);
  
  Gtk::Box* p_vbox = dialog.get_content_area();
  
  p_vbox->pack_start(top_grid, Gtk::PACK_EXPAND_WIDGET);
  
  dialog.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
  dialog.add_button(_("Create"), Gtk::RESPONSE_OK);
  
  dialog.set_default_response(Gtk::RESPONSE_OK);
  
  dialog.show_all_children();
  
  dialog.set_transient_for(*(d_dbmanager->getWindow()));
  
  int result = dialog.run();
  
  dialog.hide();
  
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  switch (result) {
    case Gtk::RESPONSE_OK: {
      //std::cout << "Create button clicked!" << std::endl;
      
      d_dbmanager->createPlaylist(new_playlist_entry.get_text());
      
      break;
    }
    
    case Gtk::RESPONSE_CANCEL: {
      //std::cout << "Cancel button clicked!" << std::endl;
    }
    
    default: {
      break;
    }
  }
}

void Dingo::PlaylistTreeView::onImportPlaylistActivated() {
  //create Gtk::FileFilter
  Glib::RefPtr<Gtk::FileFilter> p_filter = Gtk::FileFilter::create();
  
  p_filter->add_pattern("*.dpls");
  
  p_filter->set_name(_("Dingo Playlist Files (*.dpls)"));

  Gtk::FileChooserDialog dialog(_("Import Playlist"), Gtk::FILE_CHOOSER_ACTION_OPEN);
  
  dialog.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
  dialog.add_button(_("Import"), Gtk::RESPONSE_OK);
  dialog.add_filter(p_filter);
  dialog.set_select_multiple(true);
  dialog.set_current_folder_uri(d_last_import_folder_uri);
  
  dialog.set_default_response(Gtk::RESPONSE_OK);
  
  int result = dialog.run();
  
  std::vector<Glib::ustring> temp_uris = dialog.get_uris();
  d_last_import_folder_uri = dialog.get_current_folder_uri();
  
  dialog.hide();
  
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  switch (result) {
    case (Gtk::RESPONSE_OK): {
      //std::cout << "Open Clicked!" << std::endl;
      
      d_dbmanager->importPlaylists(temp_uris);
      
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
    
    case (Gtk::RESPONSE_CANCEL): {
      //std::cout << "Cancel Clicked!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unexpected button clicked!" << std::endl;
      break;
    }
  }
}

void Dingo::PlaylistTreeView::onRenamePlaylistActivated() {
  Gtk::TreeModel::iterator temp_iter = d_playlist_sort->convert_iter_to_child_iter(d_playlist_modify_filter->convert_iter_to_child_iter(get_selection()->get_selected()));
  Gtk::TreeModel::Row row = *temp_iter;

  Gtk::Image rename_playlist_image(DATADIR"/dingo/icons/scalable/dingo-playlist-scalable.svg");
  Gtk::Label rename_playlist_label(_("Please enter a new name for the playlist:"));
  Gtk::Entry rename_playlist_entry;
  Gtk::Grid top_grid;
  
  rename_playlist_entry.set_text(Dingo::Utilities::decodeToEntityString(row[(*Dingo::DBManager::playlistCR).playlistName]));
  
  top_grid.attach(rename_playlist_image, 0, 0, 2, 2);
  top_grid.attach(rename_playlist_label, 2, 0, 1, 1);
  top_grid.attach(rename_playlist_entry, 2, 1, 1, 1);
  
  top_grid.set_row_spacing(7);
  top_grid.set_column_spacing(10);
  top_grid.set_border_width(5);
  
  rename_playlist_entry.set_hexpand(true);
  rename_playlist_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  
  Gtk::Dialog dialog("", true);
  
  Gtk::Box* p_vbox = dialog.get_content_area();
  
  p_vbox->pack_start(top_grid, Gtk::PACK_EXPAND_WIDGET);

  dialog.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
  dialog.add_button(_("Rename"), Gtk::RESPONSE_OK);
  
  dialog.set_default_response(Gtk::RESPONSE_OK);
  
  dialog.show_all_children();
  
  dialog.set_transient_for(*(d_dbmanager->getWindow()));
  
  int result = dialog.run();
  
  dialog.hide();
  
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  switch (result) {
    case Gtk::RESPONSE_OK: {      
      d_dbmanager->renamePlaylist(row[(*Dingo::DBManager::playlistCR).playlistID], rename_playlist_entry.get_text());
      
      break;
    }
    
    case Gtk::RESPONSE_CANCEL: {
      //std::cout << "Rename Dialog: CANCEL button clicked!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Rename Dialog: Unknown button clicked!" << std::endl;
      break;
    }
  }
}

void Dingo::PlaylistTreeView::onExportPlaylistActivated() {
  Gtk::TreeModel::iterator temp_iter = d_playlist_sort->convert_iter_to_child_iter(d_playlist_modify_filter->convert_iter_to_child_iter(get_selection()->get_selected()));
  Gtk::TreeModel::Row temp_row = *temp_iter;
  
  Gtk::FileChooserDialog dialog(Glib::ustring::compose(_("Export Playlist: %1"), temp_row[(*Dingo::DBManager::playlistCR).playlistName]), Gtk::FILE_CHOOSER_ACTION_SAVE);
  
  dialog.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
  dialog.add_button(_("Export"), Gtk::RESPONSE_OK);
  dialog.set_select_multiple(false);
  dialog.set_current_folder_uri(d_last_export_folder_uri);
  
  dialog.set_default_response(Gtk::RESPONSE_OK);
  dialog.set_create_folders(true);
  dialog.set_do_overwrite_confirmation(true);
  dialog.set_current_name(temp_row[(*Dingo::DBManager::playlistCR).playlistName] + ".dpls");
  
  int result = dialog.run();
  
  Glib::ustring temp_uri = dialog.get_uri();
  d_last_export_folder_uri = dialog.get_current_folder_uri();
  
  dialog.hide();
  
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  switch (result) {
    case (Gtk::RESPONSE_OK): {
      //std::cout << "Open Clicked!" << std::endl;
      
      d_dbmanager->exportPlaylist(temp_uri, temp_row[(*Dingo::DBManager::playlistCR).playlistID], Dingo::Utilities::unhighlightString(temp_row[(*Dingo::DBManager::playlistCR).playlistName]));
      
      break;
    }
    
    case (Gtk::RESPONSE_CANCEL): {
      //std::cout << "Cancel Clicked!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unexpected button clicked!" << std::endl;
      break;
    }
  }
}

void Dingo::PlaylistTreeView::onDeletePlaylistActivated() {
  Gtk::TreeModel::iterator temp_iter = d_playlist_sort->convert_iter_to_child_iter(d_playlist_modify_filter->convert_iter_to_child_iter(get_selection()->get_selected()));
  Gtk::TreeModel::Row row = *temp_iter;
  
  Gtk::Image question_image(Gtk::Stock::DIALOG_QUESTION, Gtk::ICON_SIZE_DIALOG);
  Gtk::Label del_playlist_label(_("Delete the selected Playlist?"));
  Gtk::Grid top_grid;
  
  top_grid.attach(question_image, 0, 0, 2, 2);
  top_grid.attach(del_playlist_label, 2, 0, 1, 1);
  
  top_grid.set_row_spacing(7);
  top_grid.set_column_spacing(10);
  top_grid.set_border_width(5);
  
  Gtk::Dialog dialog("", true);
  
  Gtk::Box* p_vbox = dialog.get_content_area();
  
  p_vbox->pack_start(top_grid, Gtk::PACK_EXPAND_WIDGET);
  
  dialog.add_button(_("No"), Gtk::RESPONSE_NO);
  dialog.add_button(_("Yes"), Gtk::RESPONSE_YES);
  
  dialog.set_default_response(Gtk::RESPONSE_OK);
  
  dialog.show_all_children();
  
  dialog.set_transient_for(*(d_dbmanager->getWindow()));
  
  int result = dialog.run();
  
  dialog.hide();
  
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  switch (result) {
    case Gtk::RESPONSE_YES: {
      //std::cout << "Delete Dialog: Yes button clicked!" << std::endl;
      
      d_dbmanager->deletePlaylist(row[(*Dingo::DBManager::playlistCR).playlistID]);
      
      break;
    }
    
    case Gtk::RESPONSE_NO: {
      //std::cout << "Delete Dialog: No button clicked!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Delete Dialog: Unknown button clicked!" << std::endl;
      break;
    }
  }
}

//PUBLIC METHODS
void Dingo::PlaylistTreeView::activatePlaylist(int playlist_id) {
  Gtk::TreeModel::Children children = get_model()->children();
  is_started_up = true;
  
  for (Gtk::TreeModel::Children::iterator children_iter = children.begin(); children_iter != children.end(); ++children_iter) {
    Gtk::TreeModel::Row row = *(children_iter);
    
    if (row[(*Dingo::DBManager::playlistCR).playlistID] == playlist_id) {
      get_selection()->select(row);
      
      if (is_started_up) {
        Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYLISTS, "ActivePlaylist");
      }
    
      break;
    }
  }
}

//UTILITIES
void Dingo::PlaylistTreeView::refreshData() {
  filterPlaylists();

  d_playlist_modify_filter->refilter();
  
  d_playlist_sort->set_sort_column((*Dingo::DBManager::playlistCR).playlistName, Gtk::SORT_ASCENDING);
}

bool Dingo::PlaylistTreeView::on_button_press_event(GdkEventButton* event) {
  if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3)) {
    playlist_menu.popup(event->button, event->time);
    
    if (d_dbmanager->getCurrentPlaylistID() == 0) {
      playlist_rename.set_sensitive(false);
      playlist_delete.set_sensitive(false);
    }
    
    else {
      playlist_rename.set_sensitive(true);
      playlist_delete.set_sensitive(true);
    }
  }
  
  return Gtk::TreeView::on_button_press_event(event);
}
