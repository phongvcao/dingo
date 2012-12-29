#include "dingo-statusicon.h"
#include "dingo-controlbar.h"
#include "dingo-aboutdialog.h"
#include "dingo-dbmanager.h"
#include "dingo-playerengine.h"
#include "dingo-utilities.h"

#ifndef INCLUDED_GLIBMM_I18N_H
#define INCLUDED_GLIBMM_I18N_H
#include <glibmm/i18n.h>
#endif //INCLUDED_GLIBMM_I18N_H

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

//CONSTRUCTORS - DESTRUCTOR
Dingo::StatusIcon::StatusIcon(Dingo::DBManager* dbmanager, Dingo::PlayerEngine* playerengine, Gtk::Application* dingo_app) :
  Gtk::StatusIcon(),
  status_play(_("Play")), 
  status_next(_("Next")), 
  status_previous(_("Previous")), 
  status_repeat(_("Repeat")), 
  status_shuffle(_("Shuffle")), 
  status_show_window(_("Show Main Window")), 
  status_about(_("About")), 
  status_quit(_("Quit")),
  d_miniplayer_win(Gtk::WINDOW_POPUP),
  d_miniplayer_preview_image(playerengine->getPreviewImage())
{
  d_dbmanager = dbmanager;
  d_playerengine = playerengine;
  d_dingo_app = dingo_app;
  
  is_play_toggled_here = false;
  is_repeat_toggled_here = false;
  is_playorder_toggled_here = false;
  is_pos_toggled_here = false;
  is_dur_toggled_here = false;
  
  d_is_right_clicked = false;
  
  createStatusIcon();
  createPopupMenu();
  createMiniPlayer();
}

Dingo::StatusIcon::~StatusIcon() {

}
      
//PUBLIC ACCESSORS
void Dingo::StatusIcon::setControlBar(Dingo::ControlBar* controlbar) {
  d_controlbar = controlbar;
}

void Dingo::StatusIcon::setShowMainWindowToggle(bool is_active) {
  status_show_window.set_active(is_active);
}

void Dingo::StatusIcon::raise() {
  if (status_show_window.get_active()) {
    if (!d_dbmanager->getWindow()->property_is_active()) {
      d_dbmanager->getWindow()->present();
    }
      
    else {
      status_show_window.set_active(false);
    }
  }
      
  else {
    status_show_window.set_active(true);
  }
}

//INHERITED OBSERVER'S METHODS
void Dingo::StatusIcon::updateEnginePlaying() {
  if (is_play_toggled_here) {
    is_play_toggled_here = false;
  }
  
  else {
    status_play_connection.block();
    
    status_play.set_active(true);
    
    status_play_connection.unblock();
  }
  
  connectToMiniPlayerTimeOut();
  
  enableWidgets();
}

void Dingo::StatusIcon::updateEnginePaused() {
  if (is_play_toggled_here) {
    is_play_toggled_here = false;
  }
  
  else {
    status_play_connection.block();
    
    status_play.set_active(false);
    
    status_play_connection.unblock();
  }
  
  connectToMiniPlayerTimeOut();
  
  enableWidgets();
}

void Dingo::StatusIcon::updateEngineStopped() {
  if (is_play_toggled_here) {
    is_play_toggled_here = false;
  }
  
  else {
    status_play_connection.block();
    
    status_play.set_active(false);
    
    status_play_connection.unblock();
  }
  
  d_miniplayer_hscale.set_value(0);
  
  d_miniplayer_pos_connection.block();
  d_miniplayer_pos_toggle.set_active(false);
  d_miniplayer_pos_connection.unblock();
  
  d_miniplayer_dur_connection.block();
  d_miniplayer_dur_toggle.set_active(false);
  d_miniplayer_dur_connection.unblock();
  
  d_miniplayer_pos_label.set_markup("0:00 | 0:00");
  d_miniplayer_dur_label.set_markup("0:00 | 0:00");
  
  disconnectFromMiniPlayerTimeOut();
  
  disableWidgets();
}

void Dingo::StatusIcon::updateEngineNewPlaying() {
  updateEnginePlaying();
}
      
void Dingo::StatusIcon::updatePlayOrderNormal() {
  if (is_playorder_toggled_here) {
    is_playorder_toggled_here = false;
  }
  
  else {
    status_shuffle_connection.block();
    
    status_shuffle.set_active(false);
    
    status_shuffle_connection.unblock();
  }
}

