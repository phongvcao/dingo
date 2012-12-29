#ifndef INCLUDED_DINGO_ALBUMTREEVIEWOBSERVER_H
#define INCLUDED_DINGO_ALBUMTREEVIEWOBSERVER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class AlbumTreeViewObserver {
    public:
      virtual void updateAlbumsSelected() = 0;
  };
}

#endif //INCLUDED_DINGO_ALBUMTREEVIEWOBSERVER_H
