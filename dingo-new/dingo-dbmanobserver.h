#ifndef INCLUDED_DINGO_DBMANOBSERVER_H
#define INCLUDED_DINGO_DBMANOBSERVER_H

namespace Dingo {
  class DBManObserver {
    public:
      virtual void updateNormalPlaylistCreated() = 0;
      virtual void updateSmartPlaylistCreated() = 0;
      virtual void updateNormalPlaylistDeleted() = 0;
      virtual void updateSmartPlaylistDeleted() = 0;
  };
}

#endif //INCLUDED_DINGO_DBMANOBSERVER_H
