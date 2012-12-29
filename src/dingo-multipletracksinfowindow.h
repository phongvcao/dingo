#ifndef INCLUDED_DINGO_MULTIPLETRACKSINFOWINDOW_H
#define INCLUDED_DINGO_MULTIPLETRACKSINFOWINDOW_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#include "dingo-header.h"

namespace Dingo {
  class DBManager;
  class Settings;

  class MultipleTracksInfoWindow : public Gtk::Window {
    public:
      MultipleTracksInfoWindow();
      MultipleTracksInfoWindow(DBManager* dbmanager, Settings* settings);
      virtual ~MultipleTracksInfoWindow();
      
      //PUBLIC METHODS
      void setDisplayTracks(std::vector<Gtk::TreeRowReference> edit_row_vector);
    
    private:
      //CREATORS
      void createTopVBox();
      void createButtonsHBox();
      void createNoteBook();
      void createMainGrid();
      
      //SIGNAL HANDLERS
      void onSaveButtonClicked();
      void onCancelButtonClicked();
      
      void applyNewTracksInfo();
      void resetOldChangedValuesInfo();
      
      //DATA MEMBERS
      Gtk::Grid d_main_grid;
      Gtk::Label d_title_label, d_artist_label, d_album_label, d_genre_label, d_track_number_label, d_year_label, d_duration_label, d_size_label, d_comment_label;
      Gtk::TextView d_comment_textview;
      Glib::RefPtr<Gtk::TextBuffer> d_comment_textbuffer;
      Gtk::ScrolledWindow d_comment_scrolled;
      Gtk::Entry d_title_entry, d_artist_entry, d_album_entry, d_genre_entry, d_track_number_entry, d_year_entry;
      Gtk::Label d_duration_value, d_size_value;
      
      Gtk::Notebook d_notebook;
      Gtk::HBox separator_hbox;
      
      Gtk::VBox top_vbox;
      Gtk::HBox buttons_hbox;
      Gtk::Button d_cancel_button, d_save_button;
      
      Glib::ustring d_cur_title, d_cur_artist, d_cur_album, d_cur_genre, d_cur_track_number, d_cur_year, d_cur_comment;
      
      DBManager* d_dbmanager;
      Settings* d_settings;
      std::vector<Gtk::TreeRowReference> cur_edit_row_vector;
      std::map<Glib::ustring, Glib::ustring> d_changed_values_map;
      bool d_artist_value_changed, d_album_value_changed, d_genre_value_changed, d_year_value_changed;
  };
}

#endif //INCLUDED_DINGO_MULTIPLETRACKSINFOWINDOW_H
