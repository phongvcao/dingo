#include "dingo-mainwindow.h"
#include "dingo-dbmanager.h"
#include "dingo-playerengine.h"
#include "dingo-settings.h"
#include "dingo-mpriscommon.h"
#include "dingo-programoptions.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

#ifndef INCLUDED_GLIBMM_I18N_H
#define INCLUDED_GLIBMM_I18N_H
#include <glibmm/i18n.h>
#endif //INCLUDED_GLIBMM_I18N_H

//CONSTRUCTORS - DESTRUCTOR
Dingo::MainWindow::MainWindow() {

}

Dingo::MainWindow::MainWindow(Dingo::DBManager* dbmanager, Dingo::PlayerEngine* playerengine, Dingo::Settings* settings, Gtk::Application* dingo_app, Dingo::MPRISCommon* mpriscommon, Dingo::ProgramOptions* programoptions) :
  Gtk::Window(Gtk::WINDOW_TOPLEVEL)
{  
  d_dbmanager = dbmanager;
  d_playerengine = playerengine;
  d_settings = settings;
  d_mpriscommon = mpriscommon;
  d_programoptions = programoptions;
  
  d_settings->readFromDefault();
  
  createWidgets(dingo_app);
  packWidgets();
  
  set_title(_("Dingo Media Manager"));
  
  //set program name
  Glib::set_prgname(_("Dingo Media Manager"));
  Glib::set_application_name(_("Dingo"));
  
  d_settings->notifySettingsActionObserver(Dingo::SETTINGS_READ);
  
  show_all_children();
  
  set_resizable(true);
  
  d_videoarea->hide();
  
  d_processgrid->hide();
  
  d_toolbar->registerToolBarObserver(this);
  
  //Set ToggleButtons' states
  if (d_settings->getActive("home")) {
    d_toolbar->activateToggleButton(Dingo::HOME_TOGGLED);
  }
  
  if (d_settings->getActive("nowplaying")) {
    d_toolbar->activateToggleButton(Dingo::NOW_PLAYING_TOGGLED);
  }
  
  if (d_settings->getActive("playqueue")) {
    d_toolbar->activateToggleButton(Dingo::PLAY_QUEUE_TOGGLED);
  }
  
  if (d_settings->getActive("audioonly")) {
    d_toolbar->activateToggleButton(Dingo::AUDIO_FILTER_TOGGLED);
  }
  
  if (d_settings->getActive("videoonly")) {
    d_toolbar->activateToggleButton(Dingo::VIDEO_FILTER_TOGGLED);
  }
}

Dingo::MainWindow::~MainWindow() {

}
      
//INHERITED OBSERVER'S METHODS
void Dingo::MainWindow::updateHomeToggleActive() {
  d_videoarea->hide();
  d_processgrid_visible = d_processgrid->get_visible();

  top_vpaned->show_all();
  d_processgrid->set_visible(d_processgrid_visible);
}

void Dingo::MainWindow::updateNowPlayingToggleActive() {
  d_processgrid_visible = d_processgrid->get_visible();
  top_vpaned->hide();
  
  d_videoarea->show();
  d_processgrid->set_visible(d_processgrid_visible);
}

void Dingo::MainWindow::updatePlayQueueToggleActive() {
  d_processgrid_visible = d_processgrid->get_visible();
  
  top_vpaned->show_all();
  playlist_vbox.hide();
  genre_hpaned->hide();
  
  d_videoarea->hide();
  d_processgrid->set_visible(d_processgrid_visible);
}

void Dingo::MainWindow::updateFullscreenActivated() {
  d_processgrid_visible = d_processgrid->get_visible();

  show_all();

  d_menubar->hide();
  d_toolbar->hide();
  d_statusbar->hide();
  top_vpaned->hide();
  controlbar_separator_hbox.hide();
  d_processgrid->set_visible(d_processgrid_visible);
}

