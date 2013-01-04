/* 
 * dingo-programoptions.cc
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

#include "dingo-programoptions.h"
#include "dingo-toolbar.h"
#include "dingo-tracktreeview.h"
#include "dingo-playlisttreeview.h"
#include "dingo-dbmanager.h"
#include "dingo-settings.h"
#include "dingo-utilities.h"
#include "dingo-header.h"

#ifndef INCLUDED_BOOST_PROGRAM_OPTIONS_HPP
#define INCLUDED_BOOST_PROGRAM_OPTIONS_HPP
#include <boost/program_options.hpp>
#endif //INCLUDED_BOOST_PROGRAM_OPTIONS_HPP

#ifndef INCLUDED_GLIBMM_I18N_H
#define INCLUDED_GLIBMM_I18N_H
#include <glibmm/i18n.h>
#endif //INCLUDED_GLIBMM_I18N_H

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

//CONSTRUCTORS - DESTRUCTOR
Dingo::ProgramOptions::ProgramOptions(int& argc, char**& argv) {
  d_should_dingo_quit = false;

  boost::program_options::options_description desc(_("Help Options"));
  
  desc.add_options()
    ("help,h", _("Show this help"))
    ("version,v", _("Show version information"));
    
  boost::program_options::variables_map cmd_variables_map;
  
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), cmd_variables_map);
  
  boost::program_options::notify(cmd_variables_map);
  
  if (cmd_variables_map.count("help")) {
    std::cout << _("Usage: dingo [OPTIONS...]") << std::endl;
    std::cout << _("       dingo [FILES | FOLDERS paths]") << std::endl;
    std::cout << std::endl;
    
    std::cout << desc << std::endl;
    
    for (int i = 0; i < argc; ++i) {
      if ((!strcmp(argv[i], "--help")) || (!strcmp(argv[i], "-h"))) {
        argv[i] = "";
        --argc;
        
        d_should_dingo_quit = true;
      }
    }
    
    return;
  }
  
  if (cmd_variables_map.count("version")) {
    std::cout << "Dingo 0.5 (0.5.1) http://www.dingo-project.org" << std::endl;
    std::cout << _("Copyright (C) 2011-2013 Phong V. Cao and Contributors") << std::endl;
    
    for (int i = 0; i < argc; ++i) {
      if ((!strcmp(argv[i], "--version")) || (!strcmp(argv[i], "-v"))) {
        argv[i] = "";
        --argc;
        
        d_should_dingo_quit = true;
      }
    }
    
    return;
  }
  
  //save the arguments vector
  d_arguments_vector = std::vector<Glib::ustring>(argv + 1, argv + argc);
}

Dingo::ProgramOptions::~ProgramOptions() {

}
      
//PUBLIC METHODS
void Dingo::ProgramOptions::run(DBManager* dbmanager, Settings* settings) {
  for (std::vector<Glib::ustring>::iterator iter = d_arguments_vector.begin(); iter != d_arguments_vector.end(); ++iter) {
    Glib::RefPtr<Gio::File> file = Gio::File::create_for_commandline_arg(*iter);
    
    if (file->query_exists()) {
      if (file->query_file_type(Gio::FILE_QUERY_INFO_NOFOLLOW_SYMLINKS) == Gio::FILE_TYPE_REGULAR) {
        categorizeFile(file->get_uri());
      }
      
      else if (file->query_file_type(Gio::FILE_QUERY_INFO_NOFOLLOW_SYMLINKS) == Gio::FILE_TYPE_DIRECTORY) {
        categorizeFilesInFolderRecursively(file->get_uri());
      }
    }
  }
  
  for (std::vector<Glib::ustring>::iterator iter = d_settings_files_uris.begin(); iter != d_settings_files_uris.end(); ++iter) {
    if (settings->readFromFile(*iter)) {
      break;
    }
  }
  
  dbmanager->importPlaylists(d_playlists_files_uris);
  
  dbmanager->addTracksToDatabase(d_tracks_files_uris, 1);
  
  if (settings->getVisible("playlistbrowser")) {
    dbmanager->notifyDBManPlaylistObserver(Dingo::CHANGES_IN_CORE_PLAYLISTS);
  }
  
  if (settings->getVisible("genrefilter")) {
    dbmanager->notifyDBManGenreObserver(Dingo::CHANGES_IN_CORE_GENRES);
  }
      
  if (settings->getVisible("artistfilter")) {
    dbmanager->notifyDBManArtistObserver(Dingo::CHANGES_IN_CORE_ARTISTS);
  }
      
  if (settings->getVisible("yearfilter")) {
    dbmanager->notifyDBManYearObserver(Dingo::CHANGES_IN_CORE_YEARS);
  }
      
  if (settings->getVisible("albumfilter")) {
    dbmanager->notifyDBManAlbumObserver(Dingo::CHANGES_IN_CORE_ALBUMS);
  }
  
  if (settings->getVisible("trackbrowser")) {
    dbmanager->notifyDBManTrackObserver(Dingo::CHANGES_IN_CORE_TRACKS);
  }
  
  if ((!d_tracks_files_uris.empty()) || (!d_playlists_files_uris.empty())) {
    Glib::signal_timeout().connect(sigc::bind(sigc::mem_fun(*this, &Dingo::ProgramOptions::onPlayImportedFilesTimeOut), dbmanager), 1000);
  }
}

bool Dingo::ProgramOptions::getShouldDingoQuit() {
  return d_should_dingo_quit;
}

//PUBLIC ACCESSORS
void Dingo::ProgramOptions::setToolBar(Dingo::ToolBar* toolbar) {
  d_toolbar = toolbar;
}

void Dingo::ProgramOptions::setTrackTreeView(Dingo::TrackTreeView* tracktreeview) {
  d_tracktreeview = tracktreeview;
}

void Dingo::ProgramOptions::setPlaylistTreeView(Dingo::PlaylistTreeView* playlisttreeview) {
  d_playlisttreeview = playlisttreeview;
}

//SIGNAL HANDLERS
bool Dingo::ProgramOptions::onPlayImportedFilesTimeOut(Dingo::DBManager* dbmanager) {
  if (!d_tracks_files_uris.empty()) {
    d_toolbar->activateToggleButton(Dingo::PLAY_QUEUE_TOGGLED);
  
    for (std::vector<Glib::ustring>::iterator iter = d_tracks_files_uris.begin(); iter != d_tracks_files_uris.end(); ++iter) {
      if (d_tracktreeview->activateRow(*iter)) {
        break;
      }
    }
    
    return false;
  }
  
  if (!d_playlists_files_uris.empty()) {
    d_playlisttreeview->activatePlaylist(dbmanager->getLastImportedPlaylistID());
    
    d_tracktreeview->activateCurrentPlaylist();
  
    return false;
  }
}

//UTILITIES
void Dingo::ProgramOptions::categorizeFile(Glib::ustring file_uri) {
  if (Dingo::Utilities::matchFileWithExtension(file_uri, ".dgs")) {
    d_settings_files_uris.push_back(file_uri);
  }
        
  else if (Dingo::Utilities::matchFileWithExtension(file_uri, ".dpls")) {
    d_playlists_files_uris.push_back(file_uri);
  }
        
  else {
    d_tracks_files_uris.push_back(file_uri);
  }
}

void Dingo::ProgramOptions::categorizeFilesInFolderRecursively(Glib::ustring folder_uri) {
  Glib::Dir top_dir(Glib::filename_from_uri(folder_uri));
  Glib::ustring file_name = top_dir.read_name();
  
  while (!file_name.empty()) {
    if (Glib::file_test(Glib::filename_from_uri(folder_uri) + "/" + file_name, Glib::FILE_TEST_IS_DIR)) {
      categorizeFilesInFolderRecursively(folder_uri + "/" + file_name);
    }
    
    else {
      categorizeFile(folder_uri + "/" + file_name);
    }
    
    file_name = top_dir.read_name();
  }
}
