#ifndef INCLUDED_DINGO_TRACKINFOWINDOWOBSERVER_H
#define INCLUDED_DINGO_TRACKINFOWINDOWOBSERVER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class TrackInfoWindowObserver {
    public:
      virtual void updateGetNextTrackInfo() = 0;
      virtual void updateGetPreviousTrackInfo() = 0;
  };
}

#endif //INCLUDED_DINGO_TRACKINFOWINDOWOBSERVER_H
