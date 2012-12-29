/* 
 * dingo-multipletracksinfowindow.cc
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

#include "dingo-multipletracksinfowindow.h"
#include "dingo-dbmanager.h"
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

Dingo::MultipleTracksInfoWindow::MultipleTracksInfoWindow() :
  Gtk::Window(Gtk::WINDOW_TOPLEVEL)
{

}

Dingo::MultipleTracksInfoWindow::MultipleTracksInfoWindow(Dingo::DBManager* dbmanager, Dingo::Settings* settings) :
  Gtk::Window(Gtk::WINDOW_TOPLEVEL),
  d_cancel_button(_("Cancel")),
  d_save_button(_("Save"))
{
  d_dbmanager = dbmanager;
  d_settings = settings;
  
  createMainGrid();
  createNoteBook();
  createButtonsHBox();
  createTopVBox();
  
  set_size_request(390, 470);
  set_default_size(390, 520);
  set_title(_("Selected Tracks Properties"));
  set_transient_for(*(d_dbmanager->getWindow()));
  set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
  
  set_deletable(false);
  
  d_artist_value_changed = false;
  d_album_value_changed = false;
  d_genre_value_changed = false;
  d_year_value_changed = false;
}

Dingo::MultipleTracksInfoWindow::~MultipleTracksInfoWindow() {

}
      
//PUBLIC METHODS
void Dingo::MultipleTracksInfoWindow::setDisplayTracks(std::vector<Gtk::TreeRowReference> edit_row_vector) {
  cur_edit_row_vector = edit_row_vector;

  //first refresh all entries
  d_cur_title = "";
  d_cur_artist = "";
  d_cur_album = "";
  d_cur_genre = "";
  d_cur_track_number = "";
  d_cur_year = "";
  d_title_entry.set_text("");
  d_artist_entry.set_text("");
  d_album_entry.set_text("");
  d_genre_entry.set_text("");
  d_track_number_entry.set_text("");
  d_year_entry.set_text("");
  d_comment_textbuffer->set_text("");

  //now we continue to setDisplayTracks
  std::vector<int> selected_tracks_id_vector;
  
  for (std::vector<Gtk::TreeRowReference>::iterator iter = cur_edit_row_vector.begin(); iter != cur_edit_row_vector.end(); ++iter) {
    Gtk::TreeModel::Row temp_row = *(Dingo::DBManager::trackModel->get_iter(iter->get_path()));
    
    selected_tracks_id_vector.push_back(temp_row[(*Dingo::DBManager::trackCR).trackID]);
  }
  
  std::map<Glib::ustring, Glib::ustring> multiple_tracks_info_map = d_dbmanager->getMultipleTracksInfo(selected_tracks_id_vector);
  
  d_cur_title = multiple_tracks_info_map["TrackTitle"];
  d_title_entry.set_text(d_cur_title);
  
  d_cur_artist = multiple_tracks_info_map["ArtistName"];
  d_artist_entry.set_text(d_cur_artist);
  
  d_cur_album = multiple_tracks_info_map["AlbumName"];
  d_album_entry.set_text(d_cur_album);
  
  d_cur_genre = multiple_tracks_info_map["GenreName"];
  d_genre_entry.set_text(d_cur_genre);
  
  d_cur_track_number = multiple_tracks_info_map["TrackNumber"];
  d_track_number_entry.set_text(d_cur_track_number);
  
  d_cur_year = multiple_tracks_info_map["YearName"];
  d_year_entry.set_text(d_cur_year);
  
  d_cur_comment = multiple_tracks_info_map["TrackComment"];
  d_comment_textbuffer->set_text(d_cur_comment);
  
  d_duration_value.set_text(multiple_tracks_info_map["TotalDuration"]);
  d_size_value.set_text(multiple_tracks_info_map["TotalSize"]);
}
    
//CREATORS
void Dingo::MultipleTracksInfoWindow::createTopVBox() {
  separator_hbox.set_size_request(15, 15);
  top_vbox.set_border_width(8);
  
  top_vbox.pack_start(d_notebook, Gtk::PACK_EXPAND_WIDGET);
  top_vbox.pack_start(separator_hbox, Gtk::PACK_SHRINK);
  top_vbox.pack_start(buttons_hbox, Gtk::PACK_SHRINK);

  add(top_vbox);
}

void Dingo::MultipleTracksInfoWindow::createButtonsHBox() {
  d_cancel_button.signal_clicked().connect(sigc::mem_fun(*this, &Dingo::MultipleTracksInfoWindow::onCancelButtonClicked));
  d_save_button.signal_clicked().connect(sigc::mem_fun(*this, &Dingo::MultipleTracksInfoWindow::onSaveButtonClicked));

  d_save_button.set_size_request(85, 30);
  d_cancel_button.set_size_request(85, 30);

  buttons_hbox.pack_end(d_save_button, Gtk::PACK_SHRINK);
  buttons_hbox.pack_end(d_cancel_button, Gtk::PACK_SHRINK, 6);
}

void Dingo::MultipleTracksInfoWindow::createNoteBook() {
  d_notebook.append_page(d_main_grid, _("General"));
}

void Dingo::MultipleTracksInfoWindow::createMainGrid() {
  d_title_label.set_markup(Dingo::Utilities::highlightString(_("Title:")));
  d_artist_label.set_markup(Dingo::Utilities::highlightString(_("Artist:")));
  d_album_label.set_markup(Dingo::Utilities::highlightString(_("Album:")));
  d_genre_label.set_markup(Dingo::Utilities::highlightString(_("Genre:")));
  d_track_number_label.set_markup(Dingo::Utilities::highlightString(_("Track Number:")));
  d_year_label.set_markup(Dingo::Utilities::highlightString(_("Year:")));
  d_duration_label.set_markup(Dingo::Utilities::highlightString(_("Total Duration:")));
  d_size_label.set_markup(Dingo::Utilities::highlightString(_("Total Size:")));
  d_comment_label.set_markup(Dingo::Utilities::highlightString(_("Comment:")));
  
  d_title_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_artist_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_album_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_genre_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_track_number_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_year_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_duration_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_size_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_comment_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  
  d_comment_textbuffer = Gtk::TextBuffer::create();
  d_comment_textview.set_buffer(d_comment_textbuffer);
  
  d_comment_scrolled.add(d_comment_textview);
  d_comment_scrolled.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  d_comment_scrolled.set_shadow_type(Gtk::SHADOW_OUT);
  d_comment_scrolled.set_vexpand(true);

  d_main_grid.attach(d_title_label, 0, 0, 1, 1);
  d_main_grid.attach(d_title_entry, 1, 0, 2, 1);
  d_main_grid.attach(d_artist_label, 0, 1, 1, 1);
  d_main_grid.attach(d_artist_entry, 1, 1, 2, 1);
  d_main_grid.attach(d_album_label, 0, 2, 1, 1);
  d_main_grid.attach(d_album_entry, 1, 2, 2, 1);
  d_main_grid.attach(d_genre_label, 0, 3, 1, 1);
  d_main_grid.attach(d_genre_entry, 1, 3, 2, 1);
  d_main_grid.attach(d_track_number_label, 0, 4, 1, 1);
  d_main_grid.attach(d_track_number_entry, 1, 4, 2, 1);
  d_main_grid.attach(d_year_label, 0, 5, 1, 1);
  d_main_grid.attach(d_year_entry, 1, 5, 2, 1);
  d_main_grid.attach(d_duration_label, 0, 6, 1, 1);
  d_main_grid.attach(d_duration_value, 1, 6, 2, 1);
  d_main_grid.attach(d_size_label, 0, 7, 1, 1);
  d_main_grid.attach(d_size_value, 1, 7, 2, 1);
  d_main_grid.attach(d_comment_label, 0, 8, 1, 1);
  d_main_grid.attach(d_comment_scrolled, 1, 8, 2, 1);
  
  d_main_grid.set_row_spacing(7);
  d_main_grid.set_column_spacing(10);
  d_main_grid.set_border_width(10);
  d_main_grid.set_hexpand(true);
  d_main_grid.set_vexpand(true);
  
  d_title_entry.set_hexpand(true);
  d_artist_entry.set_hexpand(true);
  d_album_entry.set_hexpand(true);
  d_genre_entry.set_hexpand(true);
  d_track_number_entry.set_hexpand(true);
  d_year_entry.set_hexpand(true);
  d_duration_value.set_hexpand(true);
  d_size_value.set_hexpand(true);
  d_comment_scrolled.set_hexpand(true);
  
  d_duration_value.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_size_value.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
}
      
//SIGNAL HANDLERS
void Dingo::MultipleTracksInfoWindow::onSaveButtonClicked() {
  applyNewTracksInfo();
  
  //refresh Gtk::Main
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  resetOldChangedValuesInfo();
  
  hide();
}

void Dingo::MultipleTracksInfoWindow::onCancelButtonClicked() {
  resetOldChangedValuesInfo();
  
  hide();
}

void Dingo::MultipleTracksInfoWindow::applyNewTracksInfo() {
  bool should_set_info = false;
  
  Glib::ustring d_title_entry_value = d_title_entry.get_text();
  if (d_cur_title != d_title_entry_value) {
    should_set_info = true;
    
    if (d_title_entry_value == "") {
      d_title_entry_value = "Unknown Title";
    }
    
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackTitle", d_title_entry_value));
  }
  
  Glib::ustring d_artist_entry_value = d_artist_entry.get_text();
  if (d_cur_artist != d_artist_entry_value) {
    should_set_info = true;
    d_artist_value_changed = true;
    
    if (d_artist_entry_value == "") {
      d_artist_entry_value = "Unknown Artist";
    }
  
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackArtist", d_artist_entry_value));
  }
  
  Glib::ustring d_album_entry_value = d_album_entry.get_text();
  if (d_cur_album != d_album_entry_value) {
    should_set_info = true;
    d_album_value_changed = true;
    
    if (d_album_entry_value == "") {
      d_album_entry_value = "Unknown Album";
    }
  
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackAlbum", d_album_entry_value));
  }
  
  Glib::ustring d_genre_entry_value = d_genre_entry.get_text();
  if (d_cur_genre != d_genre_entry_value) {
    should_set_info = true;
    d_genre_value_changed = true;
    
    if (d_genre_entry_value == "") {
      d_genre_entry_value = "Unknown Genre";
    }
  
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackGenre", d_genre_entry_value));
  }
  
  Glib::ustring d_track_number_entry_value = d_track_number_entry.get_text();
  if (d_cur_track_number != d_track_number_entry_value) {
    should_set_info = true;
  
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackNumber", d_track_number_entry_value));
  }
  
  Glib::ustring d_year_entry_value = d_year_entry.get_text();
  if (d_cur_year != d_year_entry_value) {
    should_set_info = true;
    d_year_value_changed = true;
    
    if (d_year_entry.get_text() == "") {
      d_year_entry_value = "Unknown Year";
    }
  
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackYear", d_year_entry_value));
  }
  
  Glib::ustring d_comment_textview_value = d_comment_textbuffer->get_text();
  if (d_cur_comment != d_comment_textview_value) {
    should_set_info = true;
  
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackComment", d_comment_textview_value));
  }
  
  if (should_set_info) {
    d_dbmanager->setMultipleTracksInfo(cur_edit_row_vector, d_changed_values_map);

    Gtk::TreeModel::Row playing_row = *(Dingo::DBManager::trackModel->get_iter(d_dbmanager->getCurrentTrackRowRef().get_path()));
    
    for (std::vector<Gtk::TreeRowReference>::iterator iter = cur_edit_row_vector.begin(); iter != cur_edit_row_vector.end(); ++iter) {
      Gtk::TreeModel::Row row = *(Dingo::DBManager::trackModel->get_iter(iter->get_path()));
      
      if (row[(*Dingo::DBManager::trackCR).trackID] == playing_row[(*Dingo::DBManager::trackCR).trackID]) {
        Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Metadata");
      }
    }
  }
}

void Dingo::MultipleTracksInfoWindow::resetOldChangedValuesInfo() {
  if ((d_settings->getVisible("artistfilter")) && (d_artist_value_changed)) {
    d_dbmanager->notifyDBManArtistObserver(Dingo::CHANGES_IN_CORE_ARTISTS);
  }
    
  if ((d_settings->getVisible("albumfilter")) && (d_album_value_changed)) {
    d_dbmanager->notifyDBManAlbumObserver(Dingo::CHANGES_IN_CORE_ALBUMS);
  }
    
  if ((d_settings->getVisible("genrefilter")) && (d_genre_value_changed)) {
    d_dbmanager->notifyDBManGenreObserver(Dingo::CHANGES_IN_CORE_GENRES);
  }
    
  if ((d_settings->getVisible("yearfilter")) && (d_year_value_changed)) {
    d_dbmanager->notifyDBManYearObserver(Dingo::CHANGES_IN_CORE_YEARS);
  }
  
  d_artist_value_changed = false;
  d_album_value_changed = false;
  d_genre_value_changed = false;
  d_year_value_changed = false;
  
  d_changed_values_map.clear();
}