void Dingo::MainWindow::updateUnFullscreenActivated() {
  d_processgrid_visible = d_processgrid->get_visible();
  
  d_menubar->show_all();
  d_toolbar->show_all();
  d_videoarea->show_all();
  controlbar_separator_hbox.show_all();
  d_controlbar->show_all();
  d_statusbar->show_all();
  d_toolbar->activateToggleButton(Dingo::NOW_PLAYING_TOGGLED);  
  d_processgrid->set_visible(d_processgrid_visible);
}

void Dingo::MainWindow::updateSettingsRead() {
  //set size of the whole top window
  resize(d_settings->getWidth("topwindow"), d_settings->getHeight("topwindow"));
  
  //set divider of all the Gtk::Paned
  top_vpaned->set_position(d_settings->getHeight("topvpaned"));
  
  playlist_hpaned->set_position(d_settings->getWidth("playlisthpaned"));
  
  year_hpaned->set_position(d_settings->getWidth("yearhpaned"));
  
  artist_hpaned->set_position(d_settings->getWidth("artisthpaned"));
  
  genre_hpaned->set_position(d_settings->getWidth("genrehpaned"));
}

void Dingo::MainWindow::updateSettingsWrite() {
  int temp_topwindow_width = 1050;
  int temp_topwindow_height = 635;
  
  temp_topwindow_width = get_allocated_width();
  temp_topwindow_height = get_allocated_height();

  //write widths
  d_settings->setWidth("topwindow", temp_topwindow_width);
  d_settings->setWidth("playlisthpaned", playlist_hpaned->get_position());
  d_settings->setWidth("genrehpaned", genre_hpaned->get_position());
  d_settings->setWidth("artisthpaned", artist_hpaned->get_position());
  d_settings->setWidth("yearhpaned", year_hpaned->get_position());
  
  //write heights
  d_settings->setHeight("topwindow", temp_topwindow_height);
  d_settings->setHeight("topvpaned", top_vpaned->get_position());
}