void Dingo::StatusIcon::updatePlayOrderShuffle() {
  if (is_playorder_toggled_here) {
    is_playorder_toggled_here = false;
  }
  
  else {
    status_shuffle_connection.block();
    
    status_shuffle.set_active(true);
    
    status_shuffle_connection.unblock();
  }
}
      
void Dingo::StatusIcon::updateRepeatOffToggled() {
  if (is_repeat_toggled_here) {
    is_repeat_toggled_here = false;
  }
  
  else {
    status_repeat_off_connection.block();
    status_repeat_single_connection.block();
    status_repeat_playlist_connection.block();
    status_repeat_section_connection.block();
    
    status_repeat_off->set_active(true);
    
    status_repeat_off_connection.unblock();
    status_repeat_single_connection.unblock();
    status_repeat_playlist_connection.unblock();
    status_repeat_section_connection.unblock();
    
    d_miniplayer_pos_connection.block();
    d_miniplayer_pos_toggle.set_active(false);
    d_miniplayer_pos_connection.unblock();
      
    d_miniplayer_dur_connection.block();
    d_miniplayer_dur_toggle.set_active(false);
    d_miniplayer_dur_connection.unblock();
  }
  
  status_repeat_section->set_sensitive(false);
  status_repeat_section->set_label(_("Repeat Section"));
}

void Dingo::StatusIcon::updateRepeatSingleToggled() {
  if (is_repeat_toggled_here) {
    is_repeat_toggled_here = false;
  }
  
  else {
    status_repeat_off_connection.block();
    status_repeat_single_connection.block();
    status_repeat_playlist_connection.block();
    status_repeat_section_connection.block();
    
    status_repeat_single->set_active(true);
    
    status_repeat_off_connection.unblock();
    status_repeat_single_connection.unblock();
    status_repeat_playlist_connection.unblock();
    status_repeat_section_connection.unblock();
  
    d_miniplayer_pos_connection.block();
    d_miniplayer_pos_toggle.set_active(false);
    d_miniplayer_pos_connection.unblock();
      
    d_miniplayer_dur_connection.block();
    d_miniplayer_dur_toggle.set_active(false);
    d_miniplayer_dur_connection.unblock();
  }
  
  status_repeat_section->set_sensitive(false);
  status_repeat_section->set_label(_("Repeat Section"));
}

void Dingo::StatusIcon::updateRepeatPlaylistToggled() {
  if (is_repeat_toggled_here) {
    is_repeat_toggled_here = false;
  }
  
  else {
    status_repeat_off_connection.block();
    status_repeat_single_connection.block();
    status_repeat_playlist_connection.block();
    status_repeat_section_connection.block();
    
    status_repeat_playlist->set_active(true);
    
    status_repeat_off_connection.unblock();
    status_repeat_single_connection.unblock();
    status_repeat_playlist_connection.unblock();
    status_repeat_section_connection.unblock();
    
    d_miniplayer_pos_connection.block();
    d_miniplayer_pos_toggle.set_active(false);
    d_miniplayer_pos_connection.unblock();
      
    d_miniplayer_dur_connection.block();
    d_miniplayer_dur_toggle.set_active(false);
    d_miniplayer_dur_connection.unblock();
  }
  
  status_repeat_section->set_sensitive(false);
  status_repeat_section->set_label(_("Repeat Section"));
}

void Dingo::StatusIcon::updateRepeatSectionToggled(Glib::ustring section_button_code) {
  status_repeat_section->set_sensitive(true);
    
  if (is_repeat_toggled_here) {
    is_repeat_toggled_here = false;
  }
  
  else {
    status_repeat_off_connection.block();
    status_repeat_single_connection.block();
    status_repeat_playlist_connection.block();
    status_repeat_section_connection.block();
    
    status_repeat_section->set_active(true);
    
    status_repeat_off_connection.unblock();
    status_repeat_single_connection.unblock();
    status_repeat_playlist_connection.unblock();
    status_repeat_section_connection.unblock();
  }
  
  status_repeat_section->set_label(Glib::ustring::compose(_("Repeat %1 -> %2"), d_playerengine->getSectionStartString(), d_playerengine->getSectionEndString()));
  
  if (section_button_code == "section-start") {
    if (!is_pos_toggled_here) {
      d_miniplayer_pos_connection.block();
      
      d_miniplayer_pos_toggle.set_active(!d_miniplayer_pos_toggle.get_active());
    
      d_miniplayer_pos_connection.unblock();
    }
  }
  
  else if (section_button_code == "section-end") {
    if (!is_dur_toggled_here) {
      d_miniplayer_dur_connection.block();
      
      d_miniplayer_dur_toggle.set_active(!d_miniplayer_dur_toggle.get_active());
    
      d_miniplayer_dur_connection.unblock();
    }
  }
}

