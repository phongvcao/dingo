/* 
 * dingo-mpriscommon.cc
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

#include "dingo-mpriscommon.h"
#include "dingo-dbmanager.h"
#include "dingo-playerengine.h"
#include "dingo-settings.h"
#include "dingo-toolbar.h"
#include "dingo-tracktreeview.h"
#include "dingo-playlisttreeview.h"
#include "dingo-controlbar.h"
#include "dingo-statusicon.h"
#include "dingo-utilities.h"

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

#ifndef INCLUDED_GLIBMM_I18N_H
#define INCLUDED_GLIBMM_I18N_H
#include <glibmm/i18n.h>
#endif //INCLUDED_GLIBMM_I18N_H

//CONSTRUCTORS - DESTRUCTOR
Dingo::MPRISCommon::MPRISCommon(Dingo::DBManager* dbmanager, Dingo::PlayerEngine* playerengine, Dingo::Settings* settings, Gtk::Application* dingo_app) {
  d_dbmanager = dbmanager;
  d_playerengine = playerengine;
  d_settings = settings;
  d_dingo_app = dingo_app;
  
  d_active_playlist_changed_count = 0;
}

Dingo::MPRISCommon::~MPRISCommon() {

}
      
//PUBLIC ACCESSORS
void Dingo::MPRISCommon::setToolBar(Dingo::ToolBar* toolbar) {
  d_toolbar = toolbar;
}

void Dingo::MPRISCommon::setTrackTreeView(Dingo::TrackTreeView* tracktreeview) {
  d_tracktreeview = tracktreeview;
}

void Dingo::MPRISCommon::setPlaylistTreeView(Dingo::PlaylistTreeView* playlisttreeview) {
  d_playlisttreeview = playlisttreeview;
}

void Dingo::MPRISCommon::setControlBar(Dingo::ControlBar* controlbar) {
  d_controlbar = controlbar;
}

void Dingo::MPRISCommon::setStatusIcon(Dingo::StatusIcon* statusicon) {
  d_statusicon = statusicon;
}
      
//PUBLIC CONTROLLER INTERFACE
Glib::VariantContainerBase Dingo::MPRISCommon::invokeMethodRoot(const Glib::ustring& method_name, const Glib::VariantContainerBase& parameters) {
  if (method_name == "Raise") {
    raise();
  }
  
  else if (method_name == "Quit") {
    quit();
  }
  
  else {
    std::cout << "Dingo::MPRISCommon::invokeMethodRoot(): Unsupported Method!" << std::endl;
  }
}

Glib::VariantContainerBase Dingo::MPRISCommon::invokeMethodPlayer(const Glib::ustring& method_name, const Glib::VariantContainerBase& parameters) {
  if (method_name == "Next") {
    next();
  }
  
  else if (method_name == "Previous") {
    previous();
  }
  
  else if (method_name == "Pause") {
    pause();
  }
  
  else if (method_name == "PlayPause") {
    playPause();
  }
  
  else if (method_name == "Stop") {
    stop();
  }
  
  else if (method_name == "Play") {
    play();
  }
  
  else if (method_name == "Seek") {
    Glib::VariantBase seek_offset_variant;
    parameters.get_child(seek_offset_variant, 0);
  
    Glib::Variant<gint64> seek_offset = Glib::VariantBase::cast_dynamic<Glib::Variant<gint64> >(seek_offset_variant);
    
    seek(seek_offset.get());
  }
  
  else if (method_name == "SetPosition") {
    Glib::VariantBase seek_position_variant;
    parameters.get_child(seek_position_variant, 1);
    
    Glib::Variant<gint64> seek_position = Glib::VariantBase::cast_dynamic<Glib::Variant<gint64> >(seek_position_variant);
  
    setPosition(seek_position.get());
  }
  
  else if (method_name == "OpenUri") {
    Glib::VariantBase uri_variant;
    parameters.get_child(uri_variant, 0);
    
    Glib::Variant<Glib::ustring> uri = Glib::VariantBase::cast_dynamic<Glib::Variant<Glib::ustring> >(uri_variant);
    
    openURI(uri.get());
  }
  
  else {
    std::cout << "Dingo::MPRISCommon::invokeMethodPlayer(): Unsupported Method: " << method_name << "!" << std::endl;
  }
}

Glib::VariantContainerBase Dingo::MPRISCommon::invokeMethodPlaylists(const Glib::ustring& method_name, const Glib::VariantContainerBase& parameters) {
  if (method_name == "ActivatePlaylist") {
    Glib::VariantBase playlist_objpath_id_variant;
    parameters.get_child(playlist_objpath_id_variant, 0);
    
    Glib::Variant<Glib::ustring> playlist_objpath_id = Glib::VariantBase::cast_dynamic<Glib::Variant<Glib::ustring> >(playlist_objpath_id_variant);
    
    activatePlaylist(playlist_objpath_id.get());
    
    std::vector<Glib::VariantBase> fake_return_vector;
    
    return Glib::VariantContainerBase::create_tuple(fake_return_vector);
  }
  
  else if (method_name == "GetPlaylists") {
    Glib::VariantBase max_count_variant_base;
    parameters.get_child(max_count_variant_base, 1);
  
    Glib::Variant<guint32> max_count_variant = Glib::VariantBase::cast_dynamic<Glib::Variant<guint32> >(max_count_variant_base);
    
    Glib::VariantBase reverse_variant_base;
    parameters.get_child(reverse_variant_base, 3);
    
    Glib::Variant<bool> reverse_variant = Glib::VariantBase::cast_dynamic<Glib::Variant<bool> >(reverse_variant_base);
    
    return getPlaylists(max_count_variant.get(), reverse_variant.get());
  }
  
  else {
    std::cout << "Dingo::MPRISCommon::invokeMethodPlaylists(): Unsupported Method!" << std::endl;
  }
}
      
Glib::VariantBase Dingo::MPRISCommon::readPropertyValueRoot(const Glib::ustring& property_name) {
  if (property_name == "CanQuit") {
    return Glib::Variant<bool>::create(true);
  }
  
  else if (property_name == "Fullscreen") {
    return Glib::Variant<bool>::create(readFullscreen());
  }
  
  else if (property_name == "CanSetFullscreen") {
    return Glib::Variant<bool>::create(true);
  }
  
  else if (property_name == "CanRaise") {
    return Glib::Variant<bool>::create(true);
  }
  
  else if (property_name == "HasTrackList") {
    return Glib::Variant<bool>::create(true);
  }
  
  else if (property_name == "Identity") {
    return Glib::Variant<Glib::ustring>::create("Dingo");
  }
  
  else if (property_name == "DesktopEntry") {
    return Glib::Variant<Glib::ustring>::create("dingo");
  }
  
  else if (property_name == "SupportedUriSchemes") {
    std::vector<Glib::ustring> uri_schemes_vector;
    
    uri_schemes_vector.push_back("file");
    uri_schemes_vector.push_back("http");
    
    return Glib::Variant<std::vector<Glib::ustring> >::create(uri_schemes_vector);
  }
  
  else if (property_name == "SupportedMimeTypes") {
    std::vector<Glib::ustring> mimetypes_vector;
    
    mimetypes_vector.push_back("application/x-ogg");
    mimetypes_vector.push_back("application/ogg");
    mimetypes_vector.push_back("audio/basic");
    mimetypes_vector.push_back("audio/L24");
    mimetypes_vector.push_back("audio/mp4");
    mimetypes_vector.push_back("audio/mpeg");
    mimetypes_vector.push_back("audio/ogg");
    mimetypes_vector.push_back("audio/vorbis");
    mimetypes_vector.push_back("audio/vnd.wave");
    mimetypes_vector.push_back("video/mpeg");
    mimetypes_vector.push_back("video/mp4");
    mimetypes_vector.push_back("video/ogg");
    mimetypes_vector.push_back("video/quicktime");
    mimetypes_vector.push_back("video/webm");
    mimetypes_vector.push_back("video/x-matroska");
    mimetypes_vector.push_back("video/x-ms-wmv");
    mimetypes_vector.push_back("video/x-flv");
    mimetypes_vector.push_back("audio/x-vorbis+ogg");
    mimetypes_vector.push_back("audio/x-scpls");
    mimetypes_vector.push_back("audio/x-mp3");
    mimetypes_vector.push_back("audio/x-mpeg");
    mimetypes_vector.push_back("audio/x-mpegurl");
    mimetypes_vector.push_back("audio/x-flac");
    
    return Glib::Variant<std::vector<Glib::ustring> >::create(mimetypes_vector);
  }
  
  else {
    std::cout << "Dingo::MPRISCommon::readPropertyValueRoot(): Unsupported Property!" << std::endl;
  }
}

Glib::VariantBase Dingo::MPRISCommon::readPropertyValuePlayer(const Glib::ustring& property_name) {
  if (property_name == "PlaybackStatus") {
    return readPlaybackStatus();
  }
  
  else if (property_name == "LoopStatus") {
    return readLoopStatus();
  }
  
  else if (property_name == "Rate") {
    return Glib::Variant<double>::create(1);
  }
  
  else if (property_name == "Shuffle") {
    return Glib::Variant<bool>::create(readShuffle());
  }
  
  else if (property_name == "Metadata") {
    return readMetadata();
  }
  
  else if (property_name == "Volume") {
    return Glib::Variant<double>::create(readVolume());
  }
  
  else if (property_name == "Position") {
    return Glib::Variant<gint64>::create(readPosition());
  }
  
  else if (property_name == "MinimumRate") {
    return Glib::Variant<double>::create(1);
  }
  
  else if (property_name == "MaximumRate") {
    return Glib::Variant<double>::create(1);
  }
  
  else if (property_name == "CanGoNext") {
    return Glib::Variant<bool>::create(true);
  }
  
  else if (property_name == "CanGoPrevious") {
    return Glib::Variant<bool>::create(true);
  }
  
  else if (property_name == "CanPlay") {
    return Glib::Variant<bool>::create(true);
  }
  
  else if (property_name == "CanPause") {
    return Glib::Variant<bool>::create(true);
  }
  
  else if (property_name == "CanSeek") {
    return Glib::Variant<bool>::create(true);
  }
  
  else if (property_name == "CanControl") {
    return Glib::Variant<bool>::create(true);
  }
  
  else {
    std::cout << "Dingo::MPRISCommon::readPropertyValuePlayer(): Unsupported Property!" << std::endl;
  }
}

Glib::VariantBase Dingo::MPRISCommon::readPropertyValuePlaylists(const Glib::ustring& property_name) {
  if (property_name == "PlaylistCount") {
    return Glib::Variant<guint32>::create(readPlaylistCount());
  }
  
  else if (property_name == "Orderings") {
    return Glib::Variant<Glib::ustring>::create("Alphabetical");
  }
  
  else if (property_name == "ActivePlaylist") {
    return readActivePlaylist();
  }
  
  else {
    std::cout << "Dingo::MPRISCommon::readPropertyValuePlaylists(): Unsupported Property!" << std::endl;
  }
}
      
void Dingo::MPRISCommon::writePropertyValueRoot(const Glib::ustring& property_name, const Glib::VariantBase& new_value_variantbase) {
  if (property_name == "Fullscreen") {
    Glib::Variant<bool> fullscreen_value = Glib::VariantBase::cast_dynamic<Glib::Variant<bool> >(new_value_variantbase);
    
    writeFullscreen(fullscreen_value.get());
  }
  
  else {
    std::cout << "Dingo::MPRISCommon::writePropertyValueRoot(): Unwritable Property!" << std::endl;
  }
}

void Dingo::MPRISCommon::writePropertyValuePlayer(const Glib::ustring& property_name, const Glib::VariantBase& new_value_variantbase) {
  if (property_name == "LoopStatus") {
    Glib::Variant<Glib::ustring> loop_status_variant = Glib::VariantBase::cast_dynamic<Glib::Variant<Glib::ustring> >(new_value_variantbase);
    
    writeLoopStatus(loop_status_variant.get());
  }
  
  else if (property_name == "Rate") {
    //not supported!
  }
  
  else if (property_name == "Shuffle") {
    Glib::Variant<bool> shuffle_variant = Glib::VariantBase::cast_dynamic<Glib::Variant<bool> >(new_value_variantbase);
    
    writeShuffle(shuffle_variant.get());
  }
  
  else if (property_name == "Volume") {
    Glib::Variant<double> volume_variant = Glib::VariantBase::cast_dynamic<Glib::Variant<double> >(new_value_variantbase);
    
    writeVolume(volume_variant.get());
  }
  
  else {
    std::cout << "Dingo::MPRISCommon::writePropertyValuePlaylists(): Unwritable Property!" << std::endl;
  }
}

void Dingo::MPRISCommon::writePropertyValuePlaylists(const Glib::ustring& property_name, const Glib::VariantBase& new_value_variantbase) {
  //no writable properties
}

Glib::VariantContainerBase Dingo::MPRISCommon::getParametersPlayerSignal(const Glib::ustring& signal_name) {
  if (signal_name == "Seeked") {
    std::vector<Glib::VariantBase> x_vector;
    x_vector.push_back(Glib::Variant<gint64>::create(d_playerengine->getPosition() * 1000));
  
    return Glib::VariantContainerBase::create_tuple(x_vector);
  }
  
  else {
    std::cout << "Unsupported signal: " << signal_name << "!" << std::endl;
  }
}

Glib::VariantContainerBase Dingo::MPRISCommon::getParametersPlaylistsSignal(const Glib::ustring& signal_name) {
  if (signal_name == "PlaylistChanged") {
    //not implemented!
    std::vector<Glib::VariantBase> oss_vector;
    
    return Glib::VariantContainerBase::create_tuple(oss_vector);
  }
  
  else {
    std::cout << "Unsupported signal: " << signal_name << "!" << std::endl;
  }
}

//SIGNAL HANDLERS
bool Dingo::MPRISCommon::onOpenURITimeOut(Glib::ustring track_uri) {
  d_toolbar->activateToggleButton(Dingo::PLAY_QUEUE_TOGGLED);
  
  d_tracktreeview->activateRow(track_uri);
  
  return false;
}
      
//MPRISv2 ROOT COMMANDS
void Dingo::MPRISCommon::raise() {
  d_statusicon->raise();
}

void Dingo::MPRISCommon::quit() {
  d_playerengine->stop();

  d_dingo_app->release();
  
  d_dbmanager->getWindow()->hide();
}

bool Dingo::MPRISCommon::readFullscreen() {
  return d_playerengine->getFullscreen();
}

void Dingo::MPRISCommon::writeFullscreen(bool fullscreen) {
  d_playerengine->setFullscreen(fullscreen);
}
      
//MPRISv2 PLAYER COMMANDS
void Dingo::MPRISCommon::next() {
  d_controlbar->onNextButtonClicked();
}

void Dingo::MPRISCommon::previous() {
  d_controlbar->onPreviousButtonClicked();
}

void Dingo::MPRISCommon::pause() {
  if (d_playerengine->getStatus() == Dingo::ENGINE_PLAYING) {
    d_controlbar->onPlayButtonClicked();
  }
}

void Dingo::MPRISCommon::playPause() {
  d_controlbar->onPlayButtonClicked();
}

void Dingo::MPRISCommon::stop() {
  d_playerengine->stop();
}

void Dingo::MPRISCommon::play() {
  if (d_playerengine->getStatus() != Dingo::ENGINE_PLAYING) {
    d_controlbar->onPlayButtonClicked();
  }
}

void Dingo::MPRISCommon::seek(gint64 forward_pos) {
  if (d_playerengine->getStatus() != Dingo::ENGINE_STOPPED) {
    d_playerengine->seek(forward_pos * 1000);
  }
}

void Dingo::MPRISCommon::setPosition(gint64 new_pos) {
  if (d_playerengine->getStatus() != Dingo::ENGINE_STOPPED) {
    d_playerengine->seekForward(new_pos * 1000);
  }
}

void Dingo::MPRISCommon::openURI(Glib::ustring track_uri) {
  std::vector<Glib::ustring> track_uris_vector;
  
  track_uris_vector.push_back(track_uri);

  d_dbmanager->addTracksToDatabase(track_uris_vector, 1);
      
  if (d_settings->getVisible("playlistbrowser")) {
    d_dbmanager->notifyDBManPlaylistObserver(Dingo::CHANGES_IN_CORE_PLAYLISTS);
  }
  
  if (d_settings->getVisible("genrefilter")) {
    d_dbmanager->notifyDBManGenreObserver(Dingo::CHANGES_IN_CORE_GENRES);
  }
      
  if (d_settings->getVisible("artistfilter")) {
    d_dbmanager->notifyDBManArtistObserver(Dingo::CHANGES_IN_CORE_ARTISTS);
  }
      
  if (d_settings->getVisible("yearfilter")) {
    d_dbmanager->notifyDBManYearObserver(Dingo::CHANGES_IN_CORE_YEARS);
  }
      
  if (d_settings->getVisible("albumfilter")) {
    d_dbmanager->notifyDBManAlbumObserver(Dingo::CHANGES_IN_CORE_ALBUMS);
  }
      
  if (d_settings->getVisible("trackbrowser")) {
    d_dbmanager->notifyDBManTrackObserver(Dingo::CHANGES_IN_CORE_TRACKS);
  }
  
  Glib::signal_timeout().connect(sigc::bind(sigc::mem_fun(*this, &Dingo::MPRISCommon::onOpenURITimeOut), track_uri), 1000);
}
      
Glib::VariantBase Dingo::MPRISCommon::readPlaybackStatus() {
  switch (d_playerengine->getStatus()) {
    case Dingo::ENGINE_PLAYING: {      
      return Glib::Variant<Glib::ustring>::create("Playing");
    }
      
    case Dingo::ENGINE_PAUSED: {
      return Glib::Variant<Glib::ustring>::create("Paused");
    }
      
    case Dingo::ENGINE_STOPPED: {      
      return Glib::Variant<Glib::ustring>::create("Stopped");
    }
      
    case Dingo::ENGINE_NEW_PLAYING: {      
      return Glib::Variant<Glib::ustring>::create("Playing");
    }
      
    default: {
      return Glib::Variant<Glib::ustring>::create("Stopped");
    }
  }
}

Glib::VariantBase Dingo::MPRISCommon::readLoopStatus() {
  switch (d_playerengine->getRepeatMode()) {
    case Dingo::REPEAT_OFF: {
      return Glib::Variant<Glib::ustring>::create("None");
    }
      
    case Dingo::REPEAT_SINGLE: {
      return Glib::Variant<Glib::ustring>::create("Track");
    }
      
    case Dingo::REPEAT_PLAYLIST: {
      return Glib::Variant<Glib::ustring>::create("Playlist");
    }
      
    case Dingo::REPEAT_SECTION: {
      return Glib::Variant<Glib::ustring>::create("Track");
    }
      
    default: {
      return Glib::Variant<Glib::ustring>::create("None");
    }
  }
}

bool Dingo::MPRISCommon::readShuffle() {
  if (d_playerengine->getPlayOrder() == Dingo::PLAYORDER_NORMAL) {
    return false;
  }
  
  else {
    return true;
  }
}

Glib::VariantBase Dingo::MPRISCommon::readMetadata() {
  Glib::Variant<std::map<Glib::ustring, Glib::VariantBase> > metadata_variant;
  std::map<Glib::ustring, Glib::VariantBase> metadata_map;
  
  Gtk::TreeRowReference cur_playing_row_ref = d_dbmanager->getCurrentTrackRowRef();
  
  if ((!cur_playing_row_ref.is_valid()) || (d_playerengine->getStatus() == Dingo::ENGINE_STOPPED)) {
    metadata_variant = Glib::Variant<std::map<Glib::ustring, Glib::VariantBase> >::create(metadata_map);
    
    return metadata_variant;
  }
  
  Gtk::TreeModel::Row cur_playing_row = *(Dingo::DBManager::trackModel->get_iter(cur_playing_row_ref.get_path()));
  
  std::map<Glib::ustring, Glib::ustring> track_info_map = d_dbmanager->getTrackInfo(cur_playing_row[(*Dingo::DBManager::trackCR).trackID]);
  
  metadata_map.insert(std::pair<Glib::ustring, Glib::VariantBase>("mpris:trackid", Glib::Variant<Glib::ustring>::create("/org/mpris/MediaPlayer2/Track/track" + Dingo::Utilities::intToString(cur_playing_row[(*Dingo::DBManager::trackCR).trackID]))));
  
  metadata_map.insert(std::pair<Glib::ustring, Glib::VariantBase>("mpris:length", Glib::Variant<gint64>::create(Dingo::Utilities::stringToGInt64(track_info_map["TrackTime"]) * 1000)));
  
  metadata_map.insert(std::pair<Glib::ustring, Glib::VariantBase>("mpris:artUrl", Glib::Variant<Glib::ustring>::create(d_playerengine->getPreviewImageURI())));
  
  metadata_map.insert(std::pair<Glib::ustring, Glib::VariantBase>("xesam:album", Glib::Variant<Glib::ustring>::create(track_info_map["AlbumName"])));
  
  metadata_map.insert(std::pair<Glib::ustring, Glib::VariantBase>("xesam:artist", Glib::Variant<Glib::ustring>::create(track_info_map["ArtistName"])));
  
  metadata_map.insert(std::pair<Glib::ustring, Glib::VariantBase>("xesam:albumArtist", Glib::Variant<Glib::ustring>::create(track_info_map["ArtistName"])));
  
  metadata_map.insert(std::pair<Glib::ustring, Glib::VariantBase>("xesam:title", Glib::Variant<Glib::ustring>::create(track_info_map["TrackTitle"])));
  
  metadata_map.insert(std::pair<Glib::ustring, Glib::VariantBase>("xesam:genre", Glib::Variant<Glib::ustring>::create(track_info_map["GenreName"])));
  
  metadata_map.insert(std::pair<Glib::ustring, Glib::VariantBase>("xesam:comment", Glib::Variant<Glib::ustring>::create(track_info_map["TrackComment"])));
  
  metadata_map.insert(std::pair<Glib::ustring, Glib::VariantBase>("xesam:url", Glib::Variant<Glib::ustring>::create(Glib::filename_to_uri(track_info_map["TrackLocation"]))));
  
  metadata_variant = Glib::Variant<std::map<Glib::ustring, Glib::VariantBase> >::create(metadata_map);
  
  return metadata_variant;
}

double Dingo::MPRISCommon::readVolume() {
  return d_playerengine->getVolume();
}

gint64 Dingo::MPRISCommon::readPosition() {
  return d_playerengine->getPosition() / 1000;
}

void Dingo::MPRISCommon::writeLoopStatus(Glib::ustring loop_status) {
  if (loop_status == "None") {
    d_playerengine->setRepeatMode(Dingo::REPEAT_OFF);
  }
    
  else if (loop_status == "Track") {
    d_playerengine->setRepeatMode(Dingo::REPEAT_SINGLE);
  }
    
  else if (loop_status == "Playlist") {
    d_playerengine->setRepeatMode(Dingo::REPEAT_PLAYLIST);
  }
}

void Dingo::MPRISCommon::writeShuffle(bool is_shuffle) {
  if (is_shuffle) {
    d_playerengine->setPlayOrder(Dingo::PLAYORDER_SHUFFLE);
  }
  
  else {
    d_playerengine->setPlayOrder(Dingo::PLAYORDER_NORMAL);
  }
}

void Dingo::MPRISCommon::writeVolume(double volume) {
  d_playerengine->setVolume(volume);
}

//MPRISv2 PLAYLISTS COMMANDS
void Dingo::MPRISCommon::activatePlaylist(Glib::ustring playlist_objpath_id_str) {
  //playlist_objpath_id: /org/mpris/MediaPlayer2/Playlists/playlistid
  bool is_playlist_different = false;
  
  int playlist_id = Dingo::Utilities::stringToInt(playlist_objpath_id_str.substr(41));
  
  if (playlist_id != d_dbmanager->getCurrentPlaylistID()) {
    is_playlist_different = true;
  }
  
  switch (playlist_id) {
    case 0: {
      d_toolbar->activateToggleButton(Dingo::HOME_TOGGLED);
  
      d_playlisttreeview->activatePlaylist(Dingo::Utilities::stringToInt(playlist_objpath_id_str.substr(41)));
    
      break;
    }
    
    case 1: {
      d_toolbar->activateToggleButton(Dingo::PLAY_QUEUE_TOGGLED);
    
      break;
    }
    
    default: {
      d_toolbar->activateToggleButton(Dingo::HOME_TOGGLED);
  
      d_playlisttreeview->activatePlaylist(Dingo::Utilities::stringToInt(playlist_objpath_id_str.substr(41)));

      break;
    }
  }
          
  if (is_playlist_different) {
    d_tracktreeview->activateCurrentPlaylist();
  }
      
  else {
    if (d_playerengine->getStatus() == Dingo::ENGINE_STOPPED) {
      d_tracktreeview->activateCurrentPlaylist();
    }
    
    else if (d_playerengine->getStatus() == Dingo::ENGINE_PAUSED) {
      d_tracktreeview->activateRow(d_dbmanager->getCurrentTrackRowRef());
    }
  }
}

Glib::VariantContainerBase Dingo::MPRISCommon::getPlaylists(int max_count, bool is_reverse) {
  std::map<int, Glib::ustring> playlists_map = d_dbmanager->getPlaylists(max_count, is_reverse);
  
  GVariantBuilder* builder;
  
  builder = g_variant_builder_new(G_VARIANT_TYPE("a(oss)"));
  
  for (std::map<int, Glib::ustring>::iterator iter = playlists_map.begin(); iter != playlists_map.end(); ++iter) {
    //playlist_objpath_id: /org/mpris/MediaPlayer2/Playlist/playlistid  
    Glib::ustring playlist_objpath_id = "/org/mpris/MediaPlayer2/Playlist/playlist" + Dingo::Utilities::intToString(iter->first);
    
    g_variant_builder_add(builder, "(oss)", playlist_objpath_id.c_str(), iter->second.c_str(), iter->second.c_str());
  }
  
  GVariant* return_gvariant = g_variant_new("(a(oss))", builder);
  
  Glib::VariantContainerBase return_variant_container;
  return_variant_container.init(return_gvariant, true);
  
  g_variant_builder_unref(builder);
  
  return return_variant_container;
}

guint32 Dingo::MPRISCommon::readPlaylistCount() {
  return d_dbmanager->getPlaylistsCount();
}

Glib::VariantBase Dingo::MPRISCommon::readActivePlaylist() {
  ++d_active_playlist_changed_count;
  
  std::vector<Glib::VariantBase> oss_vector;
  std::vector<Glib::VariantBase> boss_vector;
  Glib::VariantContainerBase oss_variant_container_base;
  Glib::VariantContainerBase boss_variant_container_base;
  
  if (d_active_playlist_changed_count <= 1) {
    oss_vector.push_back(Glib::Variant<Glib::ustring>::create("/org/mpris/MediaPlayer2/Playlist/playlist"));
    oss_vector.push_back(Glib::Variant<Glib::ustring>::create(_("Choose Playlist")));
    oss_vector.push_back(Glib::Variant<Glib::ustring>::create(""));
  }
  
  else {
    oss_vector.push_back(Glib::Variant<Glib::ustring>::create("/org/mpris/MediaPlayer2/Playlist/playlist" + Dingo::Utilities::intToString(d_dbmanager->getCurrentPlaylistID())));
    oss_vector.push_back(Glib::Variant<Glib::ustring>::create(d_dbmanager->getCurrentPlaylistName()));
    oss_vector.push_back(Glib::Variant<bool>::create(""));
  }
  
  oss_variant_container_base = Glib::VariantContainerBase::create_tuple(oss_vector);
  
  boss_vector.push_back(Glib::Variant<bool>::create(true));
  boss_vector.push_back(oss_variant_container_base);
  
  boss_variant_container_base = Glib::VariantContainerBase::create_tuple(boss_vector);
  
  return boss_variant_container_base;
}
