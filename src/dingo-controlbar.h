#ifndef INCLUDED_DINGO_CONTROLBAR_H
#define INCLUDED_DINGO_CONTROLBAR_H

#ifndef INCLUDED_GTKMM_VOLUMEBUTTON_H
#define INCLUDED_GTKMM_VOLUMEBUTTON_H
#include <gtkmm/volumebutton.h>
#endif //INCLUDED_GTKMM_VOLUMEBUTTON_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_MEMORY
#define INCLUDED_MEMORY
#include <memory>
#endif //INCLUDED_MEMORY

#include "dingo-header.h"
#include "dingo-hscale.h"
#include "dingo-repeatmodeobserver.h"
#include "dingo-enginestatusobserver.h"
#include "dingo-fullscreenobserver.h"
#include "dingo-mousemotionobserver.h"
#include "dingo-settingsactionobserver.h"
#include "dingo-repeattoggleobserver.h"

namespace Dingo {
  class DBManager;
  class PlayerEngine;
  class Settings;
  class ControlBar;
  class StatusBar;
  
  /* Declare Dingo::RepeatButton class */
  class RepeatButton : public Gtk::ToggleButton {
    public:
      RepeatButton(PlayerEngine* playerengine, StatusBar* statusbar);
      virtual ~RepeatButton();
      
      //PUBLIC METHODS
      void setRepeatState(RepeatMode repeat_mode, bool is_block_connection = true);
      
    private:
      //CREATORS
      void createRepeatButton();
      void createRepeatMenu();
    
      //SIGNAL HANDLERS
      bool on_button_press_event(GdkEventButton* event);
      void onRepeatMenuItemToggled();
      
      //DATA MEMBERS
      Gtk::Menu d_repeat_menu;
      std::auto_ptr<Gtk::RadioMenuItem> d_repeat_off, d_repeat_single, d_repeat_playlist, d_repeat_section;
      Gtk::RadioMenuItem::Group d_repeat_group;
      Gtk::Image d_repeat_single_image, d_repeat_image;
      
      sigc::connection d_repeat_off_connection, d_repeat_single_connection;
      sigc::connection d_repeat_playlist_connection, d_repeat_section_connection;
      
      PlayerEngine* d_playerengine;
      StatusBar* d_statusbar;
  };


  /* Declare Dingo::ControlBar class */
  class ControlBar : public Gtk::VBox, public RepeatModeObserver, public EngineStatusObserver, public FullscreenObserver, public MouseMotionObserver, public SettingsActionObserver, public RepeatToggleObserver {
    public:
      ControlBar(DBManager* dbmanager, PlayerEngine* playerengine, Settings* settings, StatusBar* statusbar);
      virtual ~ControlBar();
      
      //INHERITED OBSERVER'S METHODS
      void updateRepeatOff();
      void updateRepeatSingle();
      void updateRepeatPlaylist();
      void updateRepeatSection();
      void updateRepeatSectionOff();
      
      void updateEnginePlaying();
      void updateEnginePaused();
      void updateEngineStopped();
      void updateEngineNewPlaying();
      
      void updateFullscreenActivated();
      void updateUnFullscreenActivated();
      void updateMouseHoverOnFullscreen();
      
      void updateSettingsRead();
      void updateSettingsWrite();
      
      void updateRepeatOffToggled();
      void updateRepeatSingleToggled();
      void updateRepeatPlaylistToggled();
      void updateRepeatSectionToggled(Glib::ustring section_button_code = "");
      
      //PUBLIC METHODS
      void setRepeatState(RepeatMode repeat_mode);
      void setShuffleToggle(bool shuffle_state);
      
      //PUBLIC SIGNAL HANDLERS
      void onPlayButtonClicked();
      void onFullscreenButtonClicked();
      void onPreviousButtonClicked();
      void onNextButtonClicked();
      void onPositionButtonToggled();
      void onDurationButtonToggled();
      void onShuffleToggled();
    
    private:
      //CREATORS
      void createControlBar();
      
      //SIGNAL HANDLERS
      bool onHScaleChangeValue(Gtk::ScrollType scroll, double new_value);
      void onVolumeButtonValueChanged(bool value);
      bool onRepeatButtonPressEvent(GdkEventButton* event);
      
      void connectToTimeOut();
      void disconnectFromTimeOut();
      bool onTimeOut();
      
      bool onHidePopupWinTimeOut();
      
      //UTILITIES
      void enableWidgets();
      void disableWidgets();
      
      //DATA MEMBERS
      Gtk::HBox d_controlbar;
      RepeatButton d_repeat_toggle;
      int d_repeat_single_toggle_count;
      Gtk::Button d_play_button, d_prev_button, d_next_button, d_fullscreen_button;
      HScale d_hscale;
      Gtk::Label d_pos_label, d_dur_label;
      Gtk::ToggleButton d_pos_toggle, d_dur_toggle, d_shuffle_toggle;
      bool d_is_pos_toggled_here, d_is_dur_toggled_here;
      Gtk::VolumeButton d_volume_button;
      
      sigc::connection d_connection;
      sigc::connection d_hidepopupwin_connection;
      sigc::connection d_pos_toggle_connection, d_dur_toggle_connection;
      
      Gtk::Image d_play_image, d_pause_image, d_fullscreen_image, d_unfullscreen_image;
      
      PlayerEngine* d_playerengine;
      DBManager* d_dbmanager;
      Settings* d_settings;
      StatusBar* d_statusbar;
      
      //FULLSCREEN-RELATED DATA MEMBERS
      Glib::RefPtr<Gdk::Screen> d_screen;
      Glib::RefPtr<Gdk::Window> d_window;
      Gdk::Rectangle d_rectangle;
      Gtk::Window d_popup_win;
      
      Gtk::Grid d_info_grid;
      Gtk::Image d_preview_image;
      Gtk::HBox preview_area_left_padding_hbox, info_left_padding_hbox;
      Gtk::Label d_track_title, d_track_artist;
  };
}

#endif //INCLUDED_DINGO_CONTROLBAR_H
