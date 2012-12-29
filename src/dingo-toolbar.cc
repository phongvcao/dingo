/* 
 * dingo-toolbar.cc
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

#include "dingo-toolbar.h"
#include "dingo-statusbar.h"
#include "dingo-dbmanager.h"
#include "dingo-playerengine.h"
#include "dingo-settings.h"
#include "dingo-utilities.h"
#include "dingo-toolbarobserver.h"
#include "dingo-searchentryobserver.h"
#include "dingo-audiovideofilterobserver.h"
#include "dingo-mprisv2.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

#ifndef INCLUDED_GLIBMM_I18N_H
#define INCLUDED_GLIBMM_I18N_H
#include <glibmm/i18n.h>
#endif //INCLUDED_GLIBMM_I18N_H

//CONSTRUCTORS & DESTRUCTOR
Dingo::ToolBar::ToolBar(Dingo::DBManager* dbmanager, Dingo::PlayerEngine* playerengine, Dingo::Settings* settings, Dingo::StatusBar* statusbar) :
  Gtk::HPaned(),
  d_preview_image(playerengine->getPreviewImage()),
  d_toolbar_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0),
  d_search_entry_align(Gtk::ALIGN_END, Gtk::ALIGN_CENTER, 0.0, 0.0)
{
  d_dbmanager = dbmanager;
  d_playerengine = playerengine;
  d_settings = settings;
  d_statusbar = statusbar;

  createPlayingTrackInfo();
  createToggleButtons();
  createSearchEntry();
  
  d_playlist_changed_count = 0;
}

Dingo::ToolBar::~ToolBar() {

}

//PUBLIC METHODS
void Dingo::ToolBar::activateToggleButton(Dingo::ToolBarSignals signal_type) {
  switch (signal_type) {
    case Dingo::HOME_TOGGLED: {
      d_home.set_active(true);
      
      break;
    }
    
    case Dingo::NOW_PLAYING_TOGGLED: {
      d_now_playing.set_active(true);
      break;
    }
    
    case Dingo::PLAY_QUEUE_TOGGLED: {
      d_play_queue.set_active(true);
      
      break;
    }
    
    case Dingo::AUDIO_FILTER_TOGGLED: {
      d_audio_filter.set_active(true);
      break;
    }
    
    case Dingo::VIDEO_FILTER_TOGGLED: {
      d_video_filter.set_active(true);
      break;
    }
    
    default: {
      //std::cout << "Unknown Dingo::ToolBarSignals passed to Dingo::ToolBar::activateToggleButton()!" << std::endl;
      break;
    }
  }
}

void Dingo::ToolBar::deactivateToggleButton(Dingo::ToolBarSignals signal_type) {
  switch (signal_type) {
    case Dingo::HOME_TOGGLED: {
      d_home.set_active(false);
      break;
    }
    
    case Dingo::NOW_PLAYING_TOGGLED: {
      d_now_playing.set_active(false);
      break;
    }
    
    case Dingo::PLAY_QUEUE_TOGGLED: {
      d_play_queue.set_active(false);
      break;
    }
    
    case Dingo::AUDIO_FILTER_TOGGLED: {
      d_audio_filter.set_active(false);
      break;
    }
    
    case Dingo::VIDEO_FILTER_TOGGLED: {
      d_video_filter.set_active(false);
      break;
    }
    
    default: {
      //std::cout << "Unknown Dingo::ToolBarSignals passed to Dingo::ToolBar::deactivateToggleButton()!" << std::endl;
      break;
    }
  }
}

void Dingo::ToolBar::adjustDividerPosition(int total_allocated_width) {
  set_position((total_allocated_width - 355) / 2);
  d_toggle_hpaned.set_position(355);
}

//OBSERVER'S METHODS
void Dingo::ToolBar::registerToolBarObserver(Dingo::ToolBarObserver* observer) {
  toolbar_obs_vector.push_back(observer);
}

void Dingo::ToolBar::removeToolBarObserver(Dingo::ToolBarObserver* observer) {
  std::vector<Dingo::ToolBarObserver*>::iterator iter = std::find(toolbar_obs_vector.begin(), toolbar_obs_vector.end(), observer);
  
  if (iter != toolbar_obs_vector.end()) {
    toolbar_obs_vector.erase(iter);
  }
}

void Dingo::ToolBar::notifyToolBarObserver(Dingo::ToolBarSignals signal_type) {
  switch (signal_type) {
    case Dingo::HOME_TOGGLED: {
      for (std::vector<Dingo::ToolBarObserver*>::iterator iter = toolbar_obs_vector.begin(); iter != toolbar_obs_vector.end(); ++iter) {
        (*iter)->updateHomeToggleActive();
      }
      //std::cout << "Home Toggle Button is active!" << std::endl;
      break;
    }
    
    case Dingo::NOW_PLAYING_TOGGLED: {
      for (std::vector<Dingo::ToolBarObserver*>::iterator iter = toolbar_obs_vector.begin(); iter != toolbar_obs_vector.end(); ++iter) {
        (*iter)->updateNowPlayingToggleActive(); 
      }
      //std::cout << "Now Playing Button is active!" << std::endl;
      break;
    }
    
    case Dingo::PLAY_QUEUE_TOGGLED: {
      for (std::vector<Dingo::ToolBarObserver*>::iterator iter = toolbar_obs_vector.begin(); iter != toolbar_obs_vector.end(); ++iter) {
        (*iter)->updatePlayQueueToggleActive();
      }
      //std::cout << "Play Queue Toggle is active!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Undefined Dingo::ToolBarSignals!" << std::endl;
      break;
    }
  }
}
      
//SEARCH ENTRY'S OBSERVER'S METHODS
void Dingo::ToolBar::registerSearchEntryObserver(Dingo::SearchEntryObserver* observer) {
  search_obs_vector.push_back(observer);
}

void Dingo::ToolBar::removeSearchEntryObserver(Dingo::SearchEntryObserver* observer) {
  std::vector<Dingo::SearchEntryObserver*>::iterator iter = std::find(search_obs_vector.begin(), search_obs_vector.end(), observer);
  
  if (iter != search_obs_vector.end()) {
    search_obs_vector.erase(iter);
  }
}

void Dingo::ToolBar::notifySearchEntryObserver(Dingo::SearchEntrySignals signal_type) {
  switch (signal_type) {
    case Dingo::SEARCH_ENTRY_CHANGED: {
      d_dbmanager->setSearchText(d_search_entry.get_text());
      
      for (std::vector<Dingo::SearchEntryObserver*>::iterator iter = search_obs_vector.begin(); iter != search_obs_vector.end(); ++iter) {
        (*iter)->updateSearchEntryChanged();
      }
      
      //std::cout << "Search Entry was changed!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Undefined Dingo::SearchEntrySignals!" << std::endl;
      break;
    }
  }
}

//AUDIO / VIDEO ONLY OBSERVER'S METHODS
void Dingo::ToolBar::registerAudioVideoFilterObserver(Dingo::AudioVideoFilterObserver* observer) {
  audiovideo_obs_vector.push_back(observer);
}

void Dingo::ToolBar::removeAudioVideoFilterObserver(Dingo::AudioVideoFilterObserver* observer) {
  std::vector<Dingo::AudioVideoFilterObserver*>::iterator iter = std::find(audiovideo_obs_vector.begin(), audiovideo_obs_vector.end(), observer);
  
  if (iter != audiovideo_obs_vector.end()) {
    audiovideo_obs_vector.erase(iter);
  }
}

void Dingo::ToolBar::notifyAudioVideoFilterObserver(Dingo::ToolBarSignals signal_type) {
  switch (signal_type) {
    case Dingo::AUDIO_FILTER_TOGGLED: {
      for (std::vector<Dingo::AudioVideoFilterObserver*>::iterator iter = audiovideo_obs_vector.begin(); iter != audiovideo_obs_vector.end(); ++iter) {
        (*iter)->updateAudioFilterToggled();
      }
      //std::cout << "Audio Filter Toggled!" << std::endl;
      break;
    }
    
    case Dingo::VIDEO_FILTER_TOGGLED: {
      for (std::vector<Dingo::AudioVideoFilterObserver*>::iterator iter = audiovideo_obs_vector.begin(); iter != audiovideo_obs_vector.end(); ++iter) {
        (*iter)->updateVideoFilterToggled();
      }
      //std::cout << "Video Filter Toggled!" << std::endl;
      break;
    }
    
    default: {
      break;
    }
  }
}

//INHERITED OBSERVER'S METHODS
void Dingo::ToolBar::updateTrackTreeRowActivated(Gtk::TreeRowReference cur_track_row_ref) {
  Gtk::TreeModel::Row temp_cur_row = *(Dingo::DBManager::trackModel->get_iter(cur_track_row_ref.get_path()));
  
  if (temp_cur_row[(*Dingo::DBManager::trackCR).trackMIMEType] == "video/*") {
    d_now_playing.set_active(true);
  }
  
  d_preview_image.set(d_playerengine->getPreviewImage());
  
  //set info_area information
  d_track_title.set_markup(Glib::ustring::compose("<span font='Bold 14'>%1</span>", temp_cur_row[(*Dingo::DBManager::trackCR).trackTitle]));
  
  d_track_artist.set_markup(Glib::ustring::compose(_("by %1 from %2"), "<i>" + temp_cur_row[(*Dingo::DBManager::trackCR).trackArtist] + "</i>", "<i>" + temp_cur_row[(*Dingo::DBManager::trackCR).trackAlbum] + "</i>"));
}

void Dingo::ToolBar::updateTrackTreeRowDeactivated() {
  d_track_title.set_markup("<span font='Bold 14'>" + Glib::ustring(_("Not Playing")) + "</span>");
  
  d_track_artist.set_markup("");
  
  d_preview_image.set(d_playerengine->getDefaultPreviewImage());
}

void Dingo::ToolBar::updateSettingsRead() {  
  
}

void Dingo::ToolBar::updateSettingsWrite() {
  d_settings->setActive("home", d_home.get_active());
  d_settings->setActive("nowplaying", d_now_playing.get_active());
  d_settings->setActive("playqueue", d_play_queue.get_active());
  d_settings->setActive("audioonly", d_audio_filter.get_active());
  d_settings->setActive("videoonly", d_video_filter.get_active());
}

//CREATORS
void Dingo::ToolBar::createPlayingTrackInfo() {
  d_preview_image.set_size_request(60, 60);
  d_track_title.set_markup("<span font='Bold 14'>" + Glib::ustring(_("Not Playing")) + "</span>");
  
  d_info_grid.attach(preview_area_left_padding_hbox, 0, 0, 1, 2);
  d_info_grid.attach(d_preview_image, 1, 0, 2, 2);
  d_info_grid.attach(info_left_padding_hbox, 3, 0, 1, 2);
  d_info_grid.attach(d_track_title, 4, 0, 1, 1);
  d_info_grid.attach(d_track_artist, 4, 1, 1, 1);
  
  d_track_title.set_ellipsize(Pango::ELLIPSIZE_END);
  d_track_artist.set_ellipsize(Pango::ELLIPSIZE_END);
  
  d_track_title.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_END);
  d_track_artist.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_START);
  
  d_info_grid.set_row_spacing(5);
  d_info_grid.set_column_spacing(5);
  d_info_grid.show_all();
  
  set_position(360);
  pack1(d_info_grid, Gtk::EXPAND);
}

void Dingo::ToolBar::createToggleButtons() {
  toggleman_vector.push_back(1);
  toggleman_vector.push_back(0);
  toggleman_vector.push_back(0);
  
  d_home.set_active(true);
  
  d_home.set_image(*Gtk::manage(new Gtk::Image(Gtk::Stock::HOME, Gtk::ICON_SIZE_DIALOG)));
  d_now_playing.set_image(*Gtk::manage(new Gtk::Image(DATADIR"/dingo/icons/48x48/dingo-toolbar-now-playing.png")));
  d_play_queue.set_image(*Gtk::manage(new Gtk::Image(DATADIR"/dingo/icons/48x48/dingo-toolbar-play-queue.png")));
  d_audio_filter.set_image(*Gtk::manage(new Gtk::Image(DATADIR"/dingo/icons/48x48/dingo-toolbar-audio-filter.png")));
  d_video_filter.set_image(*Gtk::manage(new Gtk::Image(DATADIR"/dingo/icons/48x48/dingo-toolbar-video-filter.png")));
  
  d_home.set_relief(Gtk::RELIEF_NONE);
  d_now_playing.set_relief(Gtk::RELIEF_NONE);
  d_play_queue.set_relief(Gtk::RELIEF_NONE);
  d_audio_filter.set_relief(Gtk::RELIEF_NONE);
  d_video_filter.set_relief(Gtk::RELIEF_NONE);
  
  d_home.set_tooltip_text(_("Home"));
  d_now_playing.set_tooltip_text(_("Now Playing"));
  d_play_queue.set_tooltip_text(_("Play Queue"));
  d_audio_filter.set_tooltip_text(_("View Audio Only"));
  d_video_filter.set_tooltip_text(_("View Video Only"));
  
  d_home.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::ToolBar::onHomeButtonToggled));
  d_now_playing.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::ToolBar::onNowPlayingButtonToggled));
  d_play_queue.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::ToolBar::onPlayQueueButtonToggled));
  d_audio_filter.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::ToolBar::onAudioFilterToggled));
  d_video_filter.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::ToolBar::onVideoFilterToggled));
  
  Gtk::VSeparator* p_vseparator = Gtk::manage(new Gtk::VSeparator());
  Gtk::HBox* p_vseparator_hbox = Gtk::manage(new Gtk::HBox);
  p_vseparator_hbox->pack_start(*p_vseparator);
  p_vseparator_hbox->set_size_request(5, 30);
   
  d_toolbar.pack_start(d_home, Gtk::PACK_SHRINK);
  d_toolbar.pack_start(d_now_playing, Gtk::PACK_SHRINK);
  d_toolbar.pack_start(d_play_queue, Gtk::PACK_SHRINK);
  d_toolbar.pack_start(*p_vseparator_hbox, Gtk::PACK_SHRINK);
  d_toolbar.pack_start(d_audio_filter, Gtk::PACK_SHRINK);
  d_toolbar.pack_start(d_video_filter, Gtk::PACK_SHRINK);
  
  d_toolbar_align.add(d_toolbar);
  d_toolbar_align.set_size_request(350, 50);
  
  d_toggle_hpaned.set_position(355);
  d_toggle_hpaned.pack1(d_toolbar_align, Gtk::SHRINK);
}

void Dingo::ToolBar::createSearchEntry() {
  d_search_entry.signal_changed().connect(sigc::mem_fun(*this, &Dingo::ToolBar::onSearchEntryChanged));
  
  d_search_entry.set_size_request(300, 30);
  
  d_search_entry.set_icon_from_stock(Gtk::Stock::FIND, Gtk::ENTRY_ICON_PRIMARY);
  d_search_entry.set_placeholder_text(_("Search..."));
  d_search_entry.signal_icon_press().connect(sigc::mem_fun(*this, &Dingo::ToolBar::onIconPressEvent));
  d_search_entry.signal_enter_notify_event().connect(sigc::mem_fun(*this, &Dingo::ToolBar::onSearchEntryEnterNotifyEvent));
  d_search_entry.signal_leave_notify_event().connect(sigc::mem_fun(*this, &Dingo::ToolBar::onSearchEntryLeaveNotifyEvent));
  
  d_search_entry_align.add(d_search_entry);
  d_search_entry_align.set_border_width(6);
  
  d_toggle_hpaned.pack2(d_search_entry_align, Gtk::EXPAND);
  
  pack2(d_toggle_hpaned, Gtk::EXPAND);
}
      
//SIGNAL HANDLERS
void Dingo::ToolBar::onHomeButtonToggled() {
  switch (toggleman_vector[0]) {
    case 0: {
      toggleman_vector[0] = 1;
      
      d_now_playing.set_active(false);
      d_play_queue.set_active(false);
      
      d_dbmanager->setCurrentPlaylistID(0);
      
      notifyToolBarObserver(Dingo::HOME_TOGGLED);
      
      ++d_playlist_changed_count;
      
      if (d_playlist_changed_count >= 1) {
        Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYLISTS, "ActivePlaylist");
      }
      
      //std::cout << "Home Button is now active!" << std::endl;
      break;
    }
    
    case 1: {
      toggleman_vector[0] = 0;
      //std::cout << "Home Button is now inactive!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unknown toggleman_vector[0]'s value!" << std::endl;
      break;
    }
  }
}

void Dingo::ToolBar::onNowPlayingButtonToggled() {
  switch (toggleman_vector[1]) {
    case 0: {
      toggleman_vector[1] = 1;
      
      d_home.set_active(false);
      d_play_queue.set_active(false);
      
      notifyToolBarObserver(Dingo::NOW_PLAYING_TOGGLED);
      //std::cout << "Now Playing Button is now active!" << std::endl;
      break;
    }
    
    case 1: {
      toggleman_vector[1] = 0;
      //std::cout << "Now Playing Button is now inactive!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unknown toggleman_vector[1]'s value!" << std::endl;
      break;
    }
  }
}

void Dingo::ToolBar::onPlayQueueButtonToggled() {
  switch (toggleman_vector[2]) {
    case 0: {
      toggleman_vector[2] = 1;
      
      d_home.set_active(false);
      d_now_playing.set_active(false);
      
      d_dbmanager->setCurrentPlaylistID(1);
      
      notifyToolBarObserver(Dingo::PLAY_QUEUE_TOGGLED);
      
      ++d_playlist_changed_count;
      
      if (d_playlist_changed_count >= 1) {
        Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYLISTS, "ActivePlaylist");
      }
      
      //std::cout << "Play Queue Button is now active!" << std::endl;
      break;
    }
    
    case 1: {
      toggleman_vector[2] = 0;
      //std::cout << "Play Queue Button is now inactive!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unknown toggleman_vector[2]'s value!" << std::endl;
      break;
    }
  }
}

void Dingo::ToolBar::onAudioFilterToggled() {
  if ((d_audio_filter.get_active()) && (d_video_filter.get_active())) {
    d_dbmanager->setMIMETypeFilter("%%");
  }
  
  else if ((d_audio_filter.get_active()) && (!d_video_filter.get_active())) {
    d_dbmanager->setMIMETypeFilter("audio/*");
  }
  
  else if ((!d_audio_filter.get_active()) && (d_video_filter.get_active())) {
    d_dbmanager->setMIMETypeFilter("video/*");
  } 
  
  else if ((!d_audio_filter.get_active()) && (!d_video_filter.get_active())) {
    d_dbmanager->setMIMETypeFilter("%%");
  }
  
  else {
    //std::cout << "Unknown Audio/Video Filter State!" << std::endl;
  }
  
  notifyAudioVideoFilterObserver(Dingo::AUDIO_FILTER_TOGGLED);
}

void Dingo::ToolBar::onVideoFilterToggled() {
  if ((d_audio_filter.get_active()) && (d_video_filter.get_active())) {
    d_dbmanager->setMIMETypeFilter("%%");
  }
  
  else if ((d_audio_filter.get_active()) && (!d_video_filter.get_active())) {
    d_dbmanager->setMIMETypeFilter("audio/*");
  }
  
  else if ((!d_audio_filter.get_active()) && (d_video_filter.get_active())) {
    d_dbmanager->setMIMETypeFilter("video/*");
  } 
  
  else if ((!d_audio_filter.get_active()) && (!d_video_filter.get_active())) {
    d_dbmanager->setMIMETypeFilter("%%");
  }
  
  else {
    //std::cout << "Unknown Audio/Video Filter State!" << std::endl;
  }

  notifyAudioVideoFilterObserver(Dingo::VIDEO_FILTER_TOGGLED);
}
      
void Dingo::ToolBar::onSearchEntryChanged() {
  if (!d_search_entry.get_text().empty()) {
    d_search_entry.set_icon_from_stock(Gtk::Stock::CLEAR, Gtk::ENTRY_ICON_SECONDARY);
  }
  
  else {
    d_search_entry.unset_icon(Gtk::ENTRY_ICON_SECONDARY);
  }
  
  d_search_entry_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &Dingo::ToolBar::onSearchEntryChangedTimeOut), 300);
}

bool Dingo::ToolBar::onSearchEntryChangedTimeOut() {
  notifySearchEntryObserver(Dingo::SEARCH_ENTRY_CHANGED);
  
  return false;
}

void Dingo::ToolBar::onIconPressEvent(Gtk::EntryIconPosition icon_position, const GdkEventButton* event) {
  if ((icon_position == Gtk::ENTRY_ICON_SECONDARY) && (event->button == 1) && (event->type == GDK_BUTTON_PRESS)) {
    d_search_entry.set_text("");
  }
}

bool Dingo::ToolBar::onSearchEntryEnterNotifyEvent(GdkEventCrossing* event) {
  d_search_entry.get_window()->set_cursor(Gdk::Cursor::create(Gdk::HAND1));
}

bool Dingo::ToolBar::onSearchEntryLeaveNotifyEvent(GdkEventCrossing* event) {
  d_search_entry.get_window()->set_cursor(Gdk::Cursor::create(Gdk::LEFT_PTR));
}
