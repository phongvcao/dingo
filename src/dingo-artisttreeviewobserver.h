#ifndef INCLUDED_DINGO_ARTISTTREEVIEWOBSERVER_H
#define INCLUDED_DINGO_ARTISTTREEVIEWOBSERVER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class ArtistTreeViewObserver {
    public:
      virtual void updateArtistsSelected() = 0;
  };
}

#endif //INCLUDED_DINGO_ARTISTTREEVIEWOBSERVER_H