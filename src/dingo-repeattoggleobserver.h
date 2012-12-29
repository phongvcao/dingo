#ifndef INCLUDED_DINGO_REPEATTOGGLEOBSERVER_H
#define INCLUDED_DINGO_REPEATTOGGLEOBSERVER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class RepeatToggleObserver {
    public:
      virtual void updateRepeatOffToggled() = 0;
      virtual void updateRepeatSingleToggled() = 0;
      virtual void updateRepeatPlaylistToggled() = 0;
      virtual void updateRepeatSectionToggled(Glib::ustring section_button_code = "") = 0;
  };
}

#endif //INCLUDED_DINGO_REPEATTOGGLEOBSERVER_H
