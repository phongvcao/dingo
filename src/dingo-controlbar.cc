/* 
 * dingo-controlbar.cc
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

#include "dingo-controlbar.h"
#include "dingo-statusbar.h"
#include "dingo-dbmanager.h"
#include "dingo-playerengine.h"
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

/* Dingo::RepeatButton */
//CONSTRUCTORS - DESTRUCTOR
Dingo::RepeatButton::RepeatButton(Dingo::PlayerEngine* playerengine, Dingo::StatusBar* statusbar) :
  Gtk::ToggleButton(),
  d_repeat_single_image(DATADIR"/dingo/icons/24x24/dingo-controlbar-repeat-single.png"), 
  d_repeat_image(DATADIR"/dingo/icons/24x24/dingo-controlbar-repeat.png")
{
  d_playerengine = playerengine;
  d_statusbar = statusbar;

  createRepeatButton();
  createRepeatMenu();
}

Dingo::RepeatButton::~RepeatButton() {

}

//PUBLIC METHODS
void Dingo::RepeatButton::setRepeatState(Dingo::RepeatMode repeat_mode, bool is_block_connection) {
  if (is_block_connection) {
    d_repeat_off_connection.block();
    d_repeat_single_connection.block();
    d_repeat_playlist_connection.block();
    d_repeat_section_connection.block();
  }

  switch (repeat_mode) {
    case Dingo::REPEAT_OFF: {
      d_repeat_off->set_active(true);
    
      set_active(false);
      set_image(d_repeat_image);
      d_repeat_section->set_label(_("Repeat Section"));
      d_repeat_section->set_sensitive(false);
      
      break;
    }
  
    case Dingo::REPEAT_SINGLE: {
      d_repeat_single->set_active(true);
    
      set_active(true);
      set_image(d_repeat_single_image);
      d_repeat_section->set_label(_("Repeat Section"));
      d_repeat_section->set_sensitive(false);
      
      break;
    }
    
    case Dingo::REPEAT_PLAYLIST: {
      d_repeat_playlist->set_active(true);
    
      set_active(true);
      set_image(d_repeat_image);
      d_repeat_section->set_label(_("Repeat Section"));
      d_repeat_section->set_sensitive(false);
      
      break;
    }
    
    case Dingo::REPEAT_SECTION: {
      d_repeat_section->set_active(true);
  
      set_active(true);
      d_repeat_section->set_label(Glib::ustring::compose(_("Repeat %1 -> %2"), d_playerengine->getSectionStartString(), d_playerengine->getSectionEndString()));
      set_image(d_repeat_image);
      d_repeat_section->set_sensitive(true);
      
      break;
    }
  }
  
  if (is_block_connection) {
    d_repeat_off_connection.unblock();
    d_repeat_single_connection.unblock();
    d_repeat_playlist_connection.unblock();
    d_repeat_section_connection.unblock();
  }
}
      
//CREATORS
void Dingo::RepeatButton::createRepeatButton() {
  set_relief(Gtk::RELIEF_NONE);
  
  set_image(d_repeat_image);
}

void Dingo::RepeatButton::createRepeatMenu() {
  d_repeat_off.reset(new Gtk::RadioMenuItem(d_repeat_group, _("Repeat Off")));
  d_repeat_single.reset(new Gtk::RadioMenuItem(d_repeat_group, _("Repeat Single")));
  d_repeat_playlist.reset(new Gtk::RadioMenuItem(d_repeat_group, _("Repeat Playlist")));
  d_repeat_section.reset(new Gtk::RadioMenuItem(d_repeat_group, _("Repeat Section")));

  d_repeat_menu.append(*d_repeat_off);
  d_repeat_menu.append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
  d_repeat_menu.append(*d_repeat_playlist);
  d_repeat_menu.append(*d_repeat_single);
  d_repeat_menu.append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
  d_repeat_menu.append(*d_repeat_section);
  
  d_repeat_off_connection = d_repeat_off->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::RepeatButton::onRepeatMenuItemToggled));
  d_repeat_playlist_connection = d_repeat_playlist->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::RepeatButton::onRepeatMenuItemToggled));
  d_repeat_single_connection = d_repeat_single->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::RepeatButton::onRepeatMenuItemToggled));
  d_repeat_section_connection = d_repeat_section->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::RepeatButton::onRepeatMenuItemToggled));
  
  d_repeat_section->set_sensitive(false);
  
  d_repeat_menu.show_all();
}
    