//CREATORS
void Dingo::MainWindow::createWidgets(Gtk::Application* dingo_app) {
  d_dbmanager->setWindow(this);
  
  top_vpaned.reset(new Gtk::VPaned());
  year_hpaned.reset(new Gtk::HPaned());
  artist_hpaned.reset(new Gtk::HPaned());
  genre_hpaned.reset(new Gtk::HPaned());
  playlist_hpaned.reset(new Gtk::HPaned());
  
  d_statusbar.reset(new Dingo::StatusBar());
  d_menubar.reset(new Dingo::MenuBar(d_dbmanager, d_playerengine, d_settings, dingo_app));
  d_toolbar.reset(new Dingo::ToolBar(d_dbmanager, d_playerengine, d_settings, d_statusbar.get()));
  
  d_playlisttreeview.reset(new Dingo::PlaylistTreeView(d_dbmanager, d_settings, d_statusbar.get()));
  d_playlistscrolled.reset(new Gtk::ScrolledWindow());
  
  d_genretreeview.reset(new Dingo::GenreTreeView(d_dbmanager, d_settings));
  d_genrescrolled.reset(new Gtk::ScrolledWindow());
  
  d_artisttreeview.reset(new Dingo::ArtistTreeView(d_dbmanager, d_settings, d_statusbar.get()));
  d_artistscrolled.reset(new Gtk::ScrolledWindow());
  
  d_yeartreeview.reset(new Dingo::YearTreeView(d_dbmanager, d_settings));
  d_yearscrolled.reset(new Gtk::ScrolledWindow());
  
  d_albumtreeview.reset(new Dingo::AlbumTreeView(d_dbmanager, d_settings, d_statusbar.get()));
  d_albumscrolled.reset(new Gtk::ScrolledWindow());
  
  d_tracktreeview.reset(new Dingo::TrackTreeView(d_dbmanager, d_playerengine, d_settings, d_statusbar.get()));
  d_trackscrolled.reset(new Gtk::ScrolledWindow());
  
  d_playlistscrolled->set_shadow_type(Gtk::SHADOW_OUT);
  d_genrescrolled->set_shadow_type(Gtk::SHADOW_OUT);
  d_artistscrolled->set_shadow_type(Gtk::SHADOW_OUT);
  d_yearscrolled->set_shadow_type(Gtk::SHADOW_OUT);
  d_albumscrolled->set_shadow_type(Gtk::SHADOW_OUT);
  d_trackscrolled->set_shadow_type(Gtk::SHADOW_OUT);
  
  d_videoarea.reset(new Dingo::VideoArea(d_dbmanager, d_playerengine, d_statusbar.get()));
  d_controlbar.reset(new Dingo::ControlBar(d_dbmanager, d_playerengine, d_settings, d_statusbar.get()));
  d_statusicon.reset(new Dingo::StatusIcon(d_dbmanager, d_playerengine, dingo_app));
  d_processgrid.reset(new Dingo::ProcessGrid());
  d_notification.reset(new Dingo::Notification(d_playerengine));
  
  d_videoarea->signal_realize().connect(sigc::mem_fun(*this, &Dingo::MainWindow::onVideoAreaRealized));
  
  //Dingo::MPRISCommon
  d_mpriscommon->setToolBar(d_toolbar.get());
  d_mpriscommon->setTrackTreeView(d_tracktreeview.get());
  d_mpriscommon->setPlaylistTreeView(d_playlisttreeview.get());
  d_mpriscommon->setControlBar(d_controlbar.get());
  d_mpriscommon->setStatusIcon(d_statusicon.get());
  
  //Dingo::ProgramOptions
  d_programoptions->setToolBar(d_toolbar.get());
  d_programoptions->setTrackTreeView(d_tracktreeview.get());
  d_programoptions->setPlaylistTreeView(d_playlisttreeview.get());
  
  //Dingo::Notification
  d_tracktreeview->registerTrackTreeViewObserver(d_notification.get());
  
  //Dingo::MainWindow
  d_playerengine->registerFullscreenObserver(this);
  
  //Dingo::MenuBar
  d_toolbar->registerToolBarObserver(d_menubar.get());
  d_toolbar->registerAudioVideoFilterObserver(d_menubar.get());
  d_playerengine->registerFullscreenObserver(d_menubar.get());
  d_playerengine->registerEngineStatusObserver(d_menubar.get());
  d_playerengine->registerPlayOrderObserver(d_menubar.get());
  d_playerengine->registerRepeatToggleObserver(d_menubar.get());
  
  //Dingo::ToolBar
  d_tracktreeview->registerTrackTreeViewObserver(d_toolbar.get());
  d_menubar->setToolBar(d_toolbar.get());
  
  //Dingo::PlaylistTreeView
  d_dbmanager->registerDBManPlaylistObserver(d_playlisttreeview.get());
  d_tracktreeview->delegateTrackPlaylistEditWindowObserver(d_playlisttreeview.get());
  d_menubar->setPlaylistTreeView(d_playlisttreeview.get(), d_playlistscrolled.get(), playlist_hpaned.get());
  
  //Dingo::GenreTreeView
  d_toolbar->registerToolBarObserver(d_genretreeview.get());
  d_toolbar->registerSearchEntryObserver(d_genretreeview.get());
  d_toolbar->registerAudioVideoFilterObserver(d_genretreeview.get());
  d_dbmanager->registerDBManGenreObserver(d_genretreeview.get());
  d_tracktreeview->delegateTrackPlaylistEditWindowObserver(d_genretreeview.get());
  d_menubar->setGenreTreeView(d_genretreeview.get(), d_genrescrolled.get(), genre_hpaned.get());
  
  //Dingo::ArtistTreeView
  d_toolbar->registerToolBarObserver(d_artisttreeview.get());
  d_toolbar->registerSearchEntryObserver(d_artisttreeview.get());
  d_toolbar->registerAudioVideoFilterObserver(d_artisttreeview.get());
  d_dbmanager->registerDBManArtistObserver(d_artisttreeview.get());
  d_tracktreeview->delegateTrackPlaylistEditWindowObserver(d_artisttreeview.get());
  d_genretreeview->registerGenreTreeViewObserver(d_artisttreeview.get());
  d_menubar->setArtistTreeView(d_artisttreeview.get(), d_artistscrolled.get(), artist_hpaned.get());
  
  //Dingo::YearTreeView
  d_toolbar->registerToolBarObserver(d_yeartreeview.get());
  d_toolbar->registerSearchEntryObserver(d_yeartreeview.get());
  d_toolbar->registerAudioVideoFilterObserver(d_yeartreeview.get());
  d_dbmanager->registerDBManYearObserver(d_yeartreeview.get());
  d_tracktreeview->delegateTrackPlaylistEditWindowObserver(d_yeartreeview.get());
  d_genretreeview->registerGenreTreeViewObserver(d_yeartreeview.get());
  d_artisttreeview->registerArtistTreeViewObserver(d_yeartreeview.get());
  d_menubar->setYearTreeView(d_yeartreeview.get(), d_yearscrolled.get(), year_hpaned.get());
  
  //Dingo::AlbumTreeView
  d_toolbar->registerToolBarObserver(d_albumtreeview.get());
  d_toolbar->registerSearchEntryObserver(d_albumtreeview.get());
  d_toolbar->registerAudioVideoFilterObserver(d_albumtreeview.get());
  d_genretreeview->registerGenreTreeViewObserver(d_albumtreeview.get());
  d_artisttreeview->registerArtistTreeViewObserver(d_albumtreeview.get());
  d_yeartreeview->registerYearTreeViewObserver(d_albumtreeview.get());
  d_dbmanager->registerDBManAlbumObserver(d_albumtreeview.get());
  d_tracktreeview->delegateTrackPlaylistEditWindowObserver(d_albumtreeview.get());
  d_menubar->setAlbumTreeView(d_albumtreeview.get(), d_albumscrolled.get());
  
  //Dingo::TrackTreeView
  d_toolbar->registerToolBarObserver(d_tracktreeview.get());
  d_toolbar->registerSearchEntryObserver(d_tracktreeview.get());
  d_toolbar->registerAudioVideoFilterObserver(d_tracktreeview.get());
  d_genretreeview->registerGenreTreeViewObserver(d_tracktreeview.get());
  d_artisttreeview->registerArtistTreeViewObserver(d_tracktreeview.get());
  d_yeartreeview->registerYearTreeViewObserver(d_tracktreeview.get());
  d_albumtreeview->registerAlbumTreeViewObserver(d_tracktreeview.get());
  d_playerengine->registerRepeatModeObserver(d_tracktreeview.get());
  d_playerengine->registerEngineNavigationObserver(d_tracktreeview.get());
  d_dbmanager->registerDBManTrackObserver(d_tracktreeview.get());
  d_tracktreeview->delegateTrackPlaylistEditWindowObserver(d_tracktreeview.get());
  d_menubar->setTrackTreeView(d_tracktreeview.get(), d_trackscrolled.get());
  
  //PlaylistTreeViewObserver
  d_playlisttreeview->registerPlaylistTreeViewObserver(d_genretreeview.get());
  d_playlisttreeview->registerPlaylistTreeViewObserver(d_artisttreeview.get());
  d_playlisttreeview->registerPlaylistTreeViewObserver(d_yeartreeview.get());
  d_playlisttreeview->registerPlaylistTreeViewObserver(d_albumtreeview.get());
  d_playlisttreeview->registerPlaylistTreeViewObserver(d_tracktreeview.get());
  
  //Dingo::VideoArea
  d_playerengine->registerFullscreenObserver(d_videoarea.get());
  d_playerengine->registerEngineStatusObserver(d_videoarea.get());
  
  //Dingo::ControlBar
  d_playerengine->registerFullscreenObserver(d_controlbar.get());
  d_videoarea->registerMouseMotionObserver(d_controlbar.get());
  d_playerengine->registerRepeatModeObserver(d_controlbar.get());
  d_playerengine->registerEngineStatusObserver(d_controlbar.get());
  d_playerengine->registerRepeatToggleObserver(d_controlbar.get());
  d_menubar->setControlBar(d_controlbar.get());
  d_statusicon->setControlBar(d_controlbar.get());
  
  //Dingo::StatusBar
  d_menubar->setStatusIcon(d_statusicon.get());
  
  //Dingo::StatusIcon
  d_playerengine->registerEngineStatusObserver(d_statusicon.get());
  d_playerengine->registerPlayOrderObserver(d_statusicon.get());
  d_playerengine->registerRepeatToggleObserver(d_statusicon.get());
  d_tracktreeview->registerTrackTreeViewObserver(d_statusicon.get());
  d_menubar->setStatusIcon(d_statusicon.get());
  
  //Dingo::ProcessGrid
  d_dbmanager->setProcessGrid(d_processgrid.get());
  
  //Dingo::Settings registers its observers
  //ORDER MATTERS!!!
  d_settings->registerSettingsActionObserver(this);
  d_settings->registerSettingsActionObserver(d_tracktreeview.get());
  d_settings->registerSettingsActionObserver(d_menubar.get());
  d_settings->registerSettingsActionObserver(d_toolbar.get());
  d_settings->registerSettingsActionObserver(d_controlbar.get());
  
  //Dingo::DBManager
  d_processgrid->registerProcessGridObserver(d_dbmanager);
}

