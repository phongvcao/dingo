/* 
 * dingo-mpriscommon.h
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

#ifndef INCLUDED_DINGO_MPRISCOMMON_H
#define INCLUDED_DINGO_MPRISCOMMON_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#include "dingo-header.h"

namespace Dingo {
  class DBManager;
  class PlayerEngine;
  class Settings;
  class ToolBar;
  class TrackTreeView;
  class PlaylistTreeView;
  class ControlBar;
  class StatusIcon;

  class MPRISCommon {
    public:
      MPRISCommon(DBManager* dbmanager, PlayerEngine* playerengine, Settings* settings, Gtk::Application* dingo_app);
      virtual ~MPRISCommon();
      
      //PUBLIC ACCESSORS
      void setToolBar(ToolBar* toolbar);
      void setTrackTreeView(TrackTreeView* tracktreeview);
      void setPlaylistTreeView(PlaylistTreeView* playlisttreeview);
      void setControlBar(ControlBar* controlbar);
      void setStatusIcon(StatusIcon* statusicon);
      
      //PUBLIC CONTROLLER INTERFACE
      Glib::VariantContainerBase invokeMethodRoot(const Glib::ustring& method_name, const Glib::VariantContainerBase& parameters);
      Glib::VariantContainerBase invokeMethodPlayer(const Glib::ustring& method_name, const Glib::VariantContainerBase& parameters);
      Glib::VariantContainerBase invokeMethodPlaylists(const Glib::ustring& method_name, const Glib::VariantContainerBase& parameters);
      
      Glib::VariantBase readPropertyValueRoot(const Glib::ustring& property_name);
      Glib::VariantBase readPropertyValuePlayer(const Glib::ustring& property_name);
      Glib::VariantBase readPropertyValuePlaylists(const Glib::ustring& property_name);
      
      void writePropertyValueRoot(const Glib::ustring& property_name, const Glib::VariantBase& new_value_variantbase);
      void writePropertyValuePlayer(const Glib::ustring& property_name, const Glib::VariantBase& new_value_variantbase);
      void writePropertyValuePlaylists(const Glib::ustring& property_name, const Glib::VariantBase& new_value_variantbase);
      
      Glib::VariantContainerBase getParametersPlayerSignal(const Glib::ustring& signal_name);
      Glib::VariantContainerBase getParametersPlaylistsSignal(const Glib::ustring& signal_name);
    
    private:
      //SIGNAL HANDLERS
      bool onOpenURITimeOut(Glib::ustring track_uri);
    
      //MPRISv2 ROOT COMMANDS
      void raise();
      void quit();
      bool readFullscreen();
      void writeFullscreen(bool fullscreen);
      
      //MPRISv2 PLAYER COMMANDS
      void next();
      void previous();
      void pause();
      void playPause();
      void stop();
      void play();
      void seek(gint64 forward_pos);
      void setPosition(gint64 new_pos);
      void openURI(Glib::ustring track_uri);
      
      Glib::VariantBase readPlaybackStatus();
      Glib::VariantBase readLoopStatus();
      bool readShuffle();
      Glib::VariantBase readMetadata();
      double readVolume();
      gint64 readPosition();
      
      void writeLoopStatus(Glib::ustring loop_status);
      void writeShuffle(bool is_shuffle);
      void writeVolume(double volume);
      
      //MPRISv2 PLAYLISTS COMMANDS
      void activatePlaylist(Glib::ustring playlist_objpath_id_str);
      Glib::VariantContainerBase getPlaylists(int max_count, bool is_reverse);
      
      guint32 readPlaylistCount();
      Glib::VariantBase readActivePlaylist();
      
      //DATA MEMBERS
      ToolBar* d_toolbar;
      TrackTreeView* d_tracktreeview;
      PlaylistTreeView* d_playlisttreeview;
      ControlBar* d_controlbar;
      StatusIcon* d_statusicon;
      int d_active_playlist_changed_count;
      
      DBManager* d_dbmanager;
      PlayerEngine* d_playerengine;
      Settings* d_settings;
      Gtk::Application* d_dingo_app;
  };
}

#endif //INCLUDED_DINGO_MPRISCOMMON_H
