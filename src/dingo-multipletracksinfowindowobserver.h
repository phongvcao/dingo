#ifndef INCLUDED_DINGO_MULTIPLETRACKSINFOWINDOWOBSERVER_H
#define INCLUDED_DINGO_MULTIPLETRACKSINFOWINDOWOBSERVER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class MultipleTracksInfoWindowObserver {
    public:
      virtual void updateMultipleTracksInfoWindowSaveClicked(std::vector<Glib::ustring> changed_values_vector);
  };
}

#endif //INCLUDED_DINGO_MULTIPLETRACKSINFOWINDOWOBSERVER_H
