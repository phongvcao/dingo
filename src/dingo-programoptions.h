/* 
 * dingo-programoptions.h
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

#ifndef INCLUDED_DINGO_PROGRAMOPTIONS_H
#define INCLUDED_DINGO_PROGRAMOPTIONS_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class DBManager;
  class PlayerEngine;
  class Settings;
  class ToolBar;
  class TrackTreeView;
  class PlaylistTreeView;

  class ProgramOptions {
    public:
      ProgramOptions(int& argc, char**& argv);
      virtual ~ProgramOptions();
      
      //PUBLIC METHODS
      void run(DBManager* dbmanager, Settings* settings);
      
      bool getShouldDingoQuit();
      
      //PUBLIC ACCESSORS
      void setToolBar(ToolBar* toolbar);
      void setTrackTreeView(TrackTreeView* tracktreeview);
      void setPlaylistTreeView(PlaylistTreeView* playlisttreeview);
    
    private:
      //SIGNAL HANDLERS
      bool onPlayImportedFilesTimeOut(DBManager* dbmanager);
      
      //UTILITIES
      void categorizeFile(Glib::ustring file_uri);
      void categorizeFilesInFolderRecursively(Glib::ustring folder_uri);
      
      //DATA MEMBERS      
      ToolBar* d_toolbar;
      TrackTreeView* d_tracktreeview;
      PlaylistTreeView* d_playlisttreeview;
      
      std::vector<Glib::ustring> d_settings_files_uris;
      std::vector<Glib::ustring> d_playlists_files_uris;
      std::vector<Glib::ustring> d_tracks_files_uris;
      
      std::vector<Glib::ustring> d_arguments_vector;
      
      bool d_should_dingo_quit;
  };
}

#endif //INCLUDED_DINGO_PROGRAMOPTIONS_H
