/* 
 * dingo-menubar.h
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

#ifndef INCLUDED_DINGO_MENUBAR_H
#define INCLUDED_DINGO_MENUBAR_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#include "dingo-audiovideofilterobserver.h"
#include "dingo-toolbarobserver.h"
#include "dingo-fullscreenobserver.h"
#include "dingo-enginestatusobserver.h"
#include "dingo-playorderobserver.h"
#include "dingo-repeattoggleobserver.h"
#include "dingo-settingsactionobserver.h"

namespace Dingo {
  class DBManager;
  class PlayerEngine;
  class Settings;
  class PlaylistTreeView;
  class TrackTreeView;
  class GenreTreeView;
  class ArtistTreeView;
  class YearTreeView;
  class AlbumTreeView;
  class ToolBar;
  class ControlBar;
  class StatusBar;
  class StatusIcon;

  class MenuBar : public Gtk::VBox, public AudioVideoFilterObserver, public FullscreenObserver, public EngineStatusObserver, public PlayOrderObserver, public RepeatToggleObserver, public ToolBarObserver, public SettingsActionObserver {
    public:
      MenuBar(DBManager* dbmanager, PlayerEngine* playerengine, Settings* settings, Gtk::Application* dingo_app);
      virtual ~MenuBar();
      
      //PUBLIC ACCESSORS      
      void setPlaylistTreeView(PlaylistTreeView* playlisttreeview, Gtk::ScrolledWindow* scrolled_win, Gtk::HPaned* playlist_hpaned);
      void setTrackTreeView(TrackTreeView* tracktreeview, Gtk::ScrolledWindow* scrolled_win);
      void setGenreTreeView(GenreTreeView* genretreeview, Gtk::ScrolledWindow* scrolled_win, Gtk::HPaned* genre_hpaned);
      void setArtistTreeView(ArtistTreeView* artisttreeview, Gtk::ScrolledWindow* scrolled_win, Gtk::HPaned* artist_hpaned);
      void setYearTreeView(YearTreeView* yeartreeview, Gtk::ScrolledWindow* scrolled_win, Gtk::HPaned* year_hpaned);
      void setAlbumTreeView(AlbumTreeView* albumtreeview, Gtk::ScrolledWindow* scrolled_win);
      
      void setToolBar(ToolBar* toolbar);
      void setControlBar(ControlBar* controlbar);
      void setStatusBar(StatusBar* statusbar);
      void setStatusIcon(StatusIcon* statusicon);
      
      //INHERITED OBSERVER'S METHODS
      void updateHomeToggleActive();
      void updateNowPlayingToggleActive();
      void updatePlayQueueToggleActive();
      
      void updateAudioFilterToggled();
      void updateVideoFilterToggled();
      void updateFullscreenActivated();
      void updateUnFullscreenActivated();
      
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
      
      void updateSettingsRead();
      void updateSettingsWrite();
    
    private:
      //CREATORS
      void createMenuBar();
      void createMediaMenu();
      void createEditMenu();
      void createViewMenu();
      void createControlMenu();
      void createHelpMenu();
      
      //SIGNAL HANDLERS - MEDIA MENU
      void onMediaImportFileActivated();
      void onMediaImportFolderActivated();
      void onMediaPlaylistSubMenuActivated();
      void onMediaNewEmptyPlaylistActivated();
      void onMediaImportFromFilesActivated();
      void onMediaRenameSelectedPlaylistActivated();
      void onMediaExportToFileActivated();
      void onMediaDeleteSelectedPlaylistActivated();
      void onMediaCloseActivated();
      void onMediaQuitActivated();
      
      //SIGNAL HANDLERS - EDIT MENU
      void onEditMenuActivated();
      void onEditCutTracksActivated();
      void onEditCopyTracksActivated();
      void onEditPasteTracksActivated();
      void onEditAddToSubMenuActivated();
      void onEditAddToPlayQueueActivated();
      void onEditAddToPlaylistsActivated();
      void onEditDeleteFromSubMenuActivated();
      void onEditDeleteFromDatabaseActivated();
      void onEditDeleteFromPlayQueueActivated();
      void onEditDeleteFromHardDriveActivated();
      void onEditDeleteFromPlaylistsActivated();
      void onEditDeleteFromCurrentPlaylistActivated();
      void onEditSingleTrackActivated();
      void onEditAllSelectedTracksActivated();
      
      void onEditSettingsLoadFromFileActivated();
      void onEditSettingsSaveToFileActivated();
      void onEditSettingsSaveCurrentAsDefaultActivated();
      void onEditSettingsRestoreToDefaultActivated();
      
      //SIGNAL HANDLERS - VIEW MENU
      void onViewPlaylistBrowserToggled();
      void onViewTrackBrowserToggled();
      
      //new
      void onViewGenreFilterToggled();
      void onViewArtistFilterToggled();
      void onViewYearFilterToggled();
      void onViewAlbumFilterToggled();
      void onViewPlaybackStatusToggled();
      void onViewTrackNumberToggled();
      void onViewTrackTitleToggled();
      void onViewTrackGenreToggled();
      void onViewTrackArtistToggled();
      void onViewTrackAlbumToggled();
      void onViewTrackYearToggled();
      void onViewTrackTimeToggled();
      void onViewTrackExtensionToggled();
      void onViewTrackBitrateToggled();
      
      void onViewAudioOnlyToggled();
      void onViewVideoOnlyToggled();
      void onViewFullscreenToggled();
      void onViewLyricsActivated();
      
      //SIGNAL HANDLERS - CONTROL MENU
      void onControlMenuActivated();
      void onControlPlayToggled();
      void onControlPreviousActivated();
      void onControlNextActivated();
      void onControlRepeatModeToggled();
      void onControlShuffleToggled();
      void onControlSubtitleLoadFileActivated();
      void onControlSubtitleUnsetSavedActivated();
      void onControlSubtitleShowToggled();
      void onControlSubtitleChooseFontActivated();
      
      //SIGNAL HANDLERS - HELP MENU
      void onHelpHomePageActivated();
      void onHelpGetInvolvedActivated();
      void onHelpAboutActivated();
      
      //OTHER DATA MEMBERS
      Glib::RefPtr<Gtk::UIManager> d_uimanager;
      Glib::RefPtr<Gtk::ActionGroup> d_actiongroup;
      Glib::RefPtr<Gtk::Action> media_rename_selected_playlist, media_delete_selected_playlist;
      Glib::RefPtr<Gtk::Action> edit_action, edit_cut, edit_copy, edit_paste;
      Glib::RefPtr<Gtk::Action> edit_single_track, edit_all_selected_tracks, edit_add_to, edit_delete_from;
      Glib::RefPtr<Gtk::Action> edit_add_to_play_queue, edit_delete_from_play_queue, edit_delete_from_current_playlist;
      Glib::RefPtr<Gtk::Action> control_action, control_subtitle;
      Glib::RefPtr<Gtk::ToggleAction> control_play;
      Glib::RefPtr<Gtk::RadioAction> control_repeat_off, control_repeat_single;
      Glib::RefPtr<Gtk::RadioAction> control_repeat_playlist, control_repeat_section;
      Glib::RefPtr<Gtk::ToggleAction> control_shuffle, control_subtitle_show;
      Glib::RefPtr<Gtk::Action> control_subtitle_unset_saved;
      Glib::RefPtr<Gtk::ToggleAction> view_playlist_browser, view_track_browser;
      Glib::RefPtr<Gtk::ToggleAction> view_genre_filter, view_artist_filter, view_year_filter, view_album_filter;
      Glib::RefPtr<Gtk::ToggleAction> view_playback_status, view_track_number, view_track_title, view_track_genre, view_track_artist, view_track_album, view_track_year, view_track_time, view_track_extension, view_track_bitrate;
      Glib::RefPtr<Gtk::ToggleAction> view_audio_only, view_video_only;
      Glib::RefPtr<Gtk::ToggleAction> view_fullscreen;
      
      sigc::connection control_play_connection;
      sigc::connection control_shuffle_connection;
      sigc::connection view_audio_only_connection, view_video_only_connection;
      sigc::connection view_fullscreen_connection;
      sigc::connection view_playlist_browser_connection, view_track_browser_connection;
      sigc::connection view_genre_filter_connection, view_artist_filter_connection, view_year_filter_connection, view_album_filter_connection;
      sigc::connection view_playback_status_connection, view_track_number_connection, view_track_title_connection, view_track_genre_connection, view_track_artist_connection, view_track_album_connection, view_track_year_connection, view_track_time_connection, view_track_extension_connection, view_track_bitrate_connection;
      sigc::connection control_repeat_off_connection, control_repeat_single_connection;
      sigc::connection control_repeat_playlist_connection, control_repeat_section_connection;
      
      bool is_audio_only_toggled_here;
      bool is_video_only_toggled_here;
      bool is_fullscreen_toggled_here;
      bool is_play_toggled_here;
      bool is_playorder_toggled_here;
      bool is_repeat_toggled_here;
      
      PlaylistTreeView* d_playlisttreeview;
      Gtk::ScrolledWindow* d_playlistscrolled;
      TrackTreeView* d_tracktreeview;
      Gtk::ScrolledWindow* d_trackscrolled;
      
      GenreTreeView* d_genretreeview;
      Gtk::ScrolledWindow* d_genrescrolled;
      ArtistTreeView* d_artisttreeview;
      Gtk::ScrolledWindow* d_artistscrolled;
      YearTreeView* d_yeartreeview;
      Gtk::ScrolledWindow* d_yearscrolled;
      AlbumTreeView* d_albumtreeview;
      Gtk::ScrolledWindow* d_albumscrolled;
      
      Gtk::HPaned* d_playlist_hpaned;
      Gtk::HPaned* d_genre_hpaned;
      Gtk::HPaned* d_artist_hpaned;
      Gtk::HPaned* d_year_hpaned;
      
      ToolBar* d_toolbar;
      ControlBar* d_controlbar;
      StatusBar* d_statusbar;
      StatusIcon* d_statusicon;
      
      DBManager* d_dbmanager;
      PlayerEngine* d_playerengine;
      Settings* d_settings;
      Gtk::Application* d_dingo_app;
      
      Glib::ustring d_import_files_folder_uri, d_import_folders_folder_uri, d_load_settings_folder_uri, d_save_settings_folder_uri, d_open_subtitle_folder_uri;
  };
}

#endif //INCLUDED_DINGO_MENUBAR_H