void Dingo::StatusIcon::updateShowMainWindow(bool is_mainwindow_showing) {
  if (!status_show_window_connection.blocked()) {
    status_show_window_connection.block();
  }
  
  status_show_window.set_active(is_mainwindow_showing);
  
  if (status_show_window_connection.blocked()) {
    status_show_window_connection.unblock();
  }
}

void Dingo::StatusIcon::updateTrackTreeRowActivated(Gtk::TreeRowReference cur_track_row_ref) {
  Gtk::TreeModel::Row temp_cur_row = *(Dingo::DBManager::trackModel->get_iter(cur_track_row_ref.get_path()));
  
  d_miniplayer_preview_image.set(d_playerengine->getPreviewImage());
  
  //set info_area information
  d_marked_up_track_title_str = Glib::ustring::compose("<span font='Bold 14'>%1</span>", temp_cur_row[(*Dingo::DBManager::trackCR).trackTitle]);
  
  d_marked_up_track_artist_str = Glib::ustring::compose(_("by %1 from %2"), "<i>" + temp_cur_row[(*Dingo::DBManager::trackCR).trackArtist] + "</i>", "<i>" + temp_cur_row[(*Dingo::DBManager::trackCR).trackAlbum] + "</i>");
  
  if (d_miniplayer_win.get_visible()) {
    d_miniplayer_track_title.set_markup(d_marked_up_track_title_str);
    d_miniplayer_track_artist.set_markup(d_marked_up_track_artist_str);
  }
}

void Dingo::StatusIcon::updateTrackTreeRowDeactivated() {
  d_marked_up_track_title_str = "<span font='Bold 14'>" + Glib::ustring(_("Not Playing")) + "</span>";
  
  d_marked_up_track_artist_str = "";
  
  if (d_miniplayer_win.get_visible()) {
    d_miniplayer_track_title.set_markup(d_marked_up_track_title_str);
    d_miniplayer_track_artist.set_markup(d_marked_up_track_artist_str);
  }
  
  d_miniplayer_preview_image.set(d_playerengine->getDefaultPreviewImage());
}
    
//CREATORS
void Dingo::StatusIcon::createStatusIcon() {
  set_from_file(DATADIR"/dingo/icons/24x24/dingo.png");
  
  set_title("Dingo");
  set_has_tooltip(true);
  
  set_visible(true);
}

void Dingo::StatusIcon::createPopupMenu() {
  status_repeat_off.reset(new Gtk::RadioMenuItem(status_repeat_group, _("Repeat Off")));
  status_repeat_single.reset(new Gtk::RadioMenuItem(status_repeat_group, _("Repeat Single")));
  status_repeat_playlist.reset(new Gtk::RadioMenuItem(status_repeat_group, _("Repeat Playlist")));
  status_repeat_section.reset(new Gtk::RadioMenuItem(status_repeat_group, _("Repeat Section")));

  status_repeat_off->set_active(true);
  status_show_window.set_active(true);
  status_repeat_section->set_sensitive(false);

  status_play_connection = status_play.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onPlayToggled));
  
  status_next.signal_activate().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onNextActivated));
  status_previous.signal_activate().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onPreviousActivated));
  
  status_repeat_off_connection = status_repeat_off->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onRepeatModeToggled));
  status_repeat_playlist_connection = status_repeat_playlist->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onRepeatModeToggled));
  status_repeat_single_connection = status_repeat_single->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onRepeatModeToggled));
  status_repeat_section_connection = status_repeat_section->signal_toggled().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onRepeatModeToggled));
  
  status_shuffle_connection = status_shuffle.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onShuffleToggled));
  
  status_show_window_connection = status_show_window.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onShowWindowToggled));
  status_about.signal_activate().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onAboutActivated));
  
  status_quit.signal_activate().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onQuitActivated));
  
  status_repeat_submenu.append(*status_repeat_off);
  status_repeat_submenu.append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
  status_repeat_submenu.append(*status_repeat_playlist);
  status_repeat_submenu.append(*status_repeat_single);
  status_repeat_submenu.append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
  status_repeat_submenu.append(*status_repeat_section);
  
  status_repeat.set_submenu(status_repeat_submenu);
  
  status_popup_menu.append(status_play);
  status_popup_menu.append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
  status_popup_menu.append(status_next);
  status_popup_menu.append(status_previous);
  status_popup_menu.append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
  status_popup_menu.append(status_repeat);
  status_popup_menu.append(status_shuffle);
  status_popup_menu.append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
  status_popup_menu.append(status_show_window);
  status_popup_menu.append(status_about);
  status_popup_menu.append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
  status_popup_menu.append(status_quit);
  
  status_repeat_submenu.show_all();
  status_popup_menu.show_all();
}

