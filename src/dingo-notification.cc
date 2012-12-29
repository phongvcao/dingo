/* 
 * dingo-notification.cc
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

#include "dingo-notification.h"
#include "dingo-dbmanager.h"
#include "dingo-playerengine.h"
#include "dingo-header.h"

#ifndef INCLUDED_GLIBMM_I18N_H
#define INCLUDED_GLIBMM_I18N_H
#include <glibmm/i18n.h>
#endif //INCLUDED_GLIBMM_I18N_H

//CONSTRUCTORS - DESTRUCTOR
Dingo::Notification::Notification(Dingo::PlayerEngine* playerengine) {
  d_playerengine = playerengine;
  
  d_notification = notify_notification_new(Glib::ustring("").c_str(), Glib::ustring("").c_str(), Glib::ustring("").c_str());
  
  notify_notification_set_timeout(d_notification, NOTIFY_EXPIRES_DEFAULT);
  
  notify_notification_set_urgency(d_notification, NOTIFY_URGENCY_CRITICAL);
}

Dingo::Notification::~Notification() {
  notify_uninit();
}

//INHERITED OBSERVER'S METHODS
void Dingo::Notification::updateTrackTreeRowActivated(Gtk::TreeRowReference cur_track_row_ref) {
  Gtk::TreeModel::Row temp_cur_row = *(Dingo::DBManager::trackModel->get_iter(cur_track_row_ref.get_path()));
  
  Glib::ustring title = temp_cur_row[(*Dingo::DBManager::trackCR).trackTitle];
  Glib::ustring artist = temp_cur_row[(*Dingo::DBManager::trackCR).trackArtist];
  Glib::ustring album = temp_cur_row[(*Dingo::DBManager::trackCR).trackAlbum];
  
  notify_notification_update(d_notification, title.c_str(), Glib::ustring::compose(_("by %1 from %2"), artist, album).c_str(), Glib::filename_from_uri(d_playerengine->getPreviewImageURI()).c_str());
  
  notify_notification_show(d_notification, NULL);
}

void Dingo::Notification::updateTrackTreeRowDeactivated() {
  notify_notification_update(d_notification, Glib::ustring(_("Not Playing")).c_str(), Glib::ustring("").c_str(), Glib::filename_from_uri(d_playerengine->getPreviewImageURI()).c_str());
}