//SIGNAL HANDLERS
bool Dingo::RepeatButton::on_button_press_event(GdkEventButton* event) {
  if ((event->type == GDK_BUTTON_PRESS) && (event->button == 1)) {
    d_repeat_menu.popup(event->button, event->time);
    
    return true;
  }
  
  return false;
}

void Dingo::RepeatButton::onRepeatMenuItemToggled() {
  if (d_repeat_off->get_active()) {
    d_playerengine->setRepeatMode(Dingo::REPEAT_OFF);
    
    set_active(false);
    set_image(d_repeat_image);
    d_repeat_section->set_label(_("Repeat Section"));
    d_repeat_section->set_sensitive(false);
  }
  
  else if (d_repeat_single->get_active()) {
    d_playerengine->setRepeatMode(Dingo::REPEAT_SINGLE);
    
    set_active(true);
    set_image(d_repeat_single_image);
    d_repeat_section->set_label(_("Repeat Section"));
    d_repeat_section->set_sensitive(false);
  }
  
  else if (d_repeat_playlist->get_active()) {
    d_playerengine->setRepeatMode(Dingo::REPEAT_PLAYLIST);
    
    set_active(true);
    set_image(d_repeat_image);
    d_repeat_section->set_label(_("Repeat Section"));
    d_repeat_section->set_sensitive(false);
  }
  
  else if (d_repeat_section->get_active()) {
    d_playerengine->notifyRepeatToggleObserver(Dingo::REPEAT_SECTION);
  
    set_active(true);
    set_image(d_repeat_image);
    d_repeat_section->set_sensitive(true);
  }
  
  else {
    //std::cout << "Unknown or wrong Repeat RadioMenuItem state!" << std::endl;
  }
  
  d_repeat_menu.popdown();
}

/* Dingo::RepeatButton Ended */


/* Dingo::ControlBar */
//CONSTRUCTORS - DESTRUCTOR
Dingo::ControlBar::ControlBar(Dingo::DBManager* dbmanager, Dingo::PlayerEngine* playerengine, Dingo::Settings* settings, Dingo::StatusBar* statusbar) :
  Gtk::VBox(),
  d_popup_win(Gtk::WINDOW_POPUP),
  d_repeat_toggle(playerengine, statusbar),
  d_play_image(Gtk::Stock::MEDIA_PLAY, Gtk::ICON_SIZE_BUTTON), 
  d_pause_image(Gtk::Stock::MEDIA_PAUSE, Gtk::ICON_SIZE_BUTTON), 
  d_fullscreen_image(Gtk::Stock::FULLSCREEN, Gtk::ICON_SIZE_BUTTON), 
  d_unfullscreen_image(Gtk::Stock::LEAVE_FULLSCREEN, Gtk::ICON_SIZE_BUTTON)
{
  d_repeat_single_toggle_count = 0;
  d_is_pos_toggled_here = false;
  d_is_dur_toggled_here = false;
  
  d_playerengine = playerengine;
  d_dbmanager = dbmanager;
  d_settings = settings;
  d_statusbar = statusbar;
  
  createControlBar();
}

Dingo::ControlBar::~ControlBar() {

}
      
//INHERITED OBSERVER'S METHODS
void Dingo::ControlBar::updateRepeatOff() {
  d_play_button.set_image(d_play_image);
  
  disconnectFromTimeOut();
  
  d_playerengine->setStatus(Dingo::ENGINE_STOPPED);
  
  d_pos_label.set_markup("0:00 | 0:00");
  d_dur_label.set_markup("0:00 | 0:00");
}

void Dingo::ControlBar::updateRepeatSingle() {
  d_play_button.set_image(d_pause_image);
  
  connectToTimeOut();
}

void Dingo::ControlBar::updateRepeatPlaylist() {
  d_play_button.set_image(d_pause_image);
  
  connectToTimeOut();
}

void Dingo::ControlBar::updateRepeatSection() {
  d_play_button.set_image(d_pause_image);
  
  connectToTimeOut();
}