void Dingo::StatusIcon::createMiniPlayer() {
  d_miniplayer_preview_image.set_size_request(60, 60);
  d_marked_up_track_title_str = "<span font='Bold 14'>" + Glib::ustring(_("Not Playing")) + "</span>";
  
  d_miniplayer_info_grid.attach(d_miniplayer_preview_image, 1, 0, 2, 2);
  d_miniplayer_info_grid.attach(d_miniplayer_info_left_padding_hbox, 3, 0, 1, 2);
  d_miniplayer_info_grid.attach(d_miniplayer_track_title, 4, 0, 1, 1);
  d_miniplayer_info_grid.attach(d_miniplayer_track_artist, 4, 1, 1, 1);
  
  d_miniplayer_track_title.set_ellipsize(Pango::ELLIPSIZE_END);
  d_miniplayer_track_artist.set_ellipsize(Pango::ELLIPSIZE_END);
  
  d_miniplayer_track_title.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_END);
  d_miniplayer_track_artist.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_START);
  
  d_miniplayer_info_grid.set_row_spacing(5);
  d_miniplayer_info_grid.set_column_spacing(5);
  d_miniplayer_info_grid.show_all();
  
  //now go construct the control_hbox section
  d_miniplayer_pos_label.set_markup("0:00 | 0:00");
  d_miniplayer_dur_label.set_markup("0:00 | 0:00");
  d_miniplayer_pos_toggle.add(d_miniplayer_pos_label);
  d_miniplayer_dur_toggle.add(d_miniplayer_dur_label);
  
  d_miniplayer_pos_connection = d_miniplayer_pos_toggle.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onPositionButtonToggled));
  d_miniplayer_dur_connection = d_miniplayer_dur_toggle.signal_toggled().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onDurationButtonToggled));
  d_miniplayer_hide_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onMiniPlayerHideTimeOut), 2000);
  
  d_miniplayer_hscale.set_draw_value(false);
  d_miniplayer_hscale.signal_change_value().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onMiniPlayerHScaleChangeValue));
  
  d_miniplayer_control_hbox.pack_start(d_miniplayer_pos_toggle, Gtk::PACK_SHRINK);
  d_miniplayer_control_hbox.pack_start(d_miniplayer_hscale, Gtk::PACK_EXPAND_WIDGET);
  d_miniplayer_control_hbox.pack_start(d_miniplayer_dur_toggle, Gtk::PACK_SHRINK);
  
  d_miniplayer_top_vbox.pack_start(d_miniplayer_info_grid, Gtk::PACK_EXPAND_WIDGET);
  d_miniplayer_top_vbox.pack_start(*Gtk::manage(new Gtk::HSeparator()), Gtk::PACK_EXPAND_WIDGET, 7);
  d_miniplayer_top_vbox.pack_start(d_miniplayer_control_hbox, Gtk::PACK_EXPAND_WIDGET);
  d_miniplayer_top_vbox.set_border_width(7);
  
  d_miniplayer_win.set_size_request(350, 122);
  d_miniplayer_win.add(d_miniplayer_top_vbox);
  
  disableWidgets();
}

void Dingo::StatusIcon::onPositionButtonToggled() {
  is_pos_toggled_here = true;

  if (d_miniplayer_pos_toggle.get_active()) {
    d_playerengine->setSectionStart(d_playerengine->getPercentage());
    
    d_playerengine->setRepeatMode(Dingo::REPEAT_SECTION, "section-start");
    
    d_playerengine->notifyRepeatModeObserver(Dingo::REPEAT_SECTION);
  }
  
  else {
    d_playerengine->setSectionStart(0);
    
    if (!d_miniplayer_dur_toggle.get_active()) {
      d_playerengine->setRepeatMode(Dingo::REPEAT_OFF);
    }
    
    else {
      d_playerengine->setRepeatMode(Dingo::REPEAT_SECTION, "section-start");
    }
  }
  
  is_pos_toggled_here = false;
}

