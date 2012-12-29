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
