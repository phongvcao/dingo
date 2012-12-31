/* 
 * dingo-playerengine.cc
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

#include "dingo-playerengine.h"
#include "dingo-repeatmodeobserver.h"
#include "dingo-enginestatusobserver.h"
#include "dingo-enginenavigationobserver.h"
#include "dingo-fullscreenobserver.h"
#include "dingo-playorderobserver.h"
#include "dingo-repeattoggleobserver.h"
#include "dingo-utilities.h"
#include "dingo-mprisv2.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

#ifndef INCLUDED_MEDIAINFO_H
#define INCLUDED_MEDIAINFO_H
#include "MediaInfo/MediaInfo.h"
#endif //INCLUDED_MEDIAINFO_H

//CONSTRUCTORS - DESTRUCTOR
Dingo::PlayerEngine::PlayerEngine() {
  d_repeat_mode = Dingo::REPEAT_OFF;
  d_status = Dingo::ENGINE_STOPPED;
  d_playorder = Dingo::PLAYORDER_NORMAL;
  
  d_section_start = 0;
  d_section_end = 1;
  
  d_pos = 0;
  d_duration = 0;
  
  d_is_start_toggle_active = false;
  d_is_end_toggle_active = false;
  
  d_is_fullscreen = false;
  d_track_id = 0;
  
  d_preview_pixbuf = Gdk::Pixbuf::create_from_file(DATADIR"/dingo/icons/128x128/dingo.png");
}

Dingo::PlayerEngine::~PlayerEngine() {

}

//OBSERVER'S METHODS
void Dingo::PlayerEngine::registerRepeatModeObserver(Dingo::RepeatModeObserver* observer) {
  repeat_obs_vector.push_back(observer);
}
      
void Dingo::PlayerEngine::removeRepeatModeObserver(Dingo::RepeatModeObserver* observer) {
  std::vector<Dingo::RepeatModeObserver*>::iterator iter = std::find(repeat_obs_vector.begin(), repeat_obs_vector.end(), observer);
  
  if (iter != repeat_obs_vector.end()) {
    repeat_obs_vector.erase(iter);
  }
}

void Dingo::PlayerEngine::notifyRepeatModeObserver(Dingo::RepeatMode repeat_mode) {
  //the RepeatModeObservers only get notified when the Gst::MESSAGE_EOS is emitted.
  switch (repeat_mode) {
    case Dingo::REPEAT_OFF: {
      for (std::vector<Dingo::RepeatModeObserver*>::iterator iter = repeat_obs_vector.begin(); iter != repeat_obs_vector.end(); ++iter) {
        stop();
        (*iter)->updateRepeatOff();
      }
      //std::cout << "Media Repeat Off!" << std::endl;
      break;
    }
    
    case Dingo::REPEAT_SINGLE: {
      for (std::vector<Dingo::RepeatModeObserver*>::iterator iter = repeat_obs_vector.begin(); iter != repeat_obs_vector.end(); ++iter) {
        play();
        (*iter)->updateRepeatSingle();
      }
      //std::cout << "Media Repeat Single!" << std::endl;
      break;
    }
    
    case Dingo::REPEAT_PLAYLIST: {
      for (std::vector<Dingo::RepeatModeObserver*>::iterator iter = repeat_obs_vector.begin(); iter != repeat_obs_vector.end(); ++iter) {
        (*iter)->updateRepeatPlaylist();
      }
      //std::cout << "Media Repeat Playlist!" << std::endl;
      break;
    }
    
    case Dingo::REPEAT_SECTION: {
      for (std::vector<Dingo::RepeatModeObserver*>::iterator iter = repeat_obs_vector.begin(); iter != repeat_obs_vector.end(); ++iter) {
        (*iter)->updateRepeatSection();
      }
      //std::cout << "Media Repeat Section!" << std::endl;
      break;
    }
    
    default: {
      std::cerr << "Undefined Dingo::RepeatMode!" << std::endl;
      break;
    }
  }
}
      
void Dingo::PlayerEngine::registerEngineStatusObserver(Dingo::EngineStatusObserver* observer) {
  status_obs_vector.push_back(observer);
}

void Dingo::PlayerEngine::removeEngineStatusObserver(Dingo::EngineStatusObserver* observer) {
  std::vector<Dingo::EngineStatusObserver*>::iterator iter = std::find(status_obs_vector.begin(), status_obs_vector.end(), observer);
  
  if (iter != status_obs_vector.end()) {
    status_obs_vector.erase(iter);
  }
}

void Dingo::PlayerEngine::notifyEngineStatusObserver(Dingo::EngineStatus engine_status) {
  switch (engine_status) {
    case Dingo::ENGINE_PLAYING: {
      for (std::vector<Dingo::EngineStatusObserver*>::iterator iter = status_obs_vector.begin(); iter != status_obs_vector.end(); ++iter) {
        (*iter)->updateEnginePlaying();
      }
      //std::cout << "Engine Playing!" << std::endl;
      break;
    }
  
    case Dingo::ENGINE_PAUSED: {
      for (std::vector<Dingo::EngineStatusObserver*>::iterator iter = status_obs_vector.begin(); iter != status_obs_vector.end(); ++iter) {
        (*iter)->updateEnginePaused();
      }
      //std::cout << "Engine Paused!" << std::endl;
      break;
    }
    
    case Dingo::ENGINE_STOPPED: {
      for (std::vector<Dingo::EngineStatusObserver*>::iterator iter = status_obs_vector.begin(); iter != status_obs_vector.end(); ++iter) {
        (*iter)->updateEngineStopped();
      }
      //std::cout << "Engine Stopped!" << std::endl;
      break;
    }
    
    case Dingo::ENGINE_NEW_PLAYING: {
      for (std::vector<Dingo::EngineStatusObserver*>::iterator iter = status_obs_vector.begin(); iter != status_obs_vector.end(); ++iter) {
        (*iter)->updateEngineNewPlaying();
      }
      //std::cout << "Engine New Playing!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unknown Dingo::EngineStatus!" << std::endl;
      break;
    }
  }
}

void Dingo::PlayerEngine::registerEngineNavigationObserver(Dingo::EngineNavigationObserver* observer) {
  nav_obs_vector.push_back(observer);
}

void Dingo::PlayerEngine::removeEngineNavigationObserver(Dingo::EngineNavigationObserver* observer) {
  std::vector<Dingo::EngineNavigationObserver*>::iterator iter = std::find(nav_obs_vector.begin(), nav_obs_vector.end(), observer);
  
  if (iter != nav_obs_vector.end()) {
    nav_obs_vector.erase(iter);
  }
}

void Dingo::PlayerEngine::notifyEngineNavigationObserver(Dingo::EngineNavigationSignals engine_nav_status) {
  switch (engine_nav_status) {
    case Dingo::ENGINE_TRACK_NEXT: {
      for (std::vector<Dingo::EngineNavigationObserver*>::iterator iter = nav_obs_vector.begin(); iter != nav_obs_vector.end(); ++iter) {
        (*iter)->updateEngineTrackNext();
      }
      //std::cout << "Engine Navigated to Next Track!" << std::endl;
      break;
    }
    
    case Dingo::ENGINE_TRACK_PREVIOUS: {
      for (std::vector<Dingo::EngineNavigationObserver*>::iterator iter = nav_obs_vector.begin(); iter != nav_obs_vector.end(); ++iter) {
        (*iter)->updateEngineTrackPrevious();
      }
      //std::cout << "Engine Navigated to Previous Track!" << std::endl;
      break;
    }
    
    case Dingo::ENGINE_TRACK_CURRENT: {
      for (std::vector<Dingo::EngineNavigationObserver*>::iterator iter = nav_obs_vector.begin(); iter != nav_obs_vector.end(); ++iter) {
        (*iter)->updateEngineTrackCurrent();
      }
      ////std::cout << "Engine Navigated to Current Track!" << std::endl;
      break;
    }
    
    default: {
      ////std::cout << "Unknown Dingo::EngineNavigationSignals!" << std::endl;
      break;
    }
  }
}

void Dingo::PlayerEngine::registerFullscreenObserver(Dingo::FullscreenObserver* observer) {
  fullscreen_obs_vector.push_back(observer);
}

void Dingo::PlayerEngine::removeFullscreenObserver(Dingo::FullscreenObserver* observer) {
  std::vector<Dingo::FullscreenObserver*>::iterator iter = std::find(fullscreen_obs_vector.begin(), fullscreen_obs_vector.end(), observer);
  
  if (iter != fullscreen_obs_vector.end()) {
    fullscreen_obs_vector.erase(iter);
  }
}

void Dingo::PlayerEngine::notifyFullscreenObserver(Dingo::FullscreenSignals signal_type) {
  switch (signal_type) {
    case Dingo::FULLSCREEN_ACTIVATED: {
      for (std::vector<Dingo::FullscreenObserver*>::iterator iter = fullscreen_obs_vector.begin(); iter != fullscreen_obs_vector.end(); ++iter) {
        (*iter)->updateFullscreenActivated();
      }
      ////std::cout << "Fullscreen Mode Activated!" << std::endl;
      break;
    }
    
    case Dingo::UNFULLSCREEN_ACTIVATED: {
      for (std::vector<Dingo::FullscreenObserver*>::iterator iter = fullscreen_obs_vector.begin(); iter != fullscreen_obs_vector.end(); ++iter) {
        (*iter)->updateUnFullscreenActivated();
      }
      ////std::cout << "UnFullscreen Mode Activated!" << std::endl;
      break;
    }
    
    default: {
      ////std::cout << "Unknown Dingo::VideoAreaSignals!" << std::endl;
      break;
    }
  }
}

void Dingo::PlayerEngine::registerPlayOrderObserver(Dingo::PlayOrderObserver* observer) {
  playorder_obs_vector.push_back(observer);
}

void Dingo::PlayerEngine::removePlayOrderObserver(Dingo::PlayOrderObserver* observer) {
  std::vector<Dingo::PlayOrderObserver*>::iterator iter = std::find(playorder_obs_vector.begin(), playorder_obs_vector.end(), observer);
  
  if (iter != playorder_obs_vector.end()) {
    playorder_obs_vector.erase(iter);
  }
}

void Dingo::PlayerEngine::notifyPlayOrderObserver(Dingo::PlayOrder play_order) {
  switch (play_order) {
    case Dingo::PLAYORDER_NORMAL: {
      for (std::vector<Dingo::PlayOrderObserver*>::iterator iter = playorder_obs_vector.begin(); iter != playorder_obs_vector.end(); ++iter) {
        (*iter)->updatePlayOrderNormal();
      }
      ////std::cout << "Normal PlayOrder Mode Activated!" << std::endl;
      break;
    }
    
    case Dingo::PLAYORDER_SHUFFLE: {
      for (std::vector<Dingo::PlayOrderObserver*>::iterator iter = playorder_obs_vector.begin(); iter != playorder_obs_vector.end(); ++iter) {
        (*iter)->updatePlayOrderShuffle();
      }
      ////std::cout << "Shuffle PlayOrder Activated!" << std::endl;
      break;
    }
    
    default: {
      ////std::cout << "Unknown Dingo::PlayOrder!" << std::endl;
      break;
    }
  }
}

void Dingo::PlayerEngine::registerRepeatToggleObserver(Dingo::RepeatToggleObserver* observer) {
  repeat_toggle_obs_vector.push_back(observer);
}

void Dingo::PlayerEngine::removeRepeatToggleObserver(Dingo::RepeatToggleObserver* observer) {
  std::vector<Dingo::RepeatToggleObserver*>::iterator iter = std::find(repeat_toggle_obs_vector.begin(), repeat_toggle_obs_vector.end(), observer);
  
  if (iter != repeat_toggle_obs_vector.end()) {
    repeat_toggle_obs_vector.erase(iter);
  }
}

void Dingo::PlayerEngine::notifyRepeatToggleObserver(Dingo::RepeatMode repeat_mode, Glib::ustring section_button_code) {
  switch (repeat_mode) {
    case Dingo::REPEAT_OFF: {
      for (std::vector<Dingo::RepeatToggleObserver*>::iterator iter = repeat_toggle_obs_vector.begin(); iter != repeat_toggle_obs_vector.end(); ++iter) {
        (*iter)->updateRepeatOffToggled();
      }
      //std::cout << "Repeat Off Toggled!" << std::endl;
      break;
    }
    
    case Dingo::REPEAT_SINGLE: {
      for (std::vector<Dingo::RepeatToggleObserver*>::iterator iter = repeat_toggle_obs_vector.begin(); iter != repeat_toggle_obs_vector.end(); ++iter) {
        (*iter)->updateRepeatSingleToggled();
      }
      //std::cout << "Repeat Single Toggled!" << std::endl;
      break;
    }
    
    case Dingo::REPEAT_PLAYLIST: {
      for (std::vector<Dingo::RepeatToggleObserver*>::iterator iter = repeat_toggle_obs_vector.begin(); iter != repeat_toggle_obs_vector.end(); ++iter) {
        (*iter)->updateRepeatPlaylistToggled();
      }
      //std::cout << "Repeat Playlist Toggled!" << std::endl;
      break;
    }
    
    case Dingo::REPEAT_SECTION: {    
      for (std::vector<Dingo::RepeatToggleObserver*>::iterator iter = repeat_toggle_obs_vector.begin(); iter != repeat_toggle_obs_vector.end(); ++iter) {
        (*iter)->updateRepeatSectionToggled(section_button_code);
      }
      //std::cout << "Repeat Section Toggled!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unknown Dingo::RepeatMode!" << std::endl;
      break;
    }
  }
}

//PUBLIC CONTROLLERS
void Dingo::PlayerEngine::read(Glib::ustring fileuri, Glib::ustring track_mimetype, int track_id, Glib::ustring suburi) {
  d_fileuri = fileuri;
  d_track_mimetype = track_mimetype;
  d_suburi = suburi;
  d_track_id = track_id;
  
  //create Gst::PlayBin2
  d_playbin = Gst::PlayBin2::create("playbin");
  
  if (!d_playbin) {
    std::cerr << "Could not create Gst::PlayBin2!" << std::endl;
    return;
  }
  
  //create Gst::XImageSink
  d_videosink = Gst::XImageSink::create("ximagesink");
  
  if (!d_videosink) {
    std::cerr << "Could not create Gst::XImageSink!" << std::endl;
    return;
  }
  
  d_playbin->property_video_sink() = d_videosink;
  
  //get the bus from pipeline
  Glib::RefPtr<Gst::Bus> bus = d_playbin->get_bus();
  
  bus->enable_sync_message_emission();
  
  bus->signal_sync_message().connect(sigc::mem_fun(*this, &Dingo::PlayerEngine::onBusMessageSync));
  
  d_watch_id = bus->add_watch(sigc::mem_fun(*this, &Dingo::PlayerEngine::onBusMessage));
  
  d_playbin->set_auto_flush_bus(true);
  
  //open the URI
  d_playbin->property_uri() = d_fileuri;
  d_playbin->property_suburi() = suburi;
  
  if (!d_subfont.empty()) {
    d_playbin->property_subtitle_font_desc() = d_subfont;
  }
}

void Dingo::PlayerEngine::setStatus(Dingo::EngineStatus status) {
  d_status = status;
  
  if (d_playbin) {
    switch (d_status) {
      case Dingo::ENGINE_PLAYING: {
        d_playbin->set_state(Gst::STATE_PLAYING);
      
        break;
      }
    
      case Dingo::ENGINE_PAUSED: {
        d_playbin->set_state(Gst::STATE_PAUSED);
      
        break;
      }
    
      case Dingo::ENGINE_STOPPED: {
        d_playbin->set_state(Gst::STATE_NULL);
      
        d_playbin->get_bus()->remove_watch(d_watch_id);
      
        Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Metadata");
      
        break;
      }
    
      default: {
        //std::cout << "Unknown Dingo::EngineStatus!" << std::endl;
        break;
      }
    }
  }
      
  Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "PlaybackStatus");
}

Dingo::EngineStatus Dingo::PlayerEngine::getStatus() {
  return d_status;
}

void Dingo::PlayerEngine::play() {
  notifyEngineStatusObserver(Dingo::ENGINE_PLAYING);
  
  setStatus(Dingo::ENGINE_PLAYING);
}

void Dingo::PlayerEngine::pause() {
  notifyEngineStatusObserver(Dingo::ENGINE_PAUSED);

  setStatus(Dingo::ENGINE_PAUSED);
}

void Dingo::PlayerEngine::next() {
  notifyEngineNavigationObserver(Dingo::ENGINE_TRACK_NEXT);
  
  play();
}

void Dingo::PlayerEngine::previous() {
  notifyEngineNavigationObserver(Dingo::ENGINE_TRACK_PREVIOUS);
  
  play();
}

void Dingo::PlayerEngine::stop() {
  notifyEngineStatusObserver(Dingo::ENGINE_STOPPED);
  
  setStatus(Dingo::ENGINE_STOPPED);
}

void Dingo::PlayerEngine::replay() {
  stop();
  
  read(d_fileuri, d_track_mimetype, d_track_id, d_suburi);
  
  play();
  
  Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Metadata");
}

void Dingo::PlayerEngine::seek(gint64 new_pos) {
  if (d_playbin->seek(Gst::FORMAT_TIME, Gst::SEEK_FLAG_FLUSH, new_pos)) {
    //std::cout << "Seek succeeded!" << std::endl;
  }
  
  else {
    //std::cout << "Seek failed!" << std::endl;
  }
  
  Dingo::MPRISv2::emitInterfacesSignal(Dingo::MPRISv2::PATH, Dingo::MPRISv2::INTERFACE_PLAYER, "Seeked");
}

void Dingo::PlayerEngine::seekForward(gint64 forward_pos) {
  if (d_playbin->seek(Gst::FORMAT_TIME, Gst::SEEK_FLAG_FLUSH, d_pos + forward_pos)) {
    //std::cout << "Seek succeeded!" << std::endl;
  }
  
  else {
    //std::cout << "Seek failed!" << std::endl;
  }
  
  Dingo::MPRISv2::emitInterfacesSignal(Dingo::MPRISv2::PATH, Dingo::MPRISv2::INTERFACE_PLAYER, "Seeked");
}

//ACCESSORS
Glib::ustring Dingo::PlayerEngine::getPositionString() {
  return Dingo::Utilities::durationToString(gint64(d_pos / 1000000000));
}

Glib::ustring Dingo::PlayerEngine::getDurationString() {
  return Dingo::Utilities::durationToString(gint64(d_duration / 1000000000));
}

double Dingo::PlayerEngine::getPercentage() {
  Gst::Format fmt = Gst::FORMAT_TIME;
  double result = 0;
  
  if (d_playbin) {
    if ((d_playbin->query_position(fmt, d_pos)) && (d_playbin->query_duration(fmt, d_duration))) {
    result = double(d_pos) / d_duration;
    }
  }
  
  return result;
}

void Dingo::PlayerEngine::setPercentage(double percentage) {
  const gint64 new_pos = gint64(percentage * d_duration);
  
  if (d_playbin->seek(Gst::FORMAT_TIME, Gst::SEEK_FLAG_FLUSH, new_pos)) {
    //std::cout << "Seek succeeded!" << std::endl;
  }
  
  else {
    //std::cout << "Seek failed!" << std::endl;
  }
  
  Dingo::MPRISv2::emitInterfacesSignal(Dingo::MPRISv2::PATH, Dingo::MPRISv2::INTERFACE_PLAYER, "Seeked");
}

double Dingo::PlayerEngine::getSectionStart() {
  return d_section_start;
}

void Dingo::PlayerEngine::setSectionStart(double start_percentage) {
  d_section_start = start_percentage;
}
      
double Dingo::PlayerEngine::getSectionEnd() {
  return d_section_end;
}

void Dingo::PlayerEngine::setSectionEnd(double end_percentage) {
  d_section_end = end_percentage;
}
      
Glib::ustring Dingo::PlayerEngine::getSectionStartString() {
  return Dingo::Utilities::durationToString(gint64(d_section_start * d_duration / 1000000000));
}

Glib::ustring Dingo::PlayerEngine::getSectionEndString() {
  return Dingo::Utilities::durationToString(gint64(d_section_end * d_duration / 1000000000));
}

void Dingo::PlayerEngine::setRepeatMode(Dingo::RepeatMode repeat_mode, Glib::ustring section_button_code) {
  d_repeat_mode = repeat_mode;
  
  notifyRepeatToggleObserver(repeat_mode, section_button_code);
  
  Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "LoopStatus");
}

Dingo::RepeatMode Dingo::PlayerEngine::getRepeatMode() {
  return d_repeat_mode;
}

void Dingo::PlayerEngine::setPlayOrder(Dingo::PlayOrder playorder) {
  d_playorder = playorder;
  
  notifyPlayOrderObserver(d_playorder);
  
  Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Shuffle");
}

Dingo::PlayOrder Dingo::PlayerEngine::getPlayOrder() {
  return d_playorder;
}

void Dingo::PlayerEngine::setXWindowID(gulong x_window_id) {
  d_x_window_id = x_window_id;
}

gulong Dingo::PlayerEngine::getXWindowID() {
  return d_x_window_id;
}

void Dingo::PlayerEngine::setFullscreen(bool is_fullscreen) {
  d_is_fullscreen = is_fullscreen;
  
  if (d_is_fullscreen) {
    notifyFullscreenObserver(Dingo::FULLSCREEN_ACTIVATED);
  }
  
  else {
    notifyFullscreenObserver(Dingo::UNFULLSCREEN_ACTIVATED);
  }
  
  Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_ROOT, "Fullscreen");
}

bool Dingo::PlayerEngine::getFullscreen() {
  return d_is_fullscreen;
}

void Dingo::PlayerEngine::setSubtitle(Glib::ustring suburi) {
  double cur_percentage = getPercentage();
  Dingo::EngineStatus cur_status = d_status;

  stop();
  
  read(d_fileuri, d_track_mimetype, d_track_id, suburi);
  
  switch (cur_status) {
    case ENGINE_PLAYING: {
      play();
    
      break;
    }
    
    case Dingo::ENGINE_PAUSED: {
      pause();
    
      break;
    }
    
    case Dingo::ENGINE_STOPPED: {
      break;
    }
    
    case Dingo::ENGINE_NEW_PLAYING: {
      play();
    
      break;
    }
    
    default: {
      break;
    }
  }
  
  Glib::signal_timeout().connect(sigc::bind(sigc::mem_fun(*this, &Dingo::PlayerEngine::restoreBeforeSubtitleSetPosition), cur_percentage), 50);
}

bool Dingo::PlayerEngine::restoreBeforeSubtitleSetPosition(double percentage) {
  setPercentage(percentage);
  
  return false;
}

void Dingo::PlayerEngine::setSubtitleFontDesc(Glib::ustring subfont) {
  d_subfont = subfont;
  
  if (d_playbin) {
    d_playbin->property_subtitle_font_desc() = subfont;
  }
}

Glib::ustring Dingo::PlayerEngine::getSubtitleFontDesc() {
  return d_subfont;
}

void Dingo::PlayerEngine::setVolume(double volume_value) {
  d_playbin->property_volume() = volume_value;
  
  Dingo::MPRISv2::emitPropertiesChangedSignal(Dingo::MPRISv2::INTERFACE_PLAYER, "Volume");
}

double Dingo::PlayerEngine::getVolume() {
  if (d_playbin) {
    return d_playbin->property_volume();
  }
  
  else {
    return 0.75;
  }
}

Glib::RefPtr<Gdk::Pixbuf> Dingo::PlayerEngine::getPreviewImage(bool is_resize) {
  if (d_track_mimetype == "audio/*") {
    Glib::RefPtr<Gdk::Pixbuf> temp_pixbuf = d_preview_pixbuf;
    
    MediaInfoLib::MediaInfo d_mediainfo;
    
    d_mediainfo.Open(Dingo::Utilities::stringToWString(Glib::filename_from_uri(d_fileuri)));
    
    Glib::ustring base64_ustring = Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("Cover_Data")));
    
    gsize data_size;
    guint8* data = g_base64_decode(base64_ustring.c_str(), &data_size);
    
    if (data_size != 0) {
      Glib::RefPtr<Gdk::PixbufLoader> temp_loader = Gdk::PixbufLoader::create();
      
      temp_loader->write(data, data_size);
    
      temp_pixbuf = temp_loader->get_pixbuf();
  
      temp_loader->close();
    }
    
    else {
      if (is_resize) {
        return d_preview_pixbuf->scale_simple(60, 60, Gdk::INTERP_HYPER);
      }
      
      else {
        return d_preview_pixbuf;
      }
    }
    
    if (is_resize) {
      double outheight_double = (double) temp_pixbuf->get_height();
      double outwidth_double = (double) temp_pixbuf->get_width();
    
      if (outwidth_double >= outheight_double) {
        temp_pixbuf = temp_pixbuf->scale_simple(60, Dingo::Utilities::doubleToInt(60 * (outheight_double / outwidth_double)), Gdk::INTERP_HYPER);      
      }
      
      else {
        temp_pixbuf = temp_pixbuf->scale_simple(Dingo::Utilities::doubleToInt(60 * (outwidth_double / outheight_double)), 60, Gdk::INTERP_HYPER);
      }
    }
  
    return temp_pixbuf;
  }
  
  else {
    if (is_resize) {
      return d_preview_pixbuf->scale_simple(60, 60, Gdk::INTERP_HYPER);
    }
      
    else {
      return d_preview_pixbuf;
    }
  }
}

Glib::ustring Dingo::PlayerEngine::getPreviewImageURI() {
  Dingo::Utilities::createCacheCoverArtFolders();
  
  return Dingo::Utilities::getPixbufFileURI(getPreviewImage(false), d_track_id);
}

Glib::RefPtr<Gdk::Pixbuf> Dingo::PlayerEngine::getDefaultPreviewImage(bool is_resize) {
  if (is_resize) {
    return d_preview_pixbuf->scale_simple(60, 60, Gdk::INTERP_HYPER);
  }
  
  else {
    return d_preview_pixbuf;
  }
}

Glib::ustring Dingo::PlayerEngine::getMIMEType() {
  return d_track_mimetype;
}

gint64 Dingo::PlayerEngine::getPosition() {
  return d_pos;
}

void Dingo::PlayerEngine::setShowSubtitle(bool is_visible) {
  if (d_playbin) {
    if (is_visible) {
      d_playbin->property_suburi() = d_suburi;    
      d_playbin->property_subtitle_font_desc() = d_subfont;
    }
    
    else {
      d_playbin->property_current_text() = -1;
    }
  }
}

//SIGNAL HANDLERS
void Dingo::PlayerEngine::onBusMessageSync(const Glib::RefPtr<Gst::Message>& message) {
  if (message->get_message_type() != Gst::MESSAGE_ELEMENT) {
    return;
  }
  
  if (!message->get_structure().has_name("prepare-xwindow-id")) {
    return;
  }   
  
  Glib::RefPtr<Gst::Element> element = Glib::RefPtr<Gst::Element>::cast_dynamic(message->get_source());
  
  Glib::RefPtr<Gst::ElementInterfaced<Gst::XOverlay> > xoverlay = Gst::Interface::cast<Gst::XOverlay>(element);
  
  if (xoverlay) {
    //we can actually save this Gst::XOverlay for later use
    xoverlay->set_xwindow_id(d_x_window_id);
    
    d_xoverlay = xoverlay;
  }
}

bool Dingo::PlayerEngine::onBusMessage(const Glib::RefPtr<Gst::Bus>& bus, const Glib::RefPtr<Gst::Message>& message) {
  switch (message->get_message_type()) {
    case Gst::MESSAGE_EOS: {
    
      switch (d_repeat_mode) {
        case Dingo::REPEAT_OFF: {
          //std::cout << "No repeat will be performed!" << std::endl;
          notifyRepeatModeObserver(d_repeat_mode);
          break;
        }
        
        case Dingo::REPEAT_SINGLE: {
          //std::cout << "Single repeat will be performed!" << std::endl;
          notifyRepeatModeObserver(d_repeat_mode);
          break;
        }
        
        case Dingo::REPEAT_PLAYLIST: {
          //std::cout << "Playlist repeat will be performed!" << std::endl;
          notifyRepeatModeObserver(d_repeat_mode);
          break;
        }
        
        default: {
          //std::cout << "Unknown d_repeat_mode!" << std::endl;
          break;
        }
      }
      
      break;
    }
    
    case Gst::MESSAGE_ERROR: {
      Glib::RefPtr<Gst::MessageError> msg_error = Glib::RefPtr<Gst::MessageError>::cast_dynamic(message);
      
      if (msg_error) {
        Glib::Error err;
        err = msg_error->parse();
        std::cerr << "Error: " << err.what() << std::endl;
      }
      
      else {
        std::cerr << "Error." << std::endl;
      }
      
      stop();
      break;
    }
    
    default: {
      //let's leave this blank for now...
      break;
    }
  }
  
  return true;
}
