/* 
 * dingo-menubar.cc
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

#include "dingo-menubar.h"
#include "dingo-dbmanager.h"
#include "dingo-playerengine.h"
#include "dingo-settings.h"
#include "dingo-utilities.h"

#include "dingo-playlisttreeview.h"
#include "dingo-genretreeview.h"
#include "dingo-artisttreeview.h"
#include "dingo-yeartreeview.h"
#include "dingo-albumtreeview.h"
#include "dingo-tracktreeview.h"
#include "dingo-toolbar.h"
#include "dingo-controlbar.h"
#include "dingo-statusbar.h"
#include "dingo-statusicon.h"
#include "dingo-aboutdialog.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

#ifndef INCLUDED_GLIBMM_I18N_H
#define INCLUDED_GLIBMM_I18N_H
#include <glibmm/i18n.h>
#endif //INCLUDED_GLIBMM_I18N_H

//CONSTRUCTORS - DESTRUCTOR
Dingo::MenuBar::MenuBar(Dingo::DBManager* dbmanager, Dingo::PlayerEngine* playerengine, Dingo::Settings* settings, Gtk::Application* dingo_app) :
  Gtk::VBox()
{
  d_dbmanager = dbmanager;
  d_playerengine = playerengine;
  d_settings = settings;
  d_dingo_app = dingo_app;
  
  is_audio_only_toggled_here = false;
  is_video_only_toggled_here = false;
  is_fullscreen_toggled_here = false;
  is_play_toggled_here = false;
  is_playorder_toggled_here = false;
  is_repeat_toggled_here = false;
  
  d_import_files_folder_uri = Glib::filename_to_uri(Glib::get_home_dir());
  d_import_folders_folder_uri = Glib::filename_to_uri(Glib::get_home_dir());
  d_load_settings_folder_uri = Glib::filename_to_uri(Glib::get_home_dir());
  d_save_settings_folder_uri = Glib::filename_to_uri(Glib::get_home_dir());
  d_open_subtitle_folder_uri = Glib::filename_to_uri(Glib::get_home_dir());
  
  createMenuBar();
    
  show_all();
  
  d_dbmanager->getWindow()->add_accel_group(d_uimanager->get_accel_group());
}

Dingo::MenuBar::~MenuBar() {

}

//PUBLIC ACCESSORS
void Dingo::MenuBar::setPlaylistTreeView(Dingo::PlaylistTreeView* playlisttreeview, Gtk::ScrolledWindow* scrolled_win, Gtk::HPaned* playlist_hpaned) {
  d_playlisttreeview = playlisttreeview;
  
  d_playlistscrolled = scrolled_win;
  
  d_playlist_hpaned = playlist_hpaned;
}

void Dingo::MenuBar::setTrackTreeView(Dingo::TrackTreeView* tracktreeview, Gtk::ScrolledWindow* scrolled_win) {
  d_tracktreeview = tracktreeview;
  
  d_trackscrolled = scrolled_win;
}

void Dingo::MenuBar::setGenreTreeView(Dingo::GenreTreeView* genretreeview, Gtk::ScrolledWindow* scrolled_win, Gtk::HPaned* genre_hpaned) {
  d_genretreeview = genretreeview;
  
  d_genrescrolled = scrolled_win;
  
  d_genre_hpaned = genre_hpaned;
}

void Dingo::MenuBar::setArtistTreeView(Dingo::ArtistTreeView* artisttreeview, Gtk::ScrolledWindow* scrolled_win, Gtk::HPaned* artist_hpaned) {
  d_artisttreeview = artisttreeview;
  
  d_artistscrolled = scrolled_win;
  
  d_artist_hpaned = artist_hpaned;
}

void Dingo::MenuBar::setYearTreeView(Dingo::YearTreeView* yeartreeview, Gtk::ScrolledWindow* scrolled_win, Gtk::HPaned* year_hpaned) {
  d_yeartreeview = yeartreeview;
  
  d_yearscrolled = scrolled_win;
  
  d_year_hpaned = year_hpaned;
}

void Dingo::MenuBar::setAlbumTreeView(Dingo::AlbumTreeView* albumtreeview, Gtk::ScrolledWindow* scrolled_win) {
  d_albumtreeview = albumtreeview;
  
  d_albumscrolled = scrolled_win;
}

void Dingo::MenuBar::setToolBar(Dingo::ToolBar* toolbar) {
  d_toolbar = toolbar;
}

void Dingo::MenuBar::setControlBar(Dingo::ControlBar* controlbar) {
  d_controlbar = controlbar;
}

void Dingo::MenuBar::setStatusBar(Dingo::StatusBar* statusbar) {
  d_statusbar = statusbar;
}

void Dingo::MenuBar::setStatusIcon(Dingo::StatusIcon* statusicon) {
  d_statusicon = statusicon;
}

//INHERITED OBSERVER'S METHODS
void Dingo::MenuBar::updateHomeToggleActive() {
  view_playlist_browser_connection.block();
  view_track_browser_connection.block();
  view_genre_filter_connection.block();
  view_artist_filter_connection.block();
  view_year_filter_connection.block();
  view_album_filter_connection.block();
  
  view_playlist_browser->set_active(true);
  view_track_browser->set_active(true);
  view_genre_filter->set_active(true);
  view_artist_filter->set_active(true);
  view_year_filter->set_active(true);
  view_album_filter->set_active(true);
  
  view_playlist_browser->set_sensitive(true);
  view_track_browser->set_sensitive(true);
  view_genre_filter->set_sensitive(true);
  view_artist_filter->set_sensitive(true);
  view_year_filter->set_sensitive(true);
  view_album_filter->set_sensitive(true);
  
  view_playlist_browser_connection.unblock();
  view_track_browser_connection.unblock();
  view_genre_filter_connection.unblock();
  view_artist_filter_connection.unblock();
  view_year_filter_connection.unblock();
  view_album_filter_connection.unblock();
}

void Dingo::MenuBar::updateNowPlayingToggleActive() {
  view_playlist_browser_connection.block();
  view_track_browser_connection.block();
  view_genre_filter_connection.block();
  view_artist_filter_connection.block();
  view_year_filter_connection.block();
  view_album_filter_connection.block();
  
  view_playlist_browser->set_active(false);
  view_track_browser->set_active(false);
  view_genre_filter->set_active(false);
  view_artist_filter->set_active(false);
  view_year_filter->set_active(false);
  view_album_filter->set_active(false);
  
  view_playlist_browser->set_sensitive(false);
  view_track_browser->set_sensitive(false);
  view_genre_filter->set_sensitive(false);
  view_artist_filter->set_sensitive(false);
  view_year_filter->set_sensitive(false);
  view_album_filter->set_sensitive(false);
  
  view_playlist_browser_connection.unblock();
  view_track_browser_connection.unblock();
  view_genre_filter_connection.unblock();
  view_artist_filter_connection.unblock();
  view_year_filter_connection.unblock();
  view_album_filter_connection.unblock();
}

void Dingo::MenuBar::updatePlayQueueToggleActive() {
  view_playlist_browser_connection.block();
  view_track_browser_connection.block();
  view_genre_filter_connection.block();
  view_artist_filter_connection.block();
  view_year_filter_connection.block();
  view_album_filter_connection.block();
  
  view_playlist_browser->set_active(false);
  view_track_browser->set_active(true);
  view_genre_filter->set_active(false);
  view_artist_filter->set_active(false);
  view_year_filter->set_active(false);
  view_album_filter->set_active(false);
  
  view_playlist_browser->set_sensitive(false);
  view_track_browser->set_sensitive(true);
  view_genre_filter->set_sensitive(true);
  view_artist_filter->set_sensitive(true);
  view_year_filter->set_sensitive(true);
  view_album_filter->set_sensitive(true);
  
  view_playlist_browser_connection.unblock();
  view_track_browser_connection.unblock();
  view_genre_filter_connection.unblock();
  view_artist_filter_connection.unblock();
  view_year_filter_connection.unblock();
  view_album_filter_connection.unblock();
}

void Dingo::MenuBar::updateAudioFilterToggled() {
  if (is_audio_only_toggled_here) {
    is_audio_only_toggled_here = false;
  }
  
  else {
    view_audio_only_connection.block();
    
    if (view_audio_only->get_active()) {
      view_audio_only->set_active(false);
    }
    
    else {
      view_audio_only->set_active(true);
    }
    
    view_audio_only_connection.unblock();
  }
}

void Dingo::MenuBar::updateVideoFilterToggled() {
  if (is_video_only_toggled_here) {
    is_video_only_toggled_here = false;
  }
  
  else {
    view_video_only_connection.block();
    
    if (view_video_only->get_active()) {
      view_video_only->set_active(false);
    }
    
    else {
      view_video_only->set_active(true);
    }
    
    view_video_only_connection.unblock();
  }
}

void Dingo::MenuBar::updateFullscreenActivated() {
  if (is_fullscreen_toggled_here) {
    is_fullscreen_toggled_here = false;
  }
  
  else {
    view_fullscreen_connection.block();
    
    view_fullscreen->set_active(true);
    
    view_fullscreen_connection.unblock();
  }
}

void Dingo::MenuBar::updateUnFullscreenActivated() {
  if (is_fullscreen_toggled_here) {
    is_fullscreen_toggled_here = false;
  }
  
  else {
    view_fullscreen_connection.block();
  
    view_fullscreen->set_active(false);
    
    view_fullscreen_connection.unblock();
  }
}

void Dingo::MenuBar::updateEnginePlaying() {
  if (is_play_toggled_here) {
    is_play_toggled_here = false;
  }
  
  else {
    control_play_connection.block();
    
    control_play->set_active(true);
    
    control_play_connection.unblock();
  }
  
  control_subtitle->set_sensitive(true);
}

void Dingo::MenuBar::updateEnginePaused() {
  if (is_play_toggled_here) {
    is_play_toggled_here = false;
  }
  
  else {
    control_play_connection.block();
    
    control_play->set_active(false);
    
    control_play_connection.unblock();
  }
  
  control_subtitle->set_sensitive(true);
}

void Dingo::MenuBar::updateEngineStopped() {
  if (is_play_toggled_here) {
    is_play_toggled_here = false;
  }
  
  else {
    control_play_connection.block();
    
    control_play->set_active(false);
    
    control_play_connection.unblock();
  }
  
  control_subtitle->set_sensitive(false);
}

void Dingo::MenuBar::updateEngineNewPlaying() {
  updateEnginePlaying();
}

void Dingo::MenuBar::updatePlayOrderNormal() {
  if (is_playorder_toggled_here) {
    is_playorder_toggled_here = false;
  }
  
  else {
    control_shuffle_connection.block();
    
    control_shuffle->set_active(false);
    
    control_shuffle_connection.unblock();
  }
}

void Dingo::MenuBar::updatePlayOrderShuffle() {
  if (is_playorder_toggled_here) {
    is_playorder_toggled_here = false;
  }
  
  else {
    control_shuffle_connection.block();
    
    control_shuffle->set_active(true);
    
    control_shuffle_connection.unblock();
  }
}

void Dingo::MenuBar::updateRepeatOffToggled() {
  if (is_repeat_toggled_here) {
    is_repeat_toggled_here = false;
  }
  
  else {
    control_repeat_off_connection.block();
    control_repeat_single_connection.block();
    control_repeat_playlist_connection.block();
    control_repeat_section_connection.block();
    
    control_repeat_off->set_active(true);
    
    control_repeat_off_connection.unblock();
    control_repeat_single_connection.unblock();
    control_repeat_playlist_connection.unblock();
    control_repeat_section_connection.unblock();
  }
  
  control_repeat_section->set_sensitive(false);
  
  control_repeat_section->set_label(_("Repeat Section"));
}

void Dingo::MenuBar::updateRepeatSingleToggled() {
  if (is_repeat_toggled_here) {
    is_repeat_toggled_here = false;
  }
  
  else {
    control_repeat_off_connection.block();
    control_repeat_single_connection.block();
    control_repeat_playlist_connection.block();
    control_repeat_section_connection.block();
    
    control_repeat_single->set_active(true);
    
    control_repeat_off_connection.unblock();
    control_repeat_single_connection.unblock();
    control_repeat_playlist_connection.unblock();
    control_repeat_section_connection.unblock();
  }
  
  control_repeat_section->set_sensitive(false);
  
  control_repeat_section->set_label(_("Repeat Section"));
}

void Dingo::MenuBar::updateRepeatPlaylistToggled() {
  if (is_repeat_toggled_here) {
    is_repeat_toggled_here = false;
  }
  
  else {
    control_repeat_off_connection.block();
    control_repeat_single_connection.block();
    control_repeat_playlist_connection.block();
    control_repeat_section_connection.block();
    
    control_repeat_playlist->set_active(true);
    
    control_repeat_off_connection.unblock();
    control_repeat_single_connection.unblock();
    control_repeat_playlist_connection.unblock();
    control_repeat_section_connection.unblock();
  }
  
  control_repeat_section->set_sensitive(false);
  
  control_repeat_section->set_label(_("Repeat Section"));
}

void Dingo::MenuBar::updateRepeatSectionToggled(Glib::ustring section_button_code) {
  control_repeat_section->set_sensitive(true);
    
  if (is_repeat_toggled_here) {
    is_repeat_toggled_here = false;
  }
  
  else {
    control_repeat_off_connection.block();
    control_repeat_single_connection.block();
    control_repeat_playlist_connection.block();
    control_repeat_section_connection.block();
    
    control_repeat_section->set_active(true);
    
    control_repeat_off_connection.unblock();
    control_repeat_single_connection.unblock();
    control_repeat_playlist_connection.unblock();
    control_repeat_section_connection.unblock();
  }
  
  control_repeat_section->set_label(Glib::ustring::compose(_("Repeat %1 -> %2"), d_playerengine->getSectionStartString(), d_playerengine->getSectionEndString()));
}

void Dingo::MenuBar::updateSettingsRead() {
  //set subtitle show
  control_subtitle_show->set_active(d_settings->getSubtitleShow());
  
  //set subtitle font
  if (!d_settings->getSubtitleFontDesc().empty()) {
    d_playerengine->setSubtitleFontDesc(d_settings->getSubtitleFontDesc());
  }
  
  //set visible
  view_playlist_browser->set_active(d_settings->getVisible("playlistbrowser"));
  view_track_browser->set_active(d_settings->getVisible("trackbrowser"));
  view_genre_filter->set_active(d_settings->getVisible("genrefilter"));
  view_artist_filter->set_active(d_settings->getVisible("artistfilter"));
  view_year_filter->set_active(d_settings->getVisible("yearfilter"));
  view_album_filter->set_active(d_settings->getVisible("albumfilter"));
  view_playback_status->set_active(d_settings->getVisible("playbackstatus"));
  view_track_number->set_active(d_settings->getVisible("tracknumber"));
  view_track_title->set_active(d_settings->getVisible("tracktitle"));
  view_track_genre->set_active(d_settings->getVisible("trackgenre"));
  view_track_artist->set_active(d_settings->getVisible("trackartist"));
  view_track_album->set_active(d_settings->getVisible("trackalbum"));
  view_track_year->set_active(d_settings->getVisible("trackyear"));
  view_track_time->set_active(d_settings->getVisible("tracktime"));
  view_track_extension->set_active(d_settings->getVisible("trackfileextension"));
  view_track_bitrate->set_active(d_settings->getVisible("trackbitrate"));
}

void Dingo::MenuBar::updateSettingsWrite() {
  //write subtitle show
  d_settings->setSubtitleShow(control_subtitle_show->get_active());
  
  //write subtitle font
  d_settings->setSubtitleFontDesc(d_playerengine->getSubtitleFontDesc());
  
  //write visible
  d_settings->setVisible("playlistbrowser", view_playlist_browser->get_active());
  d_settings->setVisible("trackbrowser", view_track_browser->get_active());
  d_settings->setVisible("genrefilter", view_genre_filter->get_active());
  d_settings->setVisible("artistfilter", view_artist_filter->get_active());
  d_settings->setVisible("yearfilter", view_year_filter->get_active());
  d_settings->setVisible("albumfilter", view_album_filter->get_active());
  d_settings->setVisible("playbackstatus", view_playback_status->get_active());
  d_settings->setVisible("tracknumber", view_track_number->get_active());
  d_settings->setVisible("tracktitle", view_track_title->get_active());
  d_settings->setVisible("trackgenre", view_track_genre->get_active());
  d_settings->setVisible("trackartist", view_track_artist->get_active());
  d_settings->setVisible("trackalbum", view_track_album->get_active());
  d_settings->setVisible("trackyear", view_track_year->get_active());
  d_settings->setVisible("tracktime", view_track_time->get_active());
  d_settings->setVisible("trackfileextension", view_track_extension->get_active());
  d_settings->setVisible("trackbitrate", view_track_bitrate->get_active());
}
    
//CREATORS
void Dingo::MenuBar::createMenuBar() {
  d_actiongroup = Gtk::ActionGroup::create();
  
  createMediaMenu();
  createEditMenu();
  createViewMenu();
  createControlMenu();
  createHelpMenu();
  
  d_uimanager = Gtk::UIManager::create();
  d_uimanager->insert_action_group(d_actiongroup);
  
  Glib::ustring ui_info = 
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='MediaMenu'>"
    "      <menuitem action='MediaImportFile'/>"
    "      <menuitem action='MediaImportFolder'/>"
    "      <separator/>"
    "      <menu action='MediaPlaylistSubMenu'>"
    "        <menuitem action='MediaNewEmptyPlaylist'/>"
    "        <menuitem action='MediaImportFromFiles'/>"
    "        <separator/>"
    "        <menuitem action='MediaRenameSelectedPlaylist'/>"
    "        <menuitem action='MediaExportToFile'/>"
    "        <separator/>"
    "        <menuitem action='MediaDeleteSelectedPlaylist'/>"
    "      </menu>"
    "      <separator/>"
    "      <menuitem action='MediaClose'/>"
    "      <separator/>"
    "      <menuitem action='MediaQuit'/>"
    "    </menu>"
    "    <menu action='EditMenu'>"
    "      <menuitem action='EditCutTracks'/>"
    "      <menuitem action='EditCopyTracks'/>"
    "      <menuitem action='EditPasteTracks'/>"
    "      <separator/>"
    "      <menu action='EditAddToSubMenu'>"
    "        <menuitem action='EditAddToPlayQueue'/>"
    "        <separator/>"
    "        <menuitem action='EditAddToPlaylists'/>"
    "      </menu>"
    "      <menu action='EditDeleteFromSubMenu'>"
    "        <menuitem action='EditDeleteFromDatabase'/>"
    "        <menuitem action='EditDeleteFromPlayQueue'/>"
    "        <separator/>"
    "        <menuitem action='EditDeleteFromHardDrive'/>"
    "        <separator/>"
    "        <menuitem action='EditDeleteFromPlaylists'/>"
    "        <separator/>"
    "        <menuitem action='EditDeleteFromCurrentPlaylist'/>"
    "      </menu>"
    "      <separator/>"
    "      <menuitem action='EditSingleTrack'/>"
    "      <menuitem action='EditAllSelectedTracks'/>"
    "      <separator/>"
    "      <menu action='EditSettingsSubMenu'>"
    "        <menuitem action='EditSettingsLoadFromFile'/>"
    "        <separator/>"
    "        <menuitem action='EditSettingsSaveToFile'/>"
    "        <menuitem action='EditSettingsSaveCurrentAsDefault'/>"
    "        <separator/>"
    "        <menuitem action='EditSettingsRestoreToDefault'/>"
    "      </menu>"
    "    </menu>"
    "    <menu action='ViewMenu'>"
    "      <menu action='ViewBrowsersSubMenu'>"
    "        <menuitem action='ViewPlaylistBrowser'/>"
    "        <menuitem action='ViewTrackBrowser'/>"
    "      </menu>"
    "      <menu action='ViewFiltersSubMenu'>"
    "        <menuitem action='ViewGenreFilter'/>"
    "        <menuitem action='ViewArtistFilter'/>"
    "        <separator/>"
    "        <menuitem action='ViewYearFilter'/>"
    "        <menuitem action='ViewAlbumFilter'/>"
    "      </menu>"
    "      <menu action='ViewColumnsSubMenu'>"
    "        <menuitem action='ViewPlaybackStatus'/>"
    "        <menuitem action='ViewTrackNumber'/>"
    "        <separator/>"
    "        <menuitem action='ViewTrackTitle'/>"
    "        <menuitem action='ViewTrackGenre'/>"
    "        <menuitem action='ViewTrackArtist'/>"
    "        <menuitem action='ViewTrackAlbum'/>"
    "        <separator/>"
    "        <menuitem action='ViewTrackYear'/>"
    "        <menuitem action='ViewTrackTime'/>"
    "        <menuitem action='ViewTrackExtension'/>"
    "        <separator/>"
    "        <menuitem action='ViewTrackBitrate'/>"
    "      </menu>"
    "      <separator/>"
    "      <menuitem action='ViewAudioOnly'/>"
    "      <menuitem action='ViewVideoOnly'/>"
    "      <separator/>"
    "      <menuitem action='ViewFullscreen'/>"
    "      <menuitem action='ViewLyrics'/>"
    "      <separator/>"
    "    </menu>"
    "    <menu action='ControlMenu'>"
    "      <menuitem action='ControlPlay'/>"
    "      <separator/>"
    "      <menuitem action='ControlPrevious'/>"
    "      <menuitem action='ControlNext'/>"
    "      <separator/>"
    "      <menu action='ControlRepeat'>"
    "        <menuitem action='ControlRepeatOff'/>"
    "        <separator/>"
    "        <menuitem action='ControlRepeatPlaylist'/>"
    "        <menuitem action='ControlRepeatSingle'/>"
    "        <separator/>"
    "        <menuitem action='ControlRepeatSection'/>"
    "      </menu>"
    "      <menuitem action='ControlShuffle'/>"
    "      <separator/>"
    "      <menu action='ControlSubtitleSubMenu'>"
    "        <menuitem action='ControlSubtitleLoadFile'/>"
    "        <separator/>"
    "        <menuitem action='ControlSubtitleUnsetSaved'/>"
    "        <separator/>"
    "        <menuitem action='ControlSubtitleShow'/>"
    "        <menuitem action='ControlSubtitleChooseFont'/>"
    "      </menu>"
    "    </menu>"
    "    <menu action='HelpMenu'>"
    "      <menuitem action='HelpHomePage'/>"
    "      <menuitem action='HelpGetInvolved'/>"
    "      <separator/>"
    "      <menuitem action='HelpAbout'/>"
    "    </menu>"
    "  </menubar>"
    "</ui>";
    
  try {
    d_uimanager->add_ui_from_string(ui_info);
  }
    
  catch (const Glib::Error& ex) {
    std::cerr << "Building MenuBar failed: " << ex.what() << std::endl;
  }
    
  Gtk::Widget* p_menubar = d_uimanager->get_widget("/MenuBar");
  if (p_menubar) {
    pack_start(*p_menubar, Gtk::PACK_SHRINK);
  }
}

void Dingo::MenuBar::createMediaMenu() {
  d_actiongroup->add(Gtk::Action::create("MediaMenu", _("Media")));
  d_actiongroup->add(Gtk::Action::create("MediaImportFile", _("Import File(s)...")), Gtk::AccelKey("<control>I"), sigc::mem_fun(*this, &Dingo::MenuBar::onMediaImportFileActivated));
  d_actiongroup->add(Gtk::Action::create("MediaImportFolder", _("Import Folder(s)...")), Gtk::AccelKey("<control>F"), sigc::mem_fun(*this, &Dingo::MenuBar::onMediaImportFolderActivated));
  d_actiongroup->add(Gtk::Action::create("MediaPlaylistSubMenu", _("Playlist")), sigc::mem_fun(*this, &Dingo::MenuBar::onMediaPlaylistSubMenuActivated));
  d_actiongroup->add(Gtk::Action::create("MediaNewEmptyPlaylist", _("New Empty Playlist...")), Gtk::AccelKey("<control>N"), sigc::mem_fun(*this, &Dingo::MenuBar::onMediaNewEmptyPlaylistActivated));
  d_actiongroup->add(Gtk::Action::create("MediaImportFromFiles", _("Import From File(s)...")), Gtk::AccelKey("<control><shift>I"), sigc::mem_fun(*this, &Dingo::MenuBar::onMediaImportFromFilesActivated));
  
  media_rename_selected_playlist = Gtk::Action::create("MediaRenameSelectedPlaylist", _("Rename Selected Playlist"));
  d_actiongroup->add(media_rename_selected_playlist, Gtk::AccelKey("<control>R"), sigc::mem_fun(*this, &Dingo::MenuBar::onMediaRenameSelectedPlaylistActivated));
  
  d_actiongroup->add(Gtk::Action::create("MediaExportToFile", _("Export To File...")), Gtk::AccelKey("<control><shift>E"), sigc::mem_fun(*this, &Dingo::MenuBar::onMediaExportToFileActivated));
  
  media_delete_selected_playlist = Gtk::Action::create("MediaDeleteSelectedPlaylist", _("Delete Selected Playlist"));
  d_actiongroup->add(media_delete_selected_playlist, Gtk::AccelKey("<control>D"), sigc::mem_fun(*this, &Dingo::MenuBar::onMediaDeleteSelectedPlaylistActivated));
  
  d_actiongroup->add(Gtk::Action::create("MediaClose", _("Close")), Gtk::AccelKey("<ctrl>W"), sigc::mem_fun(*this, &Dingo::MenuBar::onMediaCloseActivated));
  d_actiongroup->add(Gtk::Action::create("MediaQuit", _("Quit")), Gtk::AccelKey("<ctrl>Q"), sigc::mem_fun(*this, &Dingo::MenuBar::onMediaQuitActivated));
}

void Dingo::MenuBar::createEditMenu() {
  edit_action = Gtk::Action::create("EditMenu", _("Edit"));
  d_actiongroup->add(edit_action, sigc::mem_fun(*this, &Dingo::MenuBar::onEditMenuActivated));
  
  edit_cut = Gtk::Action::create("EditCutTracks", _("Cut Track(s)"));
  d_actiongroup->add(edit_cut, Gtk::AccelKey("<alt>X"), sigc::mem_fun(*this, &Dingo::MenuBar::onEditCutTracksActivated));
  
  edit_copy = Gtk::Action::create("EditCopyTracks", _("Copy Track(s)"));
  d_actiongroup->add(edit_copy, Gtk::AccelKey("<alt>C"), sigc::mem_fun(*this, &Dingo::MenuBar::onEditCopyTracksActivated));
  
  edit_paste = Gtk::Action::create("EditPasteTracks", _("Paste Track(s)"));
  d_actiongroup->add(edit_paste, Gtk::AccelKey("<alt>V"), sigc::mem_fun(*this, &Dingo::MenuBar::onEditPasteTracksActivated));
  
  edit_add_to = Gtk::Action::create("EditAddToSubMenu", _("Add to..."));
  d_actiongroup->add(edit_add_to, sigc::mem_fun(*this, &Dingo::MenuBar::onEditAddToSubMenuActivated));
  
  edit_add_to_play_queue = Gtk::Action::create("EditAddToPlayQueue", _("Play Queue"));
  d_actiongroup->add(edit_add_to_play_queue, Gtk::AccelKey("<control><alt>Q"), sigc::mem_fun(*this, &Dingo::MenuBar::onEditAddToPlayQueueActivated));
  
  d_actiongroup->add(Gtk::Action::create("EditAddToPlaylists", _("Playlist(s)...")), Gtk::AccelKey("<control><alt>A"), sigc::mem_fun(*this, &Dingo::MenuBar::onEditAddToPlaylistsActivated));
  
  edit_delete_from = Gtk::Action::create("EditDeleteFromSubMenu", _("Delete from..."));
  d_actiongroup->add(edit_delete_from, sigc::mem_fun(*this, &Dingo::MenuBar::onEditDeleteFromSubMenuActivated));
  
  d_actiongroup->add(Gtk::Action::create("EditDeleteFromDatabase", _("Database")), Gtk::AccelKey("<control><shift>B"), sigc::mem_fun(*this, &Dingo::MenuBar::onEditDeleteFromDatabaseActivated));
  
  edit_delete_from_play_queue = Gtk::Action::create("EditDeleteFromPlayQueue", _("Play Queue"));
  d_actiongroup->add(edit_delete_from_play_queue, Gtk::AccelKey("<control><shift>Q"), sigc::mem_fun(*this, &Dingo::MenuBar::onEditDeleteFromPlayQueueActivated));
  
  d_actiongroup->add(Gtk::Action::create("EditDeleteFromHardDrive", _("Hard Drive")), Gtk::AccelKey("<control><shift>D"), sigc::mem_fun(*this, &Dingo::MenuBar::onEditDeleteFromHardDriveActivated));
  d_actiongroup->add(Gtk::Action::create("EditDeleteFromPlaylists", _("Playlist(s)...")), Gtk::AccelKey("<control><shift>P"), sigc::mem_fun(*this, &Dingo::MenuBar::onEditDeleteFromPlaylistsActivated));
  
  edit_delete_from_current_playlist = Gtk::Action::create("EditDeleteFromCurrentPlaylist", _("Current Playlist"));
  d_actiongroup->add(edit_delete_from_current_playlist, Gtk::AccelKey("<control><shift>C"), sigc::mem_fun(*this, &Dingo::MenuBar::onEditDeleteFromCurrentPlaylistActivated));
  
  edit_single_track = Gtk::Action::create("EditSingleTrack", _("Edit Single Track"));
  d_actiongroup->add(edit_single_track, Gtk::AccelKey("<control>E"), sigc::mem_fun(*this, &Dingo::MenuBar::onEditSingleTrackActivated));
  
  edit_all_selected_tracks = Gtk::Action::create("EditAllSelectedTracks", _("Edit All Selected Tracks"));
  d_actiongroup->add(edit_all_selected_tracks, Gtk::AccelKey("<control><shift>E"), sigc::mem_fun(*this, &Dingo::MenuBar::onEditAllSelectedTracksActivated));
  
  d_actiongroup->add(Gtk::Action::create("EditSettingsSubMenu", _("Settings")));
  d_actiongroup->add(Gtk::Action::create("EditSettingsLoadFromFile", _("Load from File...")), sigc::mem_fun(*this, &Dingo::MenuBar::onEditSettingsLoadFromFileActivated));
  d_actiongroup->add(Gtk::Action::create("EditSettingsSaveToFile", _("Save to File...")), sigc::mem_fun(*this, &Dingo::MenuBar::onEditSettingsSaveToFileActivated));
  d_actiongroup->add(Gtk::Action::create("EditSettingsSaveCurrentAsDefault", _("Save Current as Default")), sigc::mem_fun(*this, &Dingo::MenuBar::onEditSettingsSaveCurrentAsDefaultActivated));
  d_actiongroup->add(Gtk::Action::create("EditSettingsRestoreToDefault", _("Restore to Default")), sigc::mem_fun(*this, &Dingo::MenuBar::onEditSettingsRestoreToDefaultActivated));
}

void Dingo::MenuBar::createViewMenu() {
  d_actiongroup->add(Gtk::Action::create("ViewMenu", _("View")));
  
  d_actiongroup->add(Gtk::Action::create("ViewBrowsersSubMenu", _("Browsers")));
  
  view_playlist_browser = Gtk::ToggleAction::create("ViewPlaylistBrowser", _("Playlist Browser"));
  view_playlist_browser->set_active(true);
  d_actiongroup->add(view_playlist_browser);
  view_playlist_browser_connection = view_playlist_browser->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewPlaylistBrowserToggled));
  
  view_track_browser = Gtk::ToggleAction::create("ViewTrackBrowser", _("Track Browser"));
  view_track_browser->set_active(true);
  d_actiongroup->add(view_track_browser);
  view_track_browser_connection = view_track_browser->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewTrackBrowserToggled));
  
  d_actiongroup->add(Gtk::Action::create("ViewFiltersSubMenu", _("Filters")));
  
  view_genre_filter = Gtk::ToggleAction::create("ViewGenreFilter", _("Genre Filter"));
  view_genre_filter->set_active(true);
  d_actiongroup->add(view_genre_filter);
  view_genre_filter_connection = view_genre_filter->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewGenreFilterToggled));
  
  view_artist_filter = Gtk::ToggleAction::create("ViewArtistFilter", _("Artist Filter"));
  view_artist_filter->set_active(true);
  d_actiongroup->add(view_artist_filter);
  view_artist_filter_connection = view_artist_filter->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewArtistFilterToggled));
  
  view_year_filter = Gtk::ToggleAction::create("ViewYearFilter", _("Year Filter"));
  view_year_filter->set_active(true);
  d_actiongroup->add(view_year_filter);
  view_year_filter_connection = view_year_filter->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewYearFilterToggled));
  
  view_album_filter = Gtk::ToggleAction::create("ViewAlbumFilter", _("Album Filter"));
  view_album_filter->set_active(true);
  d_actiongroup->add(view_album_filter);
  view_album_filter_connection = view_album_filter->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewAlbumFilterToggled));
  
  d_actiongroup->add(Gtk::Action::create("ViewColumnsSubMenu", _("Columns")));
  
  view_playback_status = Gtk::ToggleAction::create("ViewPlaybackStatus", _("Playback Status"));
  view_playback_status->set_active(true);
  d_actiongroup->add(view_playback_status);
  view_playback_status_connection = view_playback_status->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewPlaybackStatusToggled));
  
  view_track_number = Gtk::ToggleAction::create("ViewTrackNumber", _("Track Number"));
  view_track_number->set_active(true);
  d_actiongroup->add(view_track_number);
  view_track_number_connection = view_track_number->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewTrackNumberToggled));
  
  view_track_title = Gtk::ToggleAction::create("ViewTrackTitle", _("Track Title"));
  view_track_title->set_active(true);
  d_actiongroup->add(view_track_title);
  view_track_title_connection = view_track_title->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewTrackTitleToggled));
  
  view_track_genre = Gtk::ToggleAction::create("ViewTrackGenre", _("Track Genre"));
  view_track_genre->set_active(true);
  d_actiongroup->add(view_track_genre);
  view_track_genre_connection = view_track_genre->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewTrackGenreToggled));
  
  view_track_artist = Gtk::ToggleAction::create("ViewTrackArtist", _("Track Artist"));
  view_track_artist->set_active(true);
  d_actiongroup->add(view_track_artist);
  view_track_artist_connection = view_track_artist->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewTrackArtistToggled));
  
  view_track_album = Gtk::ToggleAction::create("ViewTrackAlbum", _("Track Album"));
  view_track_album->set_active(true);
  d_actiongroup->add(view_track_album);
  view_track_album_connection = view_track_album->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewTrackAlbumToggled));
  
  view_track_year = Gtk::ToggleAction::create("ViewTrackYear", _("Track Year"));
  view_track_year->set_active(true);
  d_actiongroup->add(view_track_year);
  view_track_year_connection = view_track_year->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewTrackYearToggled));
  
  view_track_time = Gtk::ToggleAction::create("ViewTrackTime", _("Track Time"));
  view_track_time->set_active(true);
  d_actiongroup->add(view_track_time);
  view_track_time_connection = view_track_time->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewTrackTimeToggled));
  
  view_track_extension = Gtk::ToggleAction::create("ViewTrackExtension", _("Track Extension"));
  view_track_extension->set_active(true);
  d_actiongroup->add(view_track_extension);
  view_track_extension_connection = view_track_extension->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewTrackExtensionToggled));
  
  view_track_bitrate = Gtk::ToggleAction::create("ViewTrackBitrate", _("Track Quality"));
  view_track_bitrate->set_active(true);
  d_actiongroup->add(view_track_bitrate);
  view_track_bitrate_connection = view_track_bitrate->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewTrackBitrateToggled));
  
  view_audio_only = Gtk::ToggleAction::create("ViewAudioOnly", _("Audio Only"));
  d_actiongroup->add(view_audio_only, Gtk::AccelKey("<alt>A"));
  view_audio_only_connection = view_audio_only->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewAudioOnlyToggled));
  
  view_video_only = Gtk::ToggleAction::create("ViewVideoOnly", _("Video Only"));
  d_actiongroup->add(view_video_only, Gtk::AccelKey("<alt>V"));
  view_video_only_connection = view_video_only->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewVideoOnlyToggled));
  
  view_fullscreen = Gtk::ToggleAction::create("ViewFullscreen", _("Fullscreen"));
  d_actiongroup->add(view_fullscreen, Gtk::AccelKey("<alt>F"));
  view_fullscreen_connection = view_fullscreen->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onViewFullscreenToggled));
  d_actiongroup->add(Gtk::Action::create("ViewLyrics", _("Lyrics")), Gtk::AccelKey("<alt>L"), sigc::mem_fun(*this, &Dingo::MenuBar::onViewLyricsActivated));
}

void Dingo::MenuBar::createControlMenu() {
  control_action = Gtk::Action::create("ControlMenu", _("Control"));
  d_actiongroup->add(control_action, sigc::mem_fun(*this, &Dingo::MenuBar::onControlMenuActivated));
  
  control_play = Gtk::ToggleAction::create("ControlPlay", _("Play"));
  d_actiongroup->add(control_play, Gtk::AccelKey("<alt>space"));
  control_play_connection = control_play->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onControlPlayToggled));
  
  d_actiongroup->add(Gtk::Action::create("ControlPrevious", _("Previous")), Gtk::AccelKey("<alt>P"), sigc::mem_fun(*this, &Dingo::MenuBar::onControlPreviousActivated));
  d_actiongroup->add(Gtk::Action::create("ControlNext", _("Next")), Gtk::AccelKey("<alt>N"), sigc::mem_fun(*this, &Dingo::MenuBar::onControlNextActivated));
  
  d_actiongroup->add(Gtk::Action::create("ControlRepeat", _("Repeat")));
  
  Gtk::RadioAction::Group control_repeat_group;
  control_repeat_off = Gtk::RadioAction::create(control_repeat_group, "ControlRepeatOff", _("Repeat Off"));
  d_actiongroup->add(control_repeat_off);
  control_repeat_off_connection = control_repeat_off->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onControlRepeatModeToggled));
  
  control_repeat_playlist = Gtk::RadioAction::create(control_repeat_group, "ControlRepeatPlaylist", _("Repeat Playlist"));
  d_actiongroup->add(control_repeat_playlist);
  control_repeat_playlist_connection = control_repeat_playlist->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onControlRepeatModeToggled));
  
  control_repeat_single = Gtk::RadioAction::create(control_repeat_group, "ControlRepeatSingle", _("Repeat Single"));
  d_actiongroup->add(control_repeat_single);
  control_repeat_single_connection = control_repeat_single->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onControlRepeatModeToggled));
  
  control_repeat_section = Gtk::RadioAction::create(control_repeat_group, "ControlRepeatSection", _("Repeat Section"));
  control_repeat_section->set_sensitive(false);
  d_actiongroup->add(control_repeat_section);
  control_repeat_section_connection = control_repeat_section->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onControlRepeatModeToggled));
  
  control_shuffle = Gtk::ToggleAction::create("ControlShuffle", _("Shuffle"));
  d_actiongroup->add(control_shuffle, Gtk::AccelKey("<ctrl>U"));
  control_shuffle_connection = control_shuffle->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::MenuBar::onControlShuffleToggled));
  
  control_subtitle = Gtk::Action::create("ControlSubtitleSubMenu", _("Subtitle"));
  d_actiongroup->add(control_subtitle);
  
  d_actiongroup->add(Gtk::Action::create("ControlSubtitleLoadFile", _("Load File...")), Gtk::AccelKey("<ctrl><shift>O"), sigc::mem_fun(*this, &Dingo::MenuBar::onControlSubtitleLoadFileActivated));
  
  control_subtitle_unset_saved = Gtk::Action::create("ControlSubtitleUnsetSaved", _("Unset Saved"));
  d_actiongroup->add(control_subtitle_unset_saved, sigc::mem_fun(*this, &Dingo::MenuBar::onControlSubtitleUnsetSavedActivated));
  
  control_subtitle_show = Gtk::ToggleAction::create("ControlSubtitleShow", _("Show Subtitle"));
  d_actiongroup->add(control_subtitle_show, Gtk::AccelKey("<ctrl><alt>S"), sigc::mem_fun(*this, &Dingo::MenuBar::onControlSubtitleShowToggled));
  d_actiongroup->add(Gtk::Action::create("ControlSubtitleChooseFont", _("Choose Font")), Gtk::AccelKey("<ctrl>F"), sigc::mem_fun(*this, &Dingo::MenuBar::onControlSubtitleChooseFontActivated));
}

void Dingo::MenuBar::createHelpMenu() {
  d_actiongroup->add(Gtk::Action::create("HelpMenu", _("Help")));
  d_actiongroup->add(Gtk::Action::create("HelpHomePage", _("Dingo Home Page")), Gtk::AccelKey("F1"), sigc::mem_fun(*this, &Dingo::MenuBar::onHelpHomePageActivated));
  d_actiongroup->add(Gtk::Action::create("HelpGetInvolved", _("Get Involved")), sigc::mem_fun(*this, &Dingo::MenuBar::onHelpGetInvolvedActivated));
  d_actiongroup->add(Gtk::Action::create("HelpAbout", _("About")), sigc::mem_fun(*this, &Dingo::MenuBar::onHelpAboutActivated));
}
      
//SIGNAL HANDLERS - MEDIA MENU
void Dingo::MenuBar::onMediaImportFileActivated() {
  //create Gtk::FileFilter
  Glib::RefPtr<Gtk::FileFilter> p_filter = Gtk::FileFilter::create();
  
  p_filter->add_mime_type("audio/*");
  p_filter->add_mime_type("video/*");
  
  p_filter->set_name(_("Media Files"));
  
  Gtk::FileChooserDialog dialog(_("Import File(s)"), Gtk::FILE_CHOOSER_ACTION_OPEN);
  
  dialog.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
  dialog.add_button(_("Import"), Gtk::RESPONSE_OK);
  dialog.add_filter(p_filter);
  dialog.set_select_multiple(true);
  dialog.set_default_response(Gtk::RESPONSE_OK);
  dialog.set_current_folder_uri(d_import_files_folder_uri);
  
  int result = dialog.run();
      
  std::vector<Glib::ustring> temp_uris = dialog.get_uris();
  d_import_files_folder_uri = dialog.get_current_folder_uri();
  
  dialog.hide();
  
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  switch (result) {
    case (Gtk::RESPONSE_OK): {
      //std::cout << "Open Clicked!" << std::endl;
      
      d_dbmanager->addTracksToDatabase(temp_uris);
      
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
      
      break;
    }
    
    case (Gtk::RESPONSE_CANCEL): {
      //std::cout << "Cancel Clicked!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unexpected button clicked!" << std::endl;
      break;
    }
  }
}

void Dingo::MenuBar::onMediaImportFolderActivated() {
  //create Gtk::FileFilter
  Glib::RefPtr<Gtk::FileFilter> p_filter = Gtk::FileFilter::create();
  
  p_filter->add_mime_type("audio/*");
  p_filter->add_mime_type("video/*");
  
  p_filter->set_name(_("Folders"));
  
  Gtk::FileChooserDialog dialog(_("Import Folder(s)"), Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
  
  dialog.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
  dialog.add_button(_("Import"), Gtk::RESPONSE_OK);
  dialog.add_filter(p_filter);
  dialog.set_select_multiple(true);
  dialog.set_default_response(Gtk::RESPONSE_OK);
  dialog.set_current_folder_uri(d_import_folders_folder_uri);
  
  int result = dialog.run();
      
  std::vector<Glib::ustring> temp_uris = dialog.get_uris();
  d_import_folders_folder_uri = dialog.get_current_folder_uri();
  
  dialog.hide();
  
  if (temp_uris.empty()) {
    temp_uris.push_back(d_import_folders_folder_uri);
  }
  
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  switch (result) {
    case (Gtk::RESPONSE_OK): {
      //std::cout << "Open Clicked!" << std::endl;
      
      d_dbmanager->addTracksFolderToDatabase(temp_uris);
      
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
      
      break;
    }
    
    case (Gtk::RESPONSE_CANCEL): {
      //std::cout << "Cancel Clicked!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unexpected button clicked!" << std::endl;
      break;
    }
  }
}

void Dingo::MenuBar::onMediaPlaylistSubMenuActivated() {
  if (d_dbmanager->getCurrentPlaylistID() == 0) {
    media_rename_selected_playlist->set_sensitive(false);
    media_delete_selected_playlist->set_sensitive(false);
  }
  
  else {
    media_rename_selected_playlist->set_sensitive(true);
    media_delete_selected_playlist->set_sensitive(true);
  }
}

void Dingo::MenuBar::onMediaNewEmptyPlaylistActivated() {
  d_playlisttreeview->onNewPlaylistActivated();
}

void Dingo::MenuBar::onMediaImportFromFilesActivated() {
  d_playlisttreeview->onImportPlaylistActivated();
}

void Dingo::MenuBar::onMediaRenameSelectedPlaylistActivated() {
  d_playlisttreeview->onRenamePlaylistActivated();
}

void Dingo::MenuBar::onMediaExportToFileActivated() {
  d_playlisttreeview->onExportPlaylistActivated();
}

void Dingo::MenuBar::onMediaDeleteSelectedPlaylistActivated() {
  d_playlisttreeview->onDeletePlaylistActivated();
}

void Dingo::MenuBar::onMediaCloseActivated() {
  d_statusicon->setShowMainWindowToggle(false);
}

void Dingo::MenuBar::onMediaQuitActivated() {
  d_dingo_app->release();
  
  d_dbmanager->setMainWindowVisible(false);
}
      
//SIGNAL HANDLERS - EDIT MENU
void Dingo::MenuBar::onEditMenuActivated() {
  if (!d_tracktreeview->getIsAnyTrackSelected()) {
    edit_cut->set_sensitive(false);
    edit_copy->set_sensitive(false);
    edit_add_to->set_sensitive(false);
    edit_delete_from->set_sensitive(false);
    edit_single_track->set_sensitive(false);
    edit_all_selected_tracks->set_sensitive(false);
    
    if (d_dbmanager->getCurrentPlaylistID() == 0) {
      edit_paste->set_sensitive(false);
    }
    
    else {
      edit_paste->set_sensitive(true);
    }
  }
    
  else {
    if (d_dbmanager->getCurrentPlaylistID() == 0) {
      edit_cut->set_sensitive(false);
      edit_paste->set_sensitive(false);
    }
    
    else {
      edit_cut->set_sensitive(true);
      edit_paste->set_sensitive(true);
    }
    
    edit_copy->set_sensitive(true);
    edit_add_to->set_sensitive(true);
    edit_delete_from->set_sensitive(true);
    edit_single_track->set_sensitive(true);
    edit_all_selected_tracks->set_sensitive(true);
  }
    
  if (!d_dbmanager->getIsAnyTrackInClipboard()) {
    edit_paste->set_sensitive(false);
  }
    
  else {
    edit_paste->set_sensitive(true);
  }
}

void Dingo::MenuBar::onEditCutTracksActivated() {
  d_tracktreeview->onCutTracksActivated();
}

void Dingo::MenuBar::onEditCopyTracksActivated() {
  d_tracktreeview->onCopyTracksActivated();
}

void Dingo::MenuBar::onEditPasteTracksActivated() {
  d_tracktreeview->onPasteTracksActivated();
}

void Dingo::MenuBar::onEditAddToSubMenuActivated() {
  edit_add_to_play_queue->set_sensitive(true);

  d_dbmanager->prepareQueryTrackInPlaylist();
  
  std::vector<Gtk::TreeRowReference> row_refs_vector = d_tracktreeview->getSelectionRefs();
  
  for (std::vector<Gtk::TreeRowReference>::iterator iter = row_refs_vector.begin(); iter != row_refs_vector.end(); ++iter) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::trackModel->get_iter(iter->get_path()));
    
    if (d_dbmanager->getTrackInPlaylist(row[(*Dingo::DBManager::trackCR).trackID], 1)) {
      edit_add_to_play_queue->set_sensitive(false);
      
      break;
    }
  }
  
  d_dbmanager->finalizeQueryTrackInPlaylist();
}

void Dingo::MenuBar::onEditAddToPlayQueueActivated() {
  d_tracktreeview->onAddToPlayQueueActivated();
}

void Dingo::MenuBar::onEditAddToPlaylistsActivated() {
  d_tracktreeview->onAddToNormalPlaylistActivated();
}

void Dingo::MenuBar::onEditDeleteFromSubMenuActivated() {
  edit_delete_from_play_queue->set_sensitive(false);

  d_dbmanager->prepareQueryTrackInPlaylist();
  
  std::vector<Gtk::TreeRowReference> row_refs_vector = d_tracktreeview->getSelectionRefs();
  
  for (std::vector<Gtk::TreeRowReference>::iterator iter = row_refs_vector.begin(); iter != row_refs_vector.end(); ++iter) {
    Gtk::TreeModel::Row row = *(Dingo::DBManager::trackModel->get_iter(iter->get_path()));
    
    if (d_dbmanager->getTrackInPlaylist(row[(*Dingo::DBManager::trackCR).trackID], 1)) {
      edit_delete_from_play_queue->set_sensitive(true);
      
      break;
    }
  }
  
  d_dbmanager->finalizeQueryTrackInPlaylist();
  
  if (d_dbmanager->getCurrentPlaylistID() == 0) {
    edit_delete_from_current_playlist->set_sensitive(false);
  }
  
  else {
    edit_delete_from_current_playlist->set_sensitive(true);
  }
}

void Dingo::MenuBar::onEditDeleteFromDatabaseActivated() {
  d_tracktreeview->onDeleteFromDatabaseActivated();
}

void Dingo::MenuBar::onEditDeleteFromPlayQueueActivated() {
  d_tracktreeview->onDeleteFromPlayQueueActivated();
}

void Dingo::MenuBar::onEditDeleteFromHardDriveActivated() {
  d_tracktreeview->onDeleteFromHardDriveActivated();
}

void Dingo::MenuBar::onEditDeleteFromPlaylistsActivated() {
  d_tracktreeview->onDeleteFromNormalPlaylistActivated();
}

void Dingo::MenuBar::onEditDeleteFromCurrentPlaylistActivated() {
  d_tracktreeview->onDeleteFromCurrentPlaylistActivated();
}

void Dingo::MenuBar::onEditSingleTrackActivated() {
  d_tracktreeview->onEditInfoActivated();
}

void Dingo::MenuBar::onEditAllSelectedTracksActivated() {
  d_tracktreeview->onEditAllInfoActivated();
}

void Dingo::MenuBar::onEditSettingsLoadFromFileActivated() {
  //create Gtk::FileFilter
  Glib::RefPtr<Gtk::FileFilter> p_filter = Gtk::FileFilter::create();
  
  p_filter->add_pattern("*.dgs");
  
  p_filter->set_name(_("Dingo Settings File (*.dgs)"));

  Gtk::FileChooserDialog dialog(_("Select Settings File"), Gtk::FILE_CHOOSER_ACTION_OPEN);
  
  dialog.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
  dialog.add_button(_("Open"), Gtk::RESPONSE_OK);
  
  dialog.set_default_response(Gtk::RESPONSE_OK);
  dialog.add_filter(p_filter);
  dialog.set_current_folder_uri(d_load_settings_folder_uri);
  
  int result = dialog.run();
      
  Glib::ustring temp_uri = dialog.get_uri();
  d_load_settings_folder_uri = dialog.get_current_folder_uri();
  
  dialog.hide();
  
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  switch (result) {
    case (Gtk::RESPONSE_OK): {
      //std::cout << "Open Clicked!" << std::endl;
      
      d_settings->readFromFile(temp_uri);
      d_settings->notifySettingsActionObserver(Dingo::SETTINGS_READ);
      
      break;
    }
    
    case (Gtk::RESPONSE_CANCEL): {
      //std::cout << "Cancel Clicked!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unexpected button clicked!" << std::endl;
      break;
    }
  }
}

void Dingo::MenuBar::onEditSettingsSaveToFileActivated() {
  Gtk::FileChooserDialog dialog(_("Save Settings File"), Gtk::FILE_CHOOSER_ACTION_SAVE);
  
  dialog.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
  dialog.add_button(_("Save"), Gtk::RESPONSE_OK);
  
  dialog.set_default_response(Gtk::RESPONSE_OK);
  dialog.set_create_folders(true);
  dialog.set_do_overwrite_confirmation(true);
  dialog.set_current_folder_uri(d_save_settings_folder_uri);
  dialog.set_current_name(_("dingo-settings.dgs"));
  
  int result = dialog.run();
      
  Glib::ustring temp_uri = dialog.get_uri();
  d_save_settings_folder_uri = dialog.get_current_folder_uri();
  
  dialog.hide();
  
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  switch (result) {
    case (Gtk::RESPONSE_OK): {
      //std::cout << "Open Clicked!" << std::endl;
      
      d_settings->notifySettingsActionObserver(Dingo::SETTINGS_WRITE);
      d_settings->saveToFile(temp_uri);
      
      break;
    }
    
    case (Gtk::RESPONSE_CANCEL): {
      //std::cout << "Cancel Clicked!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unexpected button clicked!" << std::endl;
      break;
    }
  }
}

void Dingo::MenuBar::onEditSettingsSaveCurrentAsDefaultActivated() {
  d_settings->notifySettingsActionObserver(Dingo::SETTINGS_WRITE);
  d_settings->saveToDefault();
}

void Dingo::MenuBar::onEditSettingsRestoreToDefaultActivated() {
  d_settings->readFromDefault();
  d_settings->notifySettingsActionObserver(Dingo::SETTINGS_READ);
}
      
//SIGNAL HANDLERS - VIEW MENU
void Dingo::MenuBar::onViewPlaylistBrowserToggled() {
  d_playlistscrolled->set_visible(view_playlist_browser->get_active());
  
  d_settings->setVisible("playlistbrowser", view_playlist_browser->get_active());
  
  if ((!view_playlist_browser->get_active()) && (!view_track_browser->get_active())) {
    d_playlist_hpaned->hide();
  }
  
  else {
    if (!d_playlist_hpaned->get_visible()) {
      d_playlist_hpaned->show();
    }
  }
}

void Dingo::MenuBar::onViewTrackBrowserToggled() {
  d_trackscrolled->set_visible(view_track_browser->get_active());
  
  d_settings->setVisible("trackbrowser", view_track_browser->get_active());
  
  if (view_track_browser->get_active()) {
    d_tracktreeview->refreshData();
  }
  
  if ((!view_playlist_browser->get_active()) && (!view_track_browser->get_active())) {
    d_playlist_hpaned->hide();
  }
  
  else {
    if (!d_playlist_hpaned->get_visible()) {
      d_playlist_hpaned->show();
    }
  }
}

//new
void Dingo::MenuBar::onViewGenreFilterToggled() {
  d_genrescrolled->set_visible(view_genre_filter->get_active());
  
  d_settings->setVisible("genrefilter", view_genre_filter->get_active());
  
  if (view_genre_filter->get_active()) {
    d_genretreeview->refreshData();
    
    d_genre_hpaned->show();
    
    view_artist_filter->set_active(!view_artist_filter->get_active());
    view_artist_filter->set_active(!view_artist_filter->get_active());
    
    view_year_filter->set_active(!view_year_filter->get_active());
    view_year_filter->set_active(!view_year_filter->get_active());
    
    view_album_filter->set_active(!view_album_filter->get_active());
    view_album_filter->set_active(!view_album_filter->get_active());
  }
  
  else {
    if ((!view_genre_filter->get_active()) && (!view_artist_filter->get_active()) && (!view_year_filter->get_active()) && (!view_album_filter->get_active())) {
      d_genre_hpaned->hide();
    }
  }
}

void Dingo::MenuBar::onViewArtistFilterToggled() {
  d_artistscrolled->set_visible(view_artist_filter->get_active());
  
  d_settings->setVisible("artistfilter", view_artist_filter->get_active());
  
  if (view_artist_filter->get_active()) {
    d_artisttreeview->refreshData();
  }
  
  if (view_artist_filter->get_active()) {
    d_artisttreeview->refreshData();
    
    d_genre_hpaned->show();
    d_artist_hpaned->show();
  }
  
  else {
    if ((!view_genre_filter->get_active()) && (!view_artist_filter->get_active()) && (!view_year_filter->get_active()) && (!view_album_filter->get_active())) {
      d_genre_hpaned->hide();
    }
    
    if ((!view_artist_filter->get_active()) && (!view_year_filter->get_active()) && (!view_album_filter->get_active())) {
      d_artist_hpaned->hide();
    }
  }
}

void Dingo::MenuBar::onViewYearFilterToggled() {
  d_yearscrolled->set_visible(view_year_filter->get_active());
  
  d_settings->setVisible("yearfilter", view_year_filter->get_active());
  
  if (view_year_filter->get_active()) {
    d_yeartreeview->refreshData();
  }
  
  if (view_year_filter->get_active()) {
    d_yeartreeview->refreshData();
    
    d_genre_hpaned->show();
    d_artist_hpaned->show();
    d_year_hpaned->show();
  }
  
  else {
    if ((!view_genre_filter->get_active()) && (!view_artist_filter->get_active()) && (!view_year_filter->get_active()) && (!view_album_filter->get_active())) {
      d_genre_hpaned->hide();
    }
    
    if ((!view_artist_filter->get_active()) && (!view_year_filter->get_active()) && (!view_album_filter->get_active())) {
      d_artist_hpaned->hide();
    }
    
    if ((!view_year_filter->get_active()) && (!view_album_filter->get_active())) {
      d_year_hpaned->hide();
    }
  }
}

void Dingo::MenuBar::onViewAlbumFilterToggled() {
  d_albumscrolled->set_visible(view_album_filter->get_active());
  
  d_settings->setVisible("albumfilter", view_album_filter->get_active());
  
  if (view_album_filter->get_active()) {
    d_albumtreeview->refreshData();
  }
  
  if (view_album_filter->get_active()) {
    d_albumtreeview->refreshData();
    
    d_genre_hpaned->show();
    d_artist_hpaned->show();
    d_year_hpaned->show();
  }
  
  else {
    if ((!view_genre_filter->get_active()) && (!view_artist_filter->get_active()) && (!view_year_filter->get_active()) && (!view_album_filter->get_active())) {
      d_genre_hpaned->hide();
    }
    
    if ((!view_artist_filter->get_active()) && (!view_year_filter->get_active()) && (!view_album_filter->get_active())) {
      d_artist_hpaned->hide();
    }
    
    if ((!view_year_filter->get_active()) && (!view_album_filter->get_active())) {
      d_year_hpaned->hide();
    }
  }
}

void Dingo::MenuBar::onViewPlaybackStatusToggled() {
  d_tracktreeview->setColumnVisible(0, view_playback_status->get_active());
  
  d_settings->setVisible("playbackstatus", view_playback_status->get_active());
}

void Dingo::MenuBar::onViewTrackNumberToggled() {
  d_tracktreeview->setColumnVisible(1, view_track_number->get_active());
  
  d_settings->setVisible("tracknumber", view_track_number->get_active());
}

void Dingo::MenuBar::onViewTrackTitleToggled() {
  d_tracktreeview->setColumnVisible(2, view_track_title->get_active());
  
  d_settings->setVisible("tracktitle", view_track_title->get_active());
}

void Dingo::MenuBar::onViewTrackGenreToggled() {
  d_tracktreeview->setColumnVisible(3, view_track_genre->get_active());
  
  d_settings->setVisible("trackgenre", view_track_genre->get_active());
}

void Dingo::MenuBar::onViewTrackArtistToggled() {
  d_tracktreeview->setColumnVisible(4, view_track_artist->get_active());
  
  d_settings->setVisible("trackartist", view_track_artist->get_active());
}

void Dingo::MenuBar::onViewTrackAlbumToggled() {
  d_tracktreeview->setColumnVisible(5, view_track_album->get_active());
  
  d_settings->setVisible("trackalbum", view_track_album->get_active());
}

void Dingo::MenuBar::onViewTrackYearToggled() {
  d_tracktreeview->setColumnVisible(6, view_track_year->get_active());
  
  d_settings->setVisible("trackyear", view_track_year->get_active());
}

void Dingo::MenuBar::onViewTrackTimeToggled() {
  d_tracktreeview->setColumnVisible(7, view_track_time->get_active());
  
  d_settings->setVisible("tracktime", view_track_time->get_active());
}

void Dingo::MenuBar::onViewTrackExtensionToggled() {
  d_tracktreeview->setColumnVisible(8, view_track_extension->get_active());
  
  d_settings->setVisible("trackfileextension", view_track_extension->get_active());
}

void Dingo::MenuBar::onViewTrackBitrateToggled() {
  d_tracktreeview->setColumnVisible(9, view_track_bitrate->get_active());
  
  d_settings->setVisible("trackbitrate", view_track_bitrate->get_active());
}

void Dingo::MenuBar::onViewAudioOnlyToggled() {
  is_audio_only_toggled_here = true;

  if (view_audio_only->get_active()) {
    d_toolbar->activateToggleButton(Dingo::AUDIO_FILTER_TOGGLED);
  }
  
  else {
    d_toolbar->deactivateToggleButton(Dingo::AUDIO_FILTER_TOGGLED);
  }
}

void Dingo::MenuBar::onViewVideoOnlyToggled() {
  is_video_only_toggled_here = true;

  if (view_video_only->get_active()) {
    d_toolbar->activateToggleButton(Dingo::VIDEO_FILTER_TOGGLED);
  }
  
  else {
    d_toolbar->deactivateToggleButton(Dingo::VIDEO_FILTER_TOGGLED);
  }
}

void Dingo::MenuBar::onViewFullscreenToggled() {
  is_fullscreen_toggled_here = true;

  d_controlbar->onFullscreenButtonClicked();
}

void Dingo::MenuBar::onViewLyricsActivated() {
  d_tracktreeview->onLyricsActivated();
}
      
//SIGNAL HANDLERS - CONTROL MENU
void Dingo::MenuBar::onControlMenuActivated() {
  Gtk::TreeRowReference cur_track_row_ref = d_dbmanager->getCurrentTrackRowRef();
  
  if (cur_track_row_ref.is_valid()) {
    Gtk::TreeModel::Row cur_track_row = *(Dingo::DBManager::trackModel->get_iter(cur_track_row_ref.get_path()));
  
    if (cur_track_row[(*Dingo::DBManager::trackCR).trackMIMEType] == "video/*") {
      control_subtitle->set_sensitive(true);
    
      if (cur_track_row[(*Dingo::DBManager::trackCR).trackSubtitleURI] != "") {
        control_subtitle_unset_saved->set_sensitive(true);
      }
    
      else {
        control_subtitle_unset_saved->set_sensitive(false);
      }
      
      if (d_playerengine->getStatus() != Dingo::ENGINE_STOPPED) {
        control_subtitle->set_sensitive(true);
      }
      
      else {
        control_subtitle->set_sensitive(false);
      }
    }
  
    else {
      control_subtitle->set_sensitive(false);
    }
  }
  
  else {
    control_subtitle->set_sensitive(false);
  }
}

void Dingo::MenuBar::onControlPlayToggled() {
  is_play_toggled_here = true;

  d_controlbar->onPlayButtonClicked();
}

void Dingo::MenuBar::onControlPreviousActivated() {
  d_controlbar->onPreviousButtonClicked();
}

void Dingo::MenuBar::onControlNextActivated() {
  d_controlbar->onNextButtonClicked();
}

void Dingo::MenuBar::onControlRepeatModeToggled() {
  is_repeat_toggled_here = true;

  if (control_repeat_off->get_active()) {
    d_controlbar->setRepeatState(Dingo::REPEAT_OFF);
    d_playerengine->setRepeatMode(Dingo::REPEAT_OFF);
  }
  
  else if (control_repeat_single->get_active()) {
    d_controlbar->setRepeatState(Dingo::REPEAT_SINGLE);
    d_playerengine->setRepeatMode(Dingo::REPEAT_SINGLE);
  }
  
  else if (control_repeat_playlist->get_active()) {
    d_controlbar->setRepeatState(Dingo::REPEAT_PLAYLIST);
    d_playerengine->setRepeatMode(Dingo::REPEAT_PLAYLIST);
  }
}

void Dingo::MenuBar::onControlShuffleToggled() {
  is_playorder_toggled_here = true;

  if (control_shuffle->get_active()) {
    d_controlbar->setShuffleToggle(true);
  }
  
  else {
    d_controlbar->setShuffleToggle(false);
  }
}

void Dingo::MenuBar::onControlSubtitleLoadFileActivated() {
  //create Gtk::FileFilter
  Glib::RefPtr<Gtk::FileFilter> p_filter = Gtk::FileFilter::create();
  
  p_filter->add_mime_type("text/*");
  
  p_filter->set_name(_("Subtitle Files"));

  Gtk::FileChooserDialog dialog(_("Open Subtitle"), Gtk::FILE_CHOOSER_ACTION_OPEN);
  
  dialog.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
  dialog.add_button(_("Open"), Gtk::RESPONSE_OK);
  dialog.add_filter(p_filter);
  dialog.set_select_multiple(false);
  dialog.set_current_folder_uri(d_open_subtitle_folder_uri);
  dialog.set_default_response(Gtk::RESPONSE_OK);
  
  int result = dialog.run();
      
  Glib::ustring temp_uri = dialog.get_uri();
  d_open_subtitle_folder_uri = dialog.get_current_folder_uri();
  
  dialog.hide();
  
  while (Gtk::Main::events_pending()) {
    Gtk::Main::iteration();
  }
  
  switch (result) {
    case (Gtk::RESPONSE_OK): {
      Gtk::TreeRowReference cur_track_row_ref = d_dbmanager->getCurrentTrackRowRef();

      //first set the TrackSubtitleURI on the Database
      std::map<Glib::ustring, Glib::ustring> changed_values_map;
  
      changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackSubtitlePath", Glib::filename_from_uri(temp_uri)));
  
      d_dbmanager->setTrackInfo(cur_track_row_ref, changed_values_map);
      
      //now finally set subtitle for the PlayerEngine
      d_playerengine->setSubtitle(temp_uri);
      
      break;
    }
    
    case (Gtk::RESPONSE_CANCEL): {
      //std::cout << "Cancel Clicked!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unexpected button clicked!" << std::endl;
      break;
    }
  }
}

void Dingo::MenuBar::onControlSubtitleUnsetSavedActivated() {
  Gtk::TreeRowReference cur_track_row_ref = d_dbmanager->getCurrentTrackRowRef();

  //first set the TrackSubtitleURI on the Database
  std::map<Glib::ustring, Glib::ustring> changed_values_map;
  
  changed_values_map.insert(std::pair<Glib::ustring, Glib::ustring>("TrackSubtitlePath", ""));
  
  d_dbmanager->setTrackInfo(cur_track_row_ref, changed_values_map);
  
  //then set the SubtitleURI of PlayerEngine
  d_playerengine->setSubtitle("");
}

void Dingo::MenuBar::onControlSubtitleShowToggled() {
  if (control_subtitle_show->get_active()) {
    d_playerengine->setShowSubtitle(true);
  }
  
  else {
    d_playerengine->setShowSubtitle(false);
  }
}

void Dingo::MenuBar::onControlSubtitleChooseFontActivated() {
  Gtk::FontChooserDialog dialog(_("Choose Subtitle Font"));
  Pango::FontDescription temp_pango(d_playerengine->getSubtitleFontDesc());
  dialog.set_font_desc(temp_pango);
  
  int result = dialog.run();
  
  switch (result) {
    case Gtk::RESPONSE_OK: {
      //std::cout << "Font Chosen!" << std::endl;
      d_playerengine->setSubtitleFontDesc(dialog.get_font_desc().to_string());
      break;
    }
    
    case Gtk::RESPONSE_CANCEL: {
      //std::cout << "Font not Chosen!" << std::endl;
      break;
    }
    
    default: {
      //std::cout << "Unexpected button clicked!" << std::endl;
      break;
    }
  }
}
      
//SIGNAL HANDLERS - HELP MENU
void Dingo::MenuBar::onHelpHomePageActivated() {
  Gtk::LinkButton link_button;
  
  link_button.set_uri("http://dingo.phongvcao.com/");
  
  link_button.clicked();
}

void Dingo::MenuBar::onHelpGetInvolvedActivated() {
  Gtk::LinkButton link_button;
  
  link_button.set_uri("http://dingo.phongvcao.com/");
  
  link_button.clicked();
}

void Dingo::MenuBar::onHelpAboutActivated() {
  Dingo::AboutDialog about_dialog;
  
  about_dialog.run();
}
