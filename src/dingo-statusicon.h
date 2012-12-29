#ifndef INCLUDED_DINGO_STATUSICON_H
#define INCLUDED_DINGO_STATUSICON_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_MEMORY
#define INCLUDED_MEMORY
#include <memory>
#endif //INCLUDED_MEMORY

#include "dingo-hscale.h"
#include "dingo-enginestatusobserver.h"
#include "dingo-playorderobserver.h"
#include "dingo-repeattoggleobserver.h"
#include "dingo-tracktreeviewobserver.h"

namespace Dingo {
  class DBManager;
  class PlayerEngine;
  class ControlBar;

  class StatusIcon : public Gtk::StatusIcon, public EngineStatusObserver, public PlayOrderObserver, public RepeatToggleObserver, public TrackTreeViewObserver {
    public:
      StatusIcon(DBManager* dbmanager, PlayerEngine* playerengine, Gtk::Application* dingo_app);
      virtual ~StatusIcon();
      
      //PUBLIC ACCESSORS
      void setControlBar(ControlBar* controlbar);
      void setShowMainWindowToggle(bool is_active);
      
      void raise();

      //INHERITED OBSERVER'S METHODS
      void updateEnginePlaying();
      void updateEnginePaused();
      void updateEngineStopped();
      void updateEngineNewPlaying();
      
      void updatePlayOrderNormal();
      void updatePlayOrderShuffle();
      
      void updateRepeatOffToggled();
      void updateRepeatSingleToggled();
      void updateRepeatPlaylistToggled();
      void updateRepeatSectionToggled(Glib::ustring section_button_code = "");
      
      void updateShowMainWindow(bool is_mainwindow_showing);
      void updateTrackTreeRowActivated(Gtk::TreeRowReference cur_track_row_ref);
      void updateTrackTreeRowDeactivated();
    
    private:
      //CREATORS
      void createStatusIcon();
      void createPopupMenu();
      void createMiniPlayer();
      
      //SIGNAL HANDLERS
      void onPlayToggled();
      void onNextActivated();
      void onPreviousActivated();
      void onRepeatModeToggled();
      void onShuffleToggled();
      void onShowWindowToggled();
      void onAboutActivated();
      void onQuitActivated();
      
      void onPositionButtonToggled();
      void onDurationButtonToggled();
      bool onMiniPlayerHScaleChangeValue(Gtk::ScrollType scroll, double new_value);
      
      bool on_button_press_event(GdkEventButton* event);
      bool on_query_tooltip(int x, int y, bool keyboard_mode, const Glib::RefPtr<Gtk::Tooltip>& tooltip);
      
      //UTILITIES
      void connectToMiniPlayerTimeOut();
      void disconnectFromMiniPlayerTimeOut();
      bool onMiniPlayerTimeOut();
      
      bool onMiniPlayerHideTimeOut();
      
      void enableWidgets();
      void disableWidgets();
    
      //DATA MEMBERS
      Gtk::Menu status_popup_menu, status_repeat_submenu;
      Gtk::CheckMenuItem status_play;
      Gtk::ImageMenuItem status_next, status_previous;
      Gtk::ImageMenuItem status_repeat;
      std::auto_ptr<Gtk::RadioMenuItem> status_repeat_off, status_repeat_single, status_repeat_playlist, status_repeat_section;
      Gtk::RadioMenuItem::Group status_repeat_group;
      Gtk::CheckMenuItem status_shuffle;
      Gtk::CheckMenuItem status_show_window;
      Gtk::ImageMenuItem status_about;
      Gtk::ImageMenuItem status_quit;
      
      sigc::connection status_play_connection;
      sigc::connection status_shuffle_connection;
      sigc::connection status_repeat_off_connection;
      sigc::connection status_repeat_single_connection;
      sigc::connection status_repeat_playlist_connection;
      sigc::connection status_repeat_section_connection;
      sigc::connection status_show_window_connection;
      
      bool is_play_toggled_here;
      bool is_repeat_toggled_here;
      bool is_playorder_toggled_here;
      bool is_pos_toggled_here;
      bool is_dur_toggled_here;
      
      DBManager* d_dbmanager;
      PlayerEngine* d_playerengine;
      ControlBar* d_controlbar;
      Gtk::Application* d_dingo_app;
      
      //MINI-PLAYER POPUP WINDOW
      Gtk::Window d_miniplayer_win;
      Gtk::HBox d_miniplayer_control_hbox;
      Gtk::VBox d_miniplayer_top_vbox;
      Gtk::ToggleButton d_miniplayer_pos_toggle, d_miniplayer_dur_toggle;
      HScale d_miniplayer_hscale;
      
      Gtk::Grid d_miniplayer_info_grid;
      Gtk::Image d_miniplayer_preview_image;
      Gtk::HBox d_miniplayer_image_left_padding_hbox, d_miniplayer_info_left_padding_hbox;
      
      Gtk::Label d_miniplayer_track_title, d_miniplayer_track_artist;
      Gtk::Label d_miniplayer_pos_label, d_miniplayer_dur_label;
      
      sigc::connection d_miniplayer_pos_connection, d_miniplayer_dur_connection;
      sigc::connection d_miniplayer_timeout_connection;
      sigc::connection d_miniplayer_hide_connection;
      
      bool d_should_miniplayer_visible;
      bool d_is_right_clicked;
      
      Glib::ustring d_marked_up_track_title_str;
      Glib::ustring d_marked_up_track_artist_str;
  };
}

#endif //INCLUDED_DINGO_STATUSICON_H
