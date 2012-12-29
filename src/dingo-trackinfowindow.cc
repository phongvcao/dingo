/* 
 * dingo-trackinfowindow.cc
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

#include "dingo-trackinfowindow.h"
#include "dingo-trackinfowindowobserver.h"
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

//CONSTRUCTORS - DESTRUCTOR
Dingo::TrackInfoWindow::TrackInfoWindow() :
  Gtk::Window(Gtk::WINDOW_POPUP)
{

}

Dingo::TrackInfoWindow::TrackInfoWindow(Dingo::DBManager* dbmanager, Dingo::Settings* settings) :
  Gtk::Window(Gtk::WINDOW_TOPLEVEL),
  d_back_button(_("Back")),
  d_forward_button(_("Forward")),
  d_close_button(_("Close"))
{
  d_dbmanager = dbmanager;
  d_settings = settings;
  
  createBasicGrid();
  createDetailsGrid();
  createLyricsTextView();
  createNoteBook();
  createButtonsHBox();
  createTopVBox();
  
  set_size_request(390, 450);
  set_default_size(390, 500);
  set_title("");
  set_transient_for(*(d_dbmanager->getWindow()));
  set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
  
  set_deletable(false);
  
  d_artist_value_changed = false;
  d_album_value_changed = false;
  d_genre_value_changed = false;
  d_year_value_changed = false;
}

Dingo::TrackInfoWindow::~TrackInfoWindow() {

}

//OBSERVER'S METHODS
void Dingo::TrackInfoWindow::registerTrackInfoWindowObserver(Dingo::TrackInfoWindowObserver* observer) {
  infowindow_obs_vector.push_back(observer);
}

void Dingo::TrackInfoWindow::removeTrackInfoWindowObserver(Dingo::TrackInfoWindowObserver* observer) {
  std::vector<Dingo::TrackInfoWindowObserver*>::iterator iter = std::find(infowindow_obs_vector.begin(), infowindow_obs_vector.end(), observer);
  
  if (iter != infowindow_obs_vector.end()) {
    infowindow_obs_vector.erase(iter);
  }
}

void Dingo::TrackInfoWindow::notifyTrackInfoWindowObserver(TrackInfoWindowSignals signal_type) {
  switch (signal_type) {
    case Dingo::PREVIOUS_TRACK_INFO: {
      for (std::vector<Dingo::TrackInfoWindowObserver*>::iterator iter = infowindow_obs_vector.begin(); iter != infowindow_obs_vector.end(); ++iter) {
        (*iter)->updateGetPreviousTrackInfo();
      }
      //std::cout << "TrackInfoWindowObserver*: Previous Track" << std::endl;
      break;
    }
    
    case Dingo::NEXT_TRACK_INFO: {
      for (std::vector<Dingo::TrackInfoWindowObserver*>::iterator iter = infowindow_obs_vector.begin(); iter != infowindow_obs_vector.end(); ++iter) {
        (*iter)->updateGetNextTrackInfo();
      }
      //std::cout << "TrackInfoWindowObserver*: Next Track!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Undefined Dingo::TrackInfoWindowSignals!" << std::endl;
      break;
    }
  }
}
      
//PUBLIC METHODS
void Dingo::TrackInfoWindow::setDisplayTrack(Gtk::TreeRowReference cur_edit_row_ref) {
  d_cur_edit_row_ref = cur_edit_row_ref;
  
  Gtk::TreeModel::Row temp_row = *(Dingo::DBManager::trackModel->get_iter(d_cur_edit_row_ref.get_path()));
  
  d_cur_edit_id = temp_row[(*Dingo::DBManager::trackCR).trackID];
  
  set_title(Glib::ustring::compose(_("%1 Properties"), Dingo::Utilities::unhighlightString(temp_row[(*Dingo::DBManager::trackCR).trackTitle])));
  
  std::map<Glib::ustring, Glib::ustring> track_info_map = d_dbmanager->getTrackInfo(d_cur_edit_id);
  
  //Basic Tab
  d_cur_title = track_info_map["TrackTitle"];
  d_title_entry.set_text(d_cur_title);
  
  d_cur_artist = track_info_map["ArtistName"];
  d_artist_entry.set_text(d_cur_artist);
  
  d_cur_album = track_info_map["AlbumName"];
  d_album_entry.set_text(d_cur_album);
  
  d_cur_genre = track_info_map["GenreName"];
  d_genre_entry.set_text(d_cur_genre);
  
  d_cur_track_number = track_info_map["TrackNumber"];
  d_track_number_entry.set_text(d_cur_track_number);
  
  d_cur_year = track_info_map["YearName"];
  d_year_entry.set_text(d_cur_year);
  
  d_cur_comment = track_info_map["TrackComment"];
  d_comment_textbuffer->set_text(d_cur_comment);
  
  if (track_info_map["TrackMIMEType"] == "video/*") {
    d_details_grid.attach(d_subtitle_path, 0, 4, 1, 1);
    d_details_grid.attach(d_subtitle_path_entry, 1, 4, 2, 1);
    d_cur_subtitle_path = track_info_map["TrackSubtitlePath"];
    d_subtitle_path_entry.set_text(d_cur_subtitle_path);
  }
  
  else {
    d_details_grid.remove(d_subtitle_path);
    d_details_grid.remove(d_subtitle_path_entry);
  }
  
  //Details Tab
  d_file_name_entry.set_text(track_info_map["TrackFileName"]);
  
  d_duration_value.set_text(Dingo::Utilities::durationToString(Dingo::Utilities::stringToGInt64(track_info_map["TrackTime"])));
  
  d_bitrate_value.set_text(track_info_map["TrackBitrate"]);
  
  d_location_entry.set_text(track_info_map["TrackLocation"]);
  
  d_subtitle_path_entry.set_text(track_info_map["TrackSubtitlePath"]);
  
  d_file_size_value.set_text(track_info_map["TrackSize"]);
  
  d_date_added_value.set_text(track_info_map["TrackDateAdded"]);
  
  //Lyrics Tab
  d_cur_lyrics = track_info_map["TrackLyrics"];
  d_lyrics_textbuffer->set_text(d_cur_lyrics);
}

void Dingo::TrackInfoWindow::setCurrentPage(int page_index) {
  d_notebook.set_current_page(page_index);
}

//CREATORS
void Dingo::TrackInfoWindow::createTopVBox() {
  top_vbox.pack_start(d_notebook, Gtk::PACK_EXPAND_WIDGET);
  top_vbox.pack_start(d_separator_hbox, Gtk::PACK_SHRINK);
  top_vbox.pack_start(d_buttons_hbox, Gtk::PACK_SHRINK);
  
  d_separator_hbox.set_size_request(15, 15); 
  top_vbox.set_border_width(10);
  
  add(top_vbox);
}

void Dingo::TrackInfoWindow::createButtonsHBox() {
  d_back_button.signal_clicked().connect(sigc::mem_fun(*this, &Dingo::TrackInfoWindow::onBackButtonClicked));
  d_forward_button.signal_clicked().connect(sigc::mem_fun(*this, &Dingo::TrackInfoWindow::onForwardButtonClicked));
  d_close_button.signal_clicked().connect(sigc::mem_fun(*this, &Dingo::TrackInfoWindow::onCloseButtonClicked));
  
  d_back_button.set_size_request(85, 30);
  d_forward_button.set_size_request(85, 30);
  d_close_button.set_size_request(85, 30);

  d_buttons_hbox.pack_end(d_close_button, Gtk::PACK_SHRINK);
  d_buttons_hbox.pack_end(d_forward_button, Gtk::PACK_SHRINK, 6);
  d_buttons_hbox.pack_end(d_back_button, Gtk::PACK_SHRINK);
  
  //d_buttons_hbox.set_border_width(9);
}

void Dingo::TrackInfoWindow::createNoteBook() {
  d_notebook.append_page(d_basic_grid, _("Basic"));
  d_notebook.append_page(d_details_grid, _("Details"));
  d_notebook.append_page(d_lyrics_scrolled, _("Lyrics"));
  
  //d_notebook.set_border_width(12);
}

void Dingo::TrackInfoWindow::createLyricsTextView() {
  d_lyrics_textbuffer = Gtk::TextBuffer::create();
  d_lyrics_textview.set_buffer(d_lyrics_textbuffer);

  d_lyrics_scrolled.add(d_lyrics_textview);
  d_lyrics_scrolled.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  d_lyrics_scrolled.set_border_width(10);
  d_lyrics_scrolled.set_shadow_type(Gtk::SHADOW_OUT);
}

void Dingo::TrackInfoWindow::createDetailsGrid() { 
  d_file_name.set_markup(Dingo::Utilities::highlightString(_("File Name:")));
  d_duration.set_markup(Dingo::Utilities::highlightString(_("Duration:")));
  d_bitrate.set_markup(Dingo::Utilities::highlightString(_("Bitrate:")));
  d_location.set_markup(Dingo::Utilities::highlightString(_("Location:")));
  d_subtitle_path.set_markup(Dingo::Utilities::highlightString(_("Subtitle:")));
  d_file_size.set_markup(Dingo::Utilities::highlightString(_("File Size:")));
  d_date_added.set_markup(Dingo::Utilities::highlightString(_("Date Added:")));
  
  d_file_name.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_duration.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_bitrate.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_location.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_subtitle_path.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_file_size.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_date_added.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  
  d_details_grid.attach(d_file_name, 0, 0, 1, 1);
  d_details_grid.attach(d_file_name_entry, 1, 0, 2, 1);
  d_details_grid.attach(d_duration, 0, 1, 1, 1);
  d_details_grid.attach(d_duration_value, 1, 1, 2, 1);
  d_details_grid.attach(d_bitrate, 0, 2, 1, 1);
  d_details_grid.attach(d_bitrate_value, 1, 2, 2, 1);
  d_details_grid.attach(d_location, 0, 3, 1, 1);
  d_details_grid.attach(d_location_entry, 1, 3, 2, 1);
  d_details_grid.attach(d_subtitle_path, 0, 4, 1, 1);
  d_details_grid.attach(d_subtitle_path_entry, 1, 4, 2, 1);
  d_details_grid.attach(d_file_size, 0, 5, 1, 1);
  d_details_grid.attach(d_file_size_value, 1, 5, 2, 1);
  d_details_grid.attach(d_date_added, 0, 6, 1, 1);
  d_details_grid.attach(d_date_added_value, 1, 6, 2, 1);
  
  d_file_name_entry.set_editable(false);
  d_location_entry.set_editable(false);
  
  d_details_grid.set_row_spacing(7);
  d_details_grid.set_column_spacing(10);
  d_details_grid.set_border_width(10);
  d_details_grid.set_hexpand(true);
  d_details_grid.set_vexpand(true);
  
  d_file_name_entry.set_hexpand(true);
  d_duration_value.set_hexpand(true);
  d_bitrate_value.set_hexpand(true);
  d_location_entry.set_hexpand(true);
  d_subtitle_path_entry.set_hexpand(true);
  d_file_size_value.set_hexpand(true);
  d_date_added_value.set_hexpand(true);
  
  d_duration_value.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_bitrate_value.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_file_size_value.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_date_added_value.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
}
      
void Dingo::TrackInfoWindow::createBasicGrid() {
  d_title.set_markup(Dingo::Utilities::highlightString(_("Title:")));
  d_artist.set_markup(Dingo::Utilities::highlightString(_("Artist:")));
  d_album.set_markup(Dingo::Utilities::highlightString(_("Album:")));
  d_genre.set_markup(Dingo::Utilities::highlightString(_("Genre:")));
  d_track_number.set_markup(Dingo::Utilities::highlightString(_("Track Number:")));
  d_year.set_markup(Dingo::Utilities::highlightString(_("Year:")));
  d_comment.set_markup(Dingo::Utilities::highlightString(_("Comment:")));
  
  d_title.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_artist.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_album.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_genre.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_track_number.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_year.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  d_comment.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
  
  d_comment_textbuffer = Gtk::TextBuffer::create();
  d_comment_textview.set_buffer(d_comment_textbuffer);
  
  d_comment_scrolled.add(d_comment_textview);
  d_comment_scrolled.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  d_comment_scrolled.set_shadow_type(Gtk::SHADOW_OUT);
  d_comment_scrolled.set_vexpand(true);
  
  d_basic_grid.attach(d_title, 0, 0, 1, 1);
  d_basic_grid.attach(d_title_entry, 1, 0 , 2, 1);
  d_basic_grid.attach(d_artist, 0, 1, 1, 1);
  d_basic_grid.attach(d_artist_entry, 1, 1, 2, 1);
  d_basic_grid.attach(d_album, 0, 2, 1, 1);
  d_basic_grid.attach(d_album_entry, 1, 2, 2, 1);
  d_basic_grid.attach(d_genre, 0, 3, 1, 1);
  d_basic_grid.attach(d_genre_entry, 1, 3, 2, 1);
  d_basic_grid.attach(d_track_number, 0, 4, 1, 1);
  d_basic_grid.attach(d_track_number_entry, 1, 4, 2, 1);
  d_basic_grid.attach(d_year, 0, 5, 1, 1);
  d_basic_grid.attach(d_year_entry, 1, 5, 2, 1);
  d_basic_grid.attach(d_comment, 0, 6, 1, 1);
  d_basic_grid.attach(d_comment_scrolled, 1, 6, 2, 1);
  
  d_basic_grid.set_row_spacing(7);
  d_basic_grid.set_column_spacing(10);
  d_basic_grid.set_border_width(10);
  d_basic_grid.set_hexpand(true);
  d_basic_grid.set_vexpand(true);
  
  d_title_entry.set_hexpand(true);
  d_artist_entry.set_hexpand(true);
  d_album_entry.set_hexpand(true);
  d_genre_entry.set_hexpand(true);
  d_track_number_entry.set_hexpand(true);
  d_year_entry.set_hexpand(true);
  d_comment_scrolled.set_hexpand(true);
}

//SIGNAL HANDLERS
void Dingo::TrackInfoWindow::onBackButtonClicked() {
  applyNewTrackInfo();
  
  //refresh Gtk::Main
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  notifyTrackInfoWindowObserver(Dingo::PREVIOUS_TRACK_INFO);
  
  resetOldChangedValuesInfo();
}

void Dingo::TrackInfoWindow::onForwardButtonClicked() {
  applyNewTrackInfo();
  
  //refresh Gtk::Main
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  notifyTrackInfoWindowObserver(Dingo::NEXT_TRACK_INFO);
  
  resetOldChangedValuesInfo();
}

void Dingo::TrackInfoWindow::onCloseButtonClicked() {
  hide();
}

void Dingo::TrackInfoWindow::applyNewTrackInfo() {
  bool values_changed = false;

  Glib::ustring d_title_entry_value = d_title_entry.get_text();
  if (d_cur_title != d_title_entry_value) {
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackTitle", d_title_entry_value));
   
    values_changed = true;
  }
  
  Glib::ustring d_artist_entry_value = d_artist_entry.get_text();
  if (d_cur_artist != d_artist_entry_value) {
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackArtist", d_artist_entry_value));
    
    values_changed = true;
    
    d_artist_value_changed = true;
  }
  
  Glib::ustring d_album_entry_value = d_album_entry.get_text();
  if (d_cur_album != d_album_entry_value) {
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackAlbum", d_album_entry_value));
    
    values_changed = true;
    
    d_album_value_changed = true;
  }
  
  Glib::ustring d_genre_entry_value = d_genre_entry.get_text();
  if (d_cur_genre != d_genre_entry_value) {
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackGenre", d_genre_entry_value));
    
    values_changed = true;
    
    d_genre_value_changed = true;
  }
  
  Glib::ustring d_track_number_entry_value = d_track_number_entry.get_text();
  if (d_cur_track_number != d_track_number_entry_value) {
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackNumber", d_track_number_entry_value));
    
    values_changed = true;
  }
  
  Glib::ustring d_year_entry_value = d_year_entry.get_text();
  if (d_cur_year != d_year_entry_value) {
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackYear", d_year_entry_value));
    
    values_changed = true;
    
    d_year_value_changed = true;
  }
  
  Glib::ustring d_comment_textview_value = d_comment_textview.get_buffer()->get_text();
  if (d_cur_comment != d_comment_textview_value) {
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackComment", d_comment_textview_value));
   
    values_changed = true;
  }
  
  Glib::ustring d_subtitle_path_value = d_subtitle_path_entry.get_text();
  if (d_cur_subtitle_path != d_subtitle_path_value) {
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackSubtitlePath", d_subtitle_path_value));
    
    values_changed = true;
  }
  
  Glib::ustring d_lyrics_textview_value = d_lyrics_textview.get_buffer()->get_text();
  if (d_cur_lyrics != d_lyrics_textview_value) {
    d_changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackLyrics", d_lyrics_textview_value));
  
    values_changed = true;
  }
  
  if (values_changed) {
    d_dbmanager->setTrackInfo(d_cur_edit_row_ref, d_changed_values_map);
    
    Gtk::TreeModel::Row row = *(Dingo::DBManager::trackModel->get_iter(d_dbmanager->getCurrentTrackRowRef().get_path()));
    
    if (row[(*Dingo::DBManager::trackCR).trackID] == d_cur_edit_id) {
      Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Metadata");
    }
  }
}

void Dingo::TrackInfoWindow::resetOldChangedValuesInfo() {
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
