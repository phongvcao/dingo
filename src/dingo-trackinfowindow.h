/* 
 * dingo-application.cc
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

#ifndef INCLUDED_DINGO_TRACKINFOWINDOW_H
#define INCLUDED_DINGO_TRACKINFOWINDOW_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#include "dingo-header.h"

namespace Dingo {
  class DBManager;
  class Settings;
  class TrackInfoWindowObserver; //mainly for Dingo::TrackTreeView - Dingo::ArtistTreeView & Dingo::AlbumTreeView will be notified by DBManager

  class TrackInfoWindow : public Gtk::Window {
    public:
      TrackInfoWindow();
      TrackInfoWindow(DBManager* dbmanager, Settings* settings);
      virtual ~TrackInfoWindow();
      
      //OBSERVER'S METHODS
      void registerTrackInfoWindowObserver(TrackInfoWindowObserver* observer);
      void removeTrackInfoWindowObserver(TrackInfoWindowObserver* observer);
      void notifyTrackInfoWindowObserver(TrackInfoWindowSignals signal_type);
      
      //PUBLIC METHODS
      void setDisplayTrack(Gtk::TreeRowReference cur_edit_row_ref);
      void setCurrentPage(int page_index);
    
    private:
      //CREATORS
      void createTopVBox();
      void createButtonsHBox();
      void createNoteBook();
      void createLyricsTextView();
      void createDetailsGrid();
      void createBasicGrid();
      
      //SIGNAL HANDLERS
      void onBackButtonClicked();
      void onForwardButtonClicked();
      void onCloseButtonClicked();
      
      void applyNewTrackInfo();
      void resetOldChangedValuesInfo();
    
      //DATA MEMBERS
      Glib::ustring d_cur_title, d_cur_artist, d_cur_album, d_cur_genre, d_cur_track_number, d_cur_year, d_cur_comment, d_cur_lyrics, d_cur_subtitle_path;
      
      Gtk::Notebook d_notebook;
      Gtk::Grid d_basic_grid, d_details_grid;
      Gtk::HBox d_buttons_hbox;
      Gtk::VBox top_vbox;
      Gtk::HBox d_separator_hbox;
      
      Gtk::Label d_title, d_artist, d_album, d_genre, d_track_number, d_year, d_comment;
      Gtk::Entry d_title_entry, d_artist_entry, d_album_entry, d_genre_entry, d_track_number_entry, d_year_entry;
      
      Gtk::TextView d_comment_textview;
      Gtk::ScrolledWindow d_comment_scrolled;
      Glib::RefPtr<Gtk::TextBuffer> d_comment_textbuffer;
      
      Gtk::Label d_subtitle_path, d_file_name, d_duration, d_bitrate;
      Gtk::Label d_location, d_file_size, d_date_added, d_date_added_value;
      Gtk::Label d_duration_value, d_bitrate_value, d_file_size_value;
      Gtk::Entry d_file_name_entry, d_location_entry, d_subtitle_path_entry;
      
      //'Lyrics' Tab
      Gtk::TextView d_lyrics_textview;
      Gtk::ScrolledWindow d_lyrics_scrolled;
      Glib::RefPtr<Gtk::TextBuffer> d_lyrics_textbuffer;
      
      Gtk::Button d_back_button, d_forward_button, d_close_button;
      
      DBManager* d_dbmanager;
      Settings* d_settings;
      
      Gtk::TreeRowReference d_cur_edit_row_ref;
      std::map<Glib::ustring, Glib::ustring> d_changed_values_map;
      int d_cur_edit_id;
      std::vector<TrackInfoWindowObserver*> infowindow_obs_vector;
      
      bool d_artist_value_changed, d_album_value_changed, d_genre_value_changed, d_year_value_changed;
  };
}

#endif //INCLUDED_DINGO_TRACKINFOWINDOW_H
