/* 
 * dingo-mainwindow.h
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

#ifndef INCLUDED_DINGO_MAINWINDOW_H
#define INCLUDED_DINGO_MAINWINDOW_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_MEMORY
#define INCLUDED_MEMORY
#include <memory>
#endif //INCLUDED_MEMORY

#include "dingo-menubar.h"
#include "dingo-toolbar.h"
#include "dingo-playlisttreeview.h"
#include "dingo-genretreeview.h"
#include "dingo-artisttreeview.h"
#include "dingo-yeartreeview.h"
#include "dingo-albumtreeview.h"
#include "dingo-tracktreeview.h"
#include "dingo-videoarea.h"
#include "dingo-controlbar.h"
#include "dingo-statusbar.h"
#include "dingo-statusicon.h"
#include "dingo-processgrid.h"
#include "dingo-notification.h"

#include "dingo-toolbarobserver.h"
#include "dingo-fullscreenobserver.h"
#include "dingo-settingsactionobserver.h"

namespace Dingo {
  class DBManager;
  class PlayerEngine;
  class Settings;
  class MPRISCommon;
  class ProgramOptions;

  class MainWindow : public Gtk::Window, public ToolBarObserver, public FullscreenObserver, public SettingsActionObserver {
    public:
      MainWindow();
      MainWindow(DBManager* dbmanager, PlayerEngine* playerengine, Settings* settings, Gtk::Application* dingo_app, MPRISCommon* mpriscommon, ProgramOptions* programoptions);
      virtual ~MainWindow();
      
      //INHERITED OBSERVER'S METHODS
      void updateHomeToggleActive();
      void updateNowPlayingToggleActive();
      void updatePlayQueueToggleActive();
      
      void updateFullscreenActivated();
      void updateUnFullscreenActivated();
      
      void updateSettingsRead();
      void updateSettingsWrite();
    
    private:
      //CREATORS
      void createWidgets(Gtk::Application* dingo_app);
      void packWidgets();
      
      //SIGNAL HANDLERS
      void onVideoAreaRealized();
      bool onSearchEntryChangedTimeOut();
     
      void on_size_allocate(Gtk::Allocation& allocation);
      bool on_delete_event(GdkEventAny* event);
    
      //DATA MEMBERS
      std::auto_ptr<MenuBar> d_menubar;
      std::auto_ptr<ToolBar> d_toolbar;
      
      std::auto_ptr<PlaylistTreeView> d_playlisttreeview;
      std::auto_ptr<Gtk::ScrolledWindow> d_playlistscrolled;
      
      std::auto_ptr<GenreTreeView> d_genretreeview;
      std::auto_ptr<Gtk::ScrolledWindow> d_genrescrolled;
      
      std::auto_ptr<ArtistTreeView> d_artisttreeview;
      std::auto_ptr<Gtk::ScrolledWindow> d_artistscrolled;
      
      std::auto_ptr<YearTreeView> d_yeartreeview;
      std::auto_ptr<Gtk::ScrolledWindow> d_yearscrolled;
      
      std::auto_ptr<AlbumTreeView> d_albumtreeview;
      std::auto_ptr<Gtk::ScrolledWindow> d_albumscrolled;
      
      std::auto_ptr<TrackTreeView> d_tracktreeview;
      std::auto_ptr<Gtk::ScrolledWindow> d_trackscrolled;
      
      std::auto_ptr<VideoArea> d_videoarea;
      std::auto_ptr<ControlBar> d_controlbar;
      std::auto_ptr<StatusBar> d_statusbar;
      std::auto_ptr<StatusIcon> d_statusicon;
      std::auto_ptr<ProcessGrid> d_processgrid;
      std::auto_ptr<Notification> d_notification;
      
      bool d_processgrid_visible;
      
      std::auto_ptr<Gtk::VPaned> top_vpaned;
      std::auto_ptr<Gtk::HPaned> year_hpaned, artist_hpaned, genre_hpaned, playlist_hpaned;
      Gtk::HBox controlbar_separator_hbox;
      
      Gtk::VBox playlist_vbox, top_vbox;
      Glib::RefPtr<Gdk::Pixbuf> d_dingo_icon;
      
      DBManager* d_dbmanager;
      PlayerEngine* d_playerengine;
      Settings* d_settings;
      MPRISCommon* d_mpriscommon;
      ProgramOptions* d_programoptions;
  };
}

#endif //INCLUDED_DINGO_MAINWINDOW_H