void Dingo::MainWindow::packWidgets() {
  d_playlistscrolled->add(*d_playlisttreeview);
  d_genrescrolled->add(*d_genretreeview);
  d_artistscrolled->add(*d_artisttreeview);
  d_yearscrolled->add(*d_yeartreeview);
  d_albumscrolled->add(*d_albumtreeview);
  d_trackscrolled->add(*d_tracktreeview);
  
  d_playlistscrolled->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  d_genrescrolled->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  d_artistscrolled->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  d_yearscrolled->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  d_albumscrolled->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  d_trackscrolled->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  
  year_hpaned->pack1(*d_yearscrolled, Gtk::EXPAND);
  year_hpaned->pack2(*d_albumscrolled, Gtk::EXPAND);
  
  artist_hpaned->pack1(*d_artistscrolled, Gtk::EXPAND);
  artist_hpaned->pack2(*year_hpaned, Gtk::EXPAND);
  
  genre_hpaned->pack1(*d_genrescrolled, Gtk::EXPAND);
  genre_hpaned->pack2(*artist_hpaned, Gtk::EXPAND);
  
  playlist_vbox.pack_start(*d_playlistscrolled, Gtk::PACK_EXPAND_WIDGET);
  playlist_vbox.pack_start(*d_processgrid, Gtk::PACK_SHRINK);
  playlist_hpaned->pack1(playlist_vbox, Gtk::EXPAND);
  playlist_hpaned->pack2(*d_trackscrolled, Gtk::EXPAND);
  
  top_vpaned->pack1(*genre_hpaned, Gtk::EXPAND);
  top_vpaned->pack2(*playlist_hpaned, Gtk::EXPAND);
  
  //separator
  controlbar_separator_hbox.set_size_request(5, 5);
  
  top_vbox.pack_start(*d_menubar, Gtk::PACK_SHRINK);
  top_vbox.pack_start(*d_toolbar, Gtk::PACK_SHRINK, 4);
  top_vbox.pack_start(*top_vpaned, Gtk::PACK_EXPAND_WIDGET);
  top_vbox.pack_start(*d_videoarea, Gtk::PACK_EXPAND_WIDGET);
  top_vbox.pack_start(controlbar_separator_hbox, Gtk::PACK_SHRINK);
  top_vbox.pack_start(*d_controlbar, Gtk::PACK_SHRINK);
  top_vbox.pack_start(*d_statusbar, Gtk::PACK_SHRINK);
  
  add(top_vbox);
}

//SIGNAL HANDLERS
void Dingo::MainWindow::onVideoAreaRealized() {
  d_playerengine->setXWindowID(GDK_WINDOW_XID(d_videoarea->get_window()->gobj()));
}

void Dingo::MainWindow::on_size_allocate(Gtk::Allocation& allocation) {
  d_toolbar->adjustDividerPosition(allocation.get_width());
  
  Gtk::Window::on_size_allocate(allocation);
}

bool Dingo::MainWindow::on_delete_event(GdkEventAny* event) {
  d_statusicon->updateShowMainWindow(false);
  d_dbmanager->saveMainWindowPosition();

  return Gtk::Window::on_delete_event(event);
}
