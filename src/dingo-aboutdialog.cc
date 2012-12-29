/* 
 * dingo-aboutdialog.cc
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

#include "dingo-aboutdialog.h"

#ifndef INCLUDED_GLIBMM_I18N_H
#define INCLUDED_GLIBMM_I18N_H
#include <glibmm/i18n.h>
#endif //INCLUDED_GLIBMM_I18N_H

//CONSTRUCTORS - DESTRUCTOR
Dingo::AboutDialog::AboutDialog() :
  Gtk::AboutDialog()
{
  createAboutDialog();
  
  set_title("");
  set_size_request(460, 340);
}

Dingo::AboutDialog::~AboutDialog() {

}

//CREATORS
void Dingo::AboutDialog::createAboutDialog() {
  d_dingo_logo = Gdk::Pixbuf::create_from_file(DATADIR"/dingo/icons/128x128/dingo.png");

  set_logo(d_dingo_logo);

  set_program_name("Dingo");
  
  set_version("0.5.0");
  
  set_copyright(_("Copyright \xc2\xa9 2011-2013 Phong V. Cao"));
  
  set_comments(_("A minimalist yet powerful media manager for Linux"));
  
  set_website("http://dingo.phongvcao.com/");
  
  set_website_label(_("Dingo Website"));
  
  d_authors_vector.push_back(_("Maintainers:"));
  d_authors_vector.push_back(_("Phong V. Cao"));
  d_authors_vector.push_back(_("E-mail: phongvcao@phongvcao.com"));
  set_authors(d_authors_vector);
  
  d_artists_vector.push_back(_("Custom Icon Design Team<support@customicondesign.com>"));
  d_artists_vector.push_back(_("Andreas Nilsson"));
  d_artists_vector.push_back(_("Garrett LeSage"));
  d_artists_vector.push_back(_("Jakub Steiner"));
  d_artists_vector.push_back(_("K. Vishnoo Charan Reddy"));
  d_artists_vector.push_back(_("Michael Monreal"));
  d_artists_vector.push_back(_("Ryan Collier"));
  d_artists_vector.push_back(_("Rhythmbox Icon Design Team"));
  set_artists(d_artists_vector);
  
  d_translators_str = Glib::ustring(_("English:")) + "\n";
  d_translators_str += Glib::ustring(_("Phong V. Cao<phongvcao@phongvcao.com>")) + "\n\n";
  d_translators_str += Glib::ustring(_("Vietnamese:")) + "\n";
  d_translators_str += Glib::ustring(_("Phong V. Cao<phongvcao@phongvcao.com>")) + "\n";
  d_translators_str += Glib::ustring(_("Khanh D. Cao<khanhcao96@gmail.com>")) + "\n";
  d_translators_str += _("Khoa D. Cao<khoacao96@gmail.com>");
  set_translator_credits(d_translators_str);
  
  set_license(_("Copyright \xc2\xa9 2011-2013 Phong V. Cao\n\nDingo is free software; you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation; either version 3 of the License, or\n(at your option) any later version.\n\nDingo is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with Dingo; if not, write to the Free Software Foundation, Inc.,\n51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA."));
}
