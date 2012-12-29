#ifndef INCLUDED_DINGO_TOOLBAR_H
#define INCLUDED_DINGO_TOOLBAR_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_GDKX_H
#define INCLUDED_GDKX_H
#include <gdk/gdkx.h>
#endif //INCLUDED_GDKX_H

#include "dingo-header.h"
#include "dingo-tracktreeviewobserver.h"
#include "dingo-settingsactionobserver.h"

namespace Dingo {
  class DBManager;
  class PlayerEngine;
  class Settings;
  class ToolBarObserver;
  class SearchEntryObserver;
  class AudioVideoFilterObserver;
  class StatusBar;
  
  class ToolBar : public Gtk::HPaned, public TrackTreeViewObserver, public SettingsActionObserver {
    public:
      ToolBar(DBManager* dbmanager, PlayerEngine* playerengine, Settings* settings, StatusBar* statusbar);
      virtual ~ToolBar();
      
      //PUBLIC METHODS
      void activateToggleButton(ToolBarSignals signal_type);
      void deactivateToggleButton(ToolBarSignals signal_type);
      void adjustDividerPosition(int total_allocated_width);
      
      //OBSERVER'S METHODS
      void registerToolBarObserver(ToolBarObserver* observer);
      void removeToolBarObserver(ToolBarObserver* observer);
      void notifyToolBarObserver(ToolBarSignals signal_type);
      
      //SEARCH ENTRY'S OBSERVER'S METHODS
      void registerSearchEntryObserver(SearchEntryObserver* observer);
      void removeSearchEntryObserver(SearchEntryObserver* observer);
      void notifySearchEntryObserver(SearchEntrySignals signal_type);
      
      //AUDIO / VIDEO ONLY OBSERVER'S METHODS
      void registerAudioVideoFilterObserver(AudioVideoFilterObserver* observer);
      void removeAudioVideoFilterObserver(AudioVideoFilterObserver* observer);
      void notifyAudioVideoFilterObserver(ToolBarSignals signal_type);
      
      //INHERITED OBSERVER'S METHODS
      void updateTrackTreeRowActivated(Gtk::TreeRowReference cur_track_row_ref);
      void updateTrackTreeRowDeactivated();
      
      void updateSettingsRead();
      void updateSettingsWrite();
      
    private:
      //CREATORS
      void createPlayingTrackInfo();
      void createToggleButtons();
      void createSearchEntry();
      
      //SIGNAL HANDLERS
      void onHomeButtonToggled();
      void onNowPlayingButtonToggled();
      void onPlayQueueButtonToggled();
      void onAudioFilterToggled();
      void onVideoFilterToggled();
      
      void onSearchEntryChanged();
      bool onSearchEntryChangedTimeOut();
      void onIconPressEvent(Gtk::EntryIconPosition icon_position, const GdkEventButton* event);
      bool onSearchEntryEnterNotifyEvent(GdkEventCrossing* event);
      bool onSearchEntryLeaveNotifyEvent(GdkEventCrossing* event);
      
      //DATA MEMBERS
      Gtk::HPaned d_toggle_hpaned;
      Gtk::Alignment d_toolbar_align, d_search_entry_align;
      
      Gtk::Grid d_info_grid;
      Gtk::Image d_preview_image;
      Gtk::HBox preview_area_left_padding_hbox, info_left_padding_hbox;
      
      Gtk::Label d_track_title, d_track_artist;
      
      Gtk::HBox d_toolbar;
      Gtk::ToggleButton d_home, d_now_playing, d_play_queue, d_audio_filter, d_video_filter;
      std::vector<int> toggleman_vector;
      int d_pre_play_queue_playlist_id;
      int d_playlist_changed_count;
      
      Gtk::Entry d_search_entry;
      sigc::connection d_search_entry_connection;
      
      std::vector<ToolBarObserver*> toolbar_obs_vector;
      std::vector<SearchEntryObserver*> search_obs_vector;
      std::vector<AudioVideoFilterObserver*> audiovideo_obs_vector;
      
      DBManager* d_dbmanager;
      PlayerEngine* d_playerengine;
      Settings* d_settings;
      StatusBar* d_statusbar;
  };
}

#endif //INCLUDED_DINGO_TOOLBAR_H