void Dingo::StatusIcon::onDurationButtonToggled() {
  is_dur_toggled_here = true;

  if (d_miniplayer_dur_toggle.get_active()) {
    d_playerengine->setSectionEnd(d_playerengine->getPercentage());
    
    d_playerengine->setRepeatMode(Dingo::REPEAT_SECTION, "section-end");
    
    d_playerengine->notifyRepeatModeObserver(Dingo::REPEAT_SECTION);
    
    d_playerengine->setPercentage(d_playerengine->getSectionStart());
  }
  
  else {
    d_playerengine->setSectionEnd(1);
    
    if (!d_miniplayer_pos_toggle.get_active()) {
      d_playerengine->setRepeatMode(Dingo::REPEAT_OFF);
    }
    
    else {
      d_playerengine->setRepeatMode(Dingo::REPEAT_SECTION, "section-end");
    }
  }
  
  is_dur_toggled_here = false;
}

bool Dingo::StatusIcon::onMiniPlayerHScaleChangeValue(Gtk::ScrollType scroll, double new_value) {
  d_playerengine->setPercentage(new_value);
  
  return false;
}

//SIGNAL HANDLERS
void Dingo::StatusIcon::onPlayToggled() {
  is_play_toggled_here = true;

  d_controlbar->onPlayButtonClicked();
}

void Dingo::StatusIcon::onNextActivated() {
  d_controlbar->onNextButtonClicked();
}

void Dingo::StatusIcon::onPreviousActivated() {
  d_controlbar->onPreviousButtonClicked();
}

void Dingo::StatusIcon::onRepeatModeToggled() {
  is_repeat_toggled_here = true;

  if (status_repeat_off->get_active()) {
    d_controlbar->setRepeatState(Dingo::REPEAT_OFF);
    d_playerengine->setRepeatMode(Dingo::REPEAT_OFF);
  }
  
  else if (status_repeat_single->get_active()) {
    d_controlbar->setRepeatState(Dingo::REPEAT_SINGLE);
    d_playerengine->setRepeatMode(Dingo::REPEAT_SINGLE);
  }
  
  else if (status_repeat_playlist->get_active()) {
    d_controlbar->setRepeatState(Dingo::REPEAT_PLAYLIST);
    d_playerengine->setRepeatMode(Dingo::REPEAT_PLAYLIST);
  }
}

void Dingo::StatusIcon::onShuffleToggled() {
  is_playorder_toggled_here = true;

  if (status_shuffle.get_active()) {
    d_controlbar->setShuffleToggle(true);
  }
  
  else {
    d_controlbar->setShuffleToggle(false);
  }
}

void Dingo::StatusIcon::onShowWindowToggled() {
  if (status_show_window.get_active()) {
    d_dbmanager->setMainWindowVisible(true);
  }
  
  else {
    d_dbmanager->setMainWindowVisible(false);
  }
}

void Dingo::StatusIcon::onAboutActivated() {
  Dingo::AboutDialog dialog;
  
  dialog.run();
}

void Dingo::StatusIcon::onQuitActivated() {
  d_dingo_app->release();
  
  d_dbmanager->getWindow()->hide();
}
      
bool Dingo::StatusIcon::on_button_press_event(GdkEventButton* event) {
  if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3)) {
    Gtk::StatusIcon::popup_menu_at_position(status_popup_menu, event->button, event->time);
    
    d_is_right_clicked = true;
    
    return true;
  }
  
  else if ((event->type == GDK_BUTTON_PRESS) && (event->button == 1)) {
    
    raise();
    
    return true;
  }
  
  bool return_value = false;
  
  return_value = Gtk::StatusIcon::on_button_press_event(event);
  
  return return_value;
}

bool Dingo::StatusIcon::on_query_tooltip(int x, int y, bool keyboard_mode, const Glib::RefPtr<Gtk::Tooltip>& tooltip) {  
  Glib::RefPtr<Gdk::Screen> screen;
  Gdk::Rectangle statusicon_rectangle;
  Gtk::Orientation traybar_orientation;
  
  int d_miniplayer_win_x = 0;
  int d_miniplayer_win_y = 0;
  
  Gtk::StatusIcon::get_geometry(screen, statusicon_rectangle, traybar_orientation);

  Dingo::Utilities::locatePopupWidgetPosition(d_miniplayer_win_x, d_miniplayer_win_y, 350, 122, screen, statusicon_rectangle, traybar_orientation);

  d_miniplayer_win.move(d_miniplayer_win_x, d_miniplayer_win_y);

  d_miniplayer_win.show_all();
  
  d_miniplayer_track_title.set_markup(d_marked_up_track_title_str);
  d_miniplayer_track_artist.set_markup(d_marked_up_track_artist_str);
  
  return false;
}