void Dingo::ControlBar::updateRepeatSectionOff() {
  d_pos_toggle_connection.block();
  d_dur_toggle_connection.block();
  
  d_pos_toggle.set_active(false);
  
  d_playerengine->setSectionStart(0);
  
  d_dur_toggle.set_active(false);
  
  d_playerengine->setSectionEnd(1);
  
  d_pos_toggle_connection.unblock();
  d_dur_toggle_connection.unblock();
}
      
void Dingo::ControlBar::updateEnginePlaying() {
  d_play_button.set_image(d_pause_image);
  
  enableWidgets();
  
  connectToTimeOut();
}

void Dingo::ControlBar::updateEnginePaused() {
  d_play_button.set_image(d_play_image);
  
  enableWidgets();
  
  connectToTimeOut();
}

void Dingo::ControlBar::updateEngineStopped() {
  d_play_button.set_image(d_play_image);
  
  disableWidgets();
  
  d_hscale.set_value(0);
  
  d_pos_label.set_markup("0:00 | 0:00");
  d_dur_label.set_markup("0:00 | 0:00");
  
  disconnectFromTimeOut();
}

void Dingo::ControlBar::updateEngineNewPlaying() {
  updateRepeatSectionOff();
  
  if (d_playerengine->getRepeatMode() == Dingo::REPEAT_SECTION) {
    d_playerengine->setRepeatMode(Dingo::REPEAT_OFF);
    d_repeat_toggle.setRepeatState(Dingo::REPEAT_OFF);
  }
  
  enableWidgets();
}

void Dingo::ControlBar::updateFullscreenActivated() {
  int controlbar_width, controlbar_height;
  
  Gtk::Window* temp_parent_window = d_dbmanager->getWindow();
  
  d_screen = temp_parent_window->get_screen();
  d_window = temp_parent_window->get_window();
  
  d_screen->get_monitor_geometry(d_screen->get_monitor_at_window(d_window), d_rectangle);
  
  controlbar_height = d_controlbar.get_height();
  
  d_popup_win.resize(d_rectangle.get_width(), controlbar_height);
  
  d_popup_win.move(d_rectangle.get_x(), d_rectangle.get_height() + d_rectangle.get_y() - controlbar_height);
  
  //please remember to add() controlbar to d_popup_win here
  remove(d_controlbar);
  
  d_popup_win.add(d_controlbar);
  
  d_fullscreen_button.set_image(d_unfullscreen_image);
  
  d_dbmanager->getWindow()->fullscreen();
  
  d_popup_win.hide();
}

void Dingo::ControlBar::updateUnFullscreenActivated() {
  d_popup_win.remove();
  
  d_fullscreen_button.set_image(d_fullscreen_image);
  
  pack_start(d_controlbar, Gtk::PACK_SHRINK);
  
  d_dbmanager->getWindow()->unfullscreen();
  
  d_popup_win.hide();
}

void Dingo::ControlBar::updateMouseHoverOnFullscreen() {
  d_popup_win.show_all();
    
  if (d_hidepopupwin_connection.connected()) {
    d_hidepopupwin_connection.disconnect();
  }
    
  d_hidepopupwin_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &Dingo::ControlBar::onHidePopupWinTimeOut), 8000);
}

void Dingo::ControlBar::updateSettingsRead() {
  //set repeat mode
  d_repeat_toggle.setRepeatState(d_settings->getRepeat(), false);
  
  //set shuffle mode
  switch (d_settings->getPlayOrder()) {
    case Dingo::PLAYORDER_NORMAL: {
      setShuffleToggle(false);
    
      break;
    }
    
    case Dingo::PLAYORDER_SHUFFLE: {
      setShuffleToggle(true);
    
      break;
    }
    
    default: {
      break;
    }
  }
  
  //set volume
  d_volume_button.set_value(d_settings->getVolume());
}

void Dingo::ControlBar::updateSettingsWrite() {
  //write repeat mode
  d_settings->setRepeat(d_playerengine->getRepeatMode());
  
  //write playorder
  d_settings->setPlayOrder(d_playerengine->getPlayOrder());
  
  //write volume
  d_settings->setVolume(d_volume_button.get_value());
}

void Dingo::ControlBar::updateRepeatOffToggled() {
  d_pos_toggle_connection.block();
  d_dur_toggle_connection.block();
  
  d_pos_toggle.set_active(false);
  d_dur_toggle.set_active(false);
  
  d_pos_toggle_connection.unblock();
  d_dur_toggle_connection.unblock();
}

