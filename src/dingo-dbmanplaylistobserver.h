#ifndef INCLUDED_DINGO_DBMANPLAYLISTOBSERVER_H
#define INCLUDED_DINGO_DBMANPLAYLISTOBSERVER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class DBManPlaylistObserver {
    public:
      virtual void updateChangesInCorePlaylists() = 0;
  };
}

#endif //INCLUDED_DINGO_DBMANPLAYLISTOBSERVER_H