//UTILITIES
void Dingo::StatusIcon::connectToMiniPlayerTimeOut() {
  if (d_miniplayer_timeout_connection.connected()) {
    d_miniplayer_timeout_connection.disconnect();
  }

  d_miniplayer_timeout_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &Dingo::StatusIcon::onMiniPlayerTimeOut), 100);
}

void Dingo::StatusIcon::disconnectFromMiniPlayerTimeOut() {
  if (d_miniplayer_timeout_connection.connected()) {
    d_miniplayer_timeout_connection.disconnect();
  }
}

bool Dingo::StatusIcon::onMiniPlayerTimeOut() {
  if ((d_miniplayer_pos_toggle.get_active()) || (d_miniplayer_dur_toggle.get_active())) {
    if (d_playerengine->getPercentage() >= d_playerengine->getSectionEnd()) {
      d_playerengine->setPercentage(d_playerengine->getSectionStart());
    }
    
    if (d_playerengine->getPercentage() < d_playerengine->getSectionStart()) {
      d_playerengine->setPercentage(d_playerengine->getSectionStart());
    }
  }

  d_miniplayer_hscale.set_value(d_playerengine->getPercentage());
  
  Glib::ustring temp_pos_str = d_playerengine->getPositionString() + " | " + d_playerengine->getSectionStartString();
  Glib::ustring temp_dur_str = d_playerengine->getSectionEndString() + " | " + d_playerengine->getDurationString();
  
  if (d_miniplayer_pos_toggle.get_active()) {
    temp_pos_str = Dingo::Utilities::highlightString(temp_pos_str);
  }
  
  if (d_miniplayer_dur_toggle.get_active()) {
    temp_dur_str = Dingo::Utilities::highlightString(temp_dur_str);
  }
  
  d_miniplayer_pos_label.set_markup(temp_pos_str);
  d_miniplayer_dur_label.set_markup(temp_dur_str);
  
  return true;
}

bool Dingo::StatusIcon::onMiniPlayerHideTimeOut() {
  Glib::RefPtr<Gdk::Screen> screen;
  Gdk::Rectangle status_icon_area;
  Gtk::Orientation tray_orientation;
  
  int pos_x, pos_y;
  Gdk::ModifierType modifier = Gdk::MODIFIER_MASK;
  Gtk::StatusIcon::get_geometry(screen, status_icon_area, tray_orientation);
  
  int d_miniplayer_win_x = 0;
  int d_miniplayer_win_y = 0;
  int d_miniplayer_win_width = 0;
  int d_miniplayer_win_height = 0;

  d_dbmanager->getWindow()->get_screen()->get_root_window()->get_pointer(pos_x, pos_y, modifier);
  
  if (d_miniplayer_win.get_visible()) {
    d_miniplayer_win.get_window()->get_geometry(d_miniplayer_win_x, d_miniplayer_win_y, d_miniplayer_win_width, d_miniplayer_win_height);
  }
  
  if ((((pos_x <= status_icon_area.get_x()) || (pos_y <= status_icon_area.get_y()) || (pos_x >= status_icon_area.get_x() + status_icon_area.get_width()) || (pos_y >= status_icon_area.get_y() + status_icon_area.get_height())) && ((pos_x <= d_miniplayer_win_x) || (pos_x >= d_miniplayer_win_x + d_miniplayer_win_width) || (pos_y <= d_miniplayer_win_y) || (pos_y >= d_miniplayer_win_y + d_miniplayer_win_height))) || (d_is_right_clicked)) {
    d_miniplayer_win.hide();
    
    d_is_right_clicked = false;
  }
  
  return true;
}
      
void Dingo::StatusIcon::enableWidgets() {
  d_miniplayer_pos_toggle.set_sensitive(true);
  d_miniplayer_dur_toggle.set_sensitive(true);
  d_miniplayer_hscale.set_sensitive(true);
}

void Dingo::StatusIcon::disableWidgets() {
  d_miniplayer_pos_toggle.set_sensitive(false);
  d_miniplayer_dur_toggle.set_sensitive(false);
  d_miniplayer_hscale.set_sensitive(false);
}