void Dingo::ControlBar::updateRepeatSingleToggled() {
  d_pos_toggle_connection.block();
  d_dur_toggle_connection.block();
  
  d_pos_toggle.set_active(false);
  d_dur_toggle.set_active(false);
  
  d_pos_toggle_connection.unblock();
  d_dur_toggle_connection.unblock();
}

void Dingo::ControlBar::updateRepeatPlaylistToggled() {
  d_pos_toggle_connection.block();
  d_dur_toggle_connection.block();
  
  d_pos_toggle.set_active(false);
  d_dur_toggle.set_active(false);
  
  d_pos_toggle_connection.unblock();
  d_dur_toggle_connection.unblock();
}

void Dingo::ControlBar::updateRepeatSectionToggled(Glib::ustring section_button_code) {
  if (section_button_code == "section-start") {
    if (!d_is_pos_toggled_here) {
      d_pos_toggle_connection.block();   
      
      d_pos_toggle.set_active(!d_pos_toggle.get_active());
    
      if (d_pos_toggle.get_active()) {   
        d_repeat_toggle.setRepeatState(Dingo::REPEAT_SECTION);
      }
  
      else {
        if (!d_dur_toggle.get_active()) {
          d_repeat_toggle.setRepeatState(Dingo::REPEAT_OFF);
        }
      
        else {
          d_repeat_toggle.setRepeatState(Dingo::REPEAT_SECTION);
        }
      }
    
      d_pos_toggle_connection.unblock();
    }
  }
  
  else if (section_button_code == "section-end") {
    if (!d_is_dur_toggled_here) {
      d_dur_toggle_connection.block();
      
      d_dur_toggle.set_active(!d_dur_toggle.get_active());
    
      if (d_dur_toggle.get_active()) {
        d_repeat_toggle.setRepeatState(Dingo::REPEAT_SECTION);
      }
  
      else {
        if (!d_pos_toggle.get_active()) {
          d_repeat_toggle.setRepeatState(Dingo::REPEAT_OFF);
        }
    
        else {
          d_repeat_toggle.setRepeatState(Dingo::REPEAT_SECTION);
        }
      }
    
      d_dur_toggle_connection.unblock();
    }
  }
}

//PUBLIC METHODS
void Dingo::ControlBar::setRepeatState(Dingo::RepeatMode repeat_mode) {
  d_repeat_toggle.setRepeatState(repeat_mode);
} 

void Dingo::ControlBar::setShuffleToggle(bool shuffle_state) {
  d_shuffle_toggle.set_active(shuffle_state);
}
    
