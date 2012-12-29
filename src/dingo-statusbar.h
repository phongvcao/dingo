#ifndef INCLUDED_DINGO_STATUSBAR_H
#define INCLUDED_DINGO_STATUSBAR_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class StatusBar : public Gtk::Statusbar {
    public:
      StatusBar();
      virtual ~StatusBar();
  };
}

#endif //INCLUDED_DINGO_STATUSBAR_H
