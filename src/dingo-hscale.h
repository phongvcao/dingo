#ifndef INCLUDED_DINGO_HSCALE_H
#define INCLUDED_DINGO_HSCALE_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class HScale : public Gtk::HScale {
    public:
      HScale();
      virtual ~HScale();
    
    private:
      //SIGNAL HANDLERS
      bool on_button_press_event(GdkEventButton* event);
      bool on_button_release_event(GdkEventButton* event);
  };
}

#endif //INCLUDED_DINGO_HSCALE_H
