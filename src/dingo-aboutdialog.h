/* 
 * dingo-aboutdialog.h
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

#ifndef INCLUDED_DINGO_ABOUTDIALOG_H
#define INCLUDED_DINGO_ABOUTDIALOG_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class AboutDialog : public Gtk::AboutDialog {
    public:
      AboutDialog();
      virtual ~AboutDialog();
      
    private:
      //CREATORS
      void createAboutDialog();
    
      //DATA MEMBERS
      std::vector<Glib::ustring> d_authors_vector;
      std::vector<Glib::ustring> d_artists_vector;
      Glib::ustring d_translators_str;
      Glib::RefPtr<Gdk::Pixbuf> d_dingo_logo;
  };
}

#endif //INCLUDED_DINGO_ABOUTDIALOG_H