//CREATORS
void Dingo::ControlBar::createControlBar() {
  //set images of all buttons
  d_play_button.set_image(d_play_image);
  d_prev_button.set_image(*(Gtk::manage(new Gtk::Image(Gtk::Stock::MEDIA_PREVIOUS, Gtk::ICON_SIZE_BUTTON))));
  d_next_button.set_image(*(Gtk::manage(new Gtk::Image(Gtk::Stock::MEDIA_NEXT, Gtk::ICON_SIZE_BUTTON))));
  d_fullscreen_button.set_image(d_fullscreen_image);
  d_shuffle_toggle.set_image(*Gtk::manage(new Gtk::Image(DATADIR"/dingo/icons/24x24/dingo-controlbar-shuffle.png")));
  
  d_pos_label.set_markup("0:00 | 0:00");
  d_dur_label.set_markup("0:00 | 0:00");
  d_pos_toggle.add(d_pos_label);
  d_dur_toggle.add(d_dur_label);
  
  //set relief of all buttons
  d_play_button.set_relief(Gtk::RELIEF_NONE);
  d_fullscreen_button.set_relief(Gtk::RELIEF_NONE);
  d_prev_button.set_relief(Gtk::RELIEF_NONE);
  d_next_button.set_relief(Gtk::RELIEF_NONE);
  d_volume_button.set_relief(Gtk::RELIEF_NONE);
  d_shuffle_toggle.set_relief(Gtk::RELIEF_NONE);

  d_volume_button.set_size_request(24, 24);
  d_volume_button.set_value(0.75);
  
  //connect buttons to signals
  d_play_button.signal_clicked().connect(sigc::mem_fun(*this, &Dingo::ControlBar::onPlayButtonClicked));
  d_fullscreen_button.signal_clicked().connect(sigc::mem_fun(*this, &Dingo::ControlBar::onFullscreenButtonClicked));
  d_prev_button.signal_clicked().connect(sigc::mem_fun(*this, &Dingo::ControlBar::onPreviousButtonClicked));
  d_next_button.signal_clicked().connect(sigc::mem_fun(*this, &Dingo::ControlBar::onNextButtonClicked));
  d_pos_toggle_connection = d_pos_toggle.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::ControlBar::onPositionButtonToggled));
  d_dur_toggle_connection = d_dur_toggle.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::ControlBar::onDurationButtonToggled));
  d_shuffle_toggle.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::ControlBar::onShuffleToggled));
  
  d_volume_button.signal_value_changed().connect(sigc::mem_fun(*this, &Dingo::ControlBar::onVolumeButtonValueChanged));
  
  //connect d_hscale to signals
  d_hscale.set_draw_value(false);
  d_hscale.signal_change_value().connect(sigc::mem_fun(*this, &Dingo::ControlBar::onHScaleChangeValue));
  
  //push_back buttons into container
  d_controlbar.pack_start(d_prev_button, Gtk::PACK_SHRINK);
  d_controlbar.pack_start(d_play_button, Gtk::PACK_SHRINK);
  d_controlbar.pack_start(d_next_button, Gtk::PACK_SHRINK);
  d_controlbar.pack_start(d_pos_toggle, Gtk::PACK_SHRINK);
  d_controlbar.pack_start(d_hscale, Gtk::PACK_EXPAND_WIDGET);
  d_controlbar.pack_start(d_dur_toggle, Gtk::PACK_SHRINK);
  d_controlbar.pack_start(d_fullscreen_button, Gtk::PACK_SHRINK);
  Gtk::VSeparator* first_vseparator = Gtk::manage(new Gtk::VSeparator());
  d_controlbar.pack_start(*first_vseparator, Gtk::PACK_SHRINK);
  d_controlbar.pack_start(d_repeat_toggle, Gtk::PACK_SHRINK);
  d_controlbar.pack_start(d_shuffle_toggle, Gtk::PACK_SHRINK);
  Gtk::VSeparator* second_vseparator = Gtk::manage(new Gtk::VSeparator());
  d_controlbar.pack_start(*second_vseparator, Gtk::PACK_SHRINK);
  d_controlbar.pack_start(d_volume_button, Gtk::PACK_SHRINK);
  
  disableWidgets();
  
  //pack_start d_controlbar to the Dingo::ControlBar Gtk::VBox
  pack_start(d_controlbar, Gtk::PACK_SHRINK);
  
  first_vseparator->set_size_request(5, 5);
  second_vseparator->set_size_request(5, 5);
}
      
//SIGNAL HANDLERS
void Dingo::ControlBar::onPlayButtonClicked() {
  d_playerengine->notifyEngineNavigationObserver(Dingo::ENGINE_TRACK_CURRENT);

  if (d_playerengine->getStatus() == Dingo::ENGINE_PAUSED) {
    disconnectFromTimeOut();
  }
  
  else {
    connectToTimeOut();
  }
}

void Dingo::ControlBar::onFullscreenButtonClicked() {
  if (!d_playerengine->getFullscreen()) {
    d_playerengine->setFullscreen(true);
  }
  
  else {
    d_playerengine->setFullscreen(false);
  }
}

void Dingo::ControlBar::onPreviousButtonClicked() {
  d_playerengine->previous();
  
  connectToTimeOut();
}

void Dingo::ControlBar::onNextButtonClicked() {
  d_playerengine->next();
  
  connectToTimeOut();
}

void Dingo::ControlBar::onPositionButtonToggled() {
  d_is_pos_toggled_here = true;

  if (d_pos_toggle.get_active()) {
    d_playerengine->setSectionStart(d_playerengine->getPercentage());
    
    d_playerengine->setRepeatMode(Dingo::REPEAT_SECTION, "section-start");
    
    d_playerengine->notifyRepeatModeObserver(Dingo::REPEAT_SECTION);
      
    d_repeat_toggle.setRepeatState(Dingo::REPEAT_SECTION);
  }
  
  else {
    d_playerengine->setSectionStart(0);
    
    if (!d_dur_toggle.get_active()) {
      d_playerengine->setRepeatMode(Dingo::REPEAT_OFF);
      
      d_repeat_toggle.setRepeatState(Dingo::REPEAT_OFF);
    }
    
    else {
      d_repeat_toggle.setRepeatState(Dingo::REPEAT_SECTION);
      
      d_playerengine->setRepeatMode(Dingo::REPEAT_SECTION, "section-start");
    }
  }
  
  d_is_pos_toggled_here = false;
}

