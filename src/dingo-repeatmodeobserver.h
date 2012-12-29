#ifndef INCLUDED_DINGO_REPEATMODEOBSERVER_H
#define INCLUDED_DINGO_REPEATMODEOBSERVER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class RepeatModeObserver {
    public:
      virtual void updateRepeatOff() = 0;
      virtual void updateRepeatSingle() = 0;
      virtual void updateRepeatPlaylist() = 0;
      virtual void updateRepeatSection() = 0;
      virtual void updateRepeatSectionOff() = 0;
  };
}

#endif //INCLUDED_DINGO_REPEATMODEOBSERVER_H
