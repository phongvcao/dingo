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
