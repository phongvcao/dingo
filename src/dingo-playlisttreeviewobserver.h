#ifndef INCLUDED_DINGO_PLAYLISTTREEVIEWOBSERVER_H
#define INCLUDED_DINGO_PLAYLISTTREEVIEWOBSERVER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class PlaylistTreeViewObserver {
    public:
      virtual void updateBlockSelection() = 0;
      virtual void updateRestoreSelectedRows() = 0;
      virtual void updateUnblockSelection() = 0;
  };
}

#endif //INCLUDED_DINGO_PLAYLISTTREEVIEWOBSERVER_H