void Dingo::ControlBar::onDurationButtonToggled() {
  d_is_dur_toggled_here = true;

  if (d_dur_toggle.get_active()) {
    d_playerengine->setSectionEnd(d_playerengine->getPercentage());
    
    d_playerengine->setRepeatMode(Dingo::REPEAT_SECTION, "section-end");
    
    d_playerengine->notifyRepeatModeObserver(Dingo::REPEAT_SECTION);
    
    d_repeat_toggle.setRepeatState(Dingo::REPEAT_SECTION);
    
    d_playerengine->setPercentage(d_playerengine->getSectionStart());
  }
  
  else {
    d_playerengine->setSectionEnd(1);
    
    if (!d_pos_toggle.get_active()) {
      d_playerengine->setRepeatMode(Dingo::REPEAT_OFF);
      
      d_repeat_toggle.setRepeatState(Dingo::REPEAT_OFF);
    }
    
    else {
      d_repeat_toggle.setRepeatState(Dingo::REPEAT_SECTION);
      
      d_playerengine->setRepeatMode(Dingo::REPEAT_SECTION, "section-end");
    }
  }
  
  d_is_dur_toggled_here = false;
}

void Dingo::ControlBar::onShuffleToggled() {  
  if (d_shuffle_toggle.get_active()) {
    d_playerengine->setPlayOrder(Dingo::PLAYORDER_SHUFFLE);
  }
  
  else {
    d_playerengine->setPlayOrder(Dingo::PLAYORDER_NORMAL);
  }
}

bool Dingo::ControlBar::onHScaleChangeValue(Gtk::ScrollType scroll, double new_value) {
  d_playerengine->setPercentage(new_value);
  
  return false;
}

void Dingo::ControlBar::onVolumeButtonValueChanged(bool value) {
  d_playerengine->setVolume(value);
}
      
void Dingo::ControlBar::connectToTimeOut() {
  if (d_connection.connected()) {
    d_connection.disconnect();
  }

  d_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &Dingo::ControlBar::onTimeOut), 200);
}

void Dingo::ControlBar::disconnectFromTimeOut() {
  if (d_connection.connected()) {
    d_connection.disconnect();
  }
}

bool Dingo::ControlBar::onTimeOut() {
  if ((d_pos_toggle.get_active()) || (d_dur_toggle.get_active())) {
    if (d_playerengine->getPercentage() >= d_playerengine->getSectionEnd()) {
      d_playerengine->setPercentage(d_playerengine->getSectionStart());
    }
    
    if (d_playerengine->getPercentage() < d_playerengine->getSectionStart()) {
      d_playerengine->setPercentage(d_playerengine->getSectionStart());
    }
  }

  d_hscale.set_value(d_playerengine->getPercentage());
  
  Glib::ustring temp_pos_str = d_playerengine->getPositionString() + " | " + d_playerengine->getSectionStartString();
  Glib::ustring temp_dur_str = d_playerengine->getSectionEndString() + " | " + d_playerengine->getDurationString();
  
  if (d_pos_toggle.get_active()) {
    temp_pos_str = Dingo::Utilities::highlightString(temp_pos_str);
  }
  
  if (d_dur_toggle.get_active()) {
    temp_dur_str = Dingo::Utilities::highlightString(temp_dur_str);
  }
  
  d_pos_label.set_markup(temp_pos_str);
  d_dur_label.set_markup(temp_dur_str);
  
  Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Position");
  
  return true;
}
      
bool Dingo::ControlBar::onHidePopupWinTimeOut() {
  d_popup_win.hide();
  
  return false;
}
      
//UTILITIES
void Dingo::ControlBar::enableWidgets() {
  d_pos_toggle.set_sensitive(true);
  d_dur_toggle.set_sensitive(true);
  d_hscale.set_sensitive(true);
}

void Dingo::ControlBar::disableWidgets() {
  d_pos_toggle.set_sensitive(false);
  d_dur_toggle.set_sensitive(false);
  d_hscale.set_sensitive(false);
}
