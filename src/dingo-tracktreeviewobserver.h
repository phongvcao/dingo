#ifndef INCLUDED_DINGO_TRACKTREEVIEWOBSERVER_H
#define INCLUDED_DINGO_TRACKTREEVIEWOBSERVER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class TrackTreeViewObserver {
    public:
      virtual void updateTrackTreeRowActivated(Gtk::TreeRowReference activated_row_ref) = 0;
      virtual void updateTrackTreeRowDeactivated() = 0;
  };
}

#endif //INCLUDED_DINGO_TRACKTREEVIEWOBSERVER_H
