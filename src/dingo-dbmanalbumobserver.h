#ifndef INCLUDED_DINGO_DBMANALBUMOBSERVER_H
#define INCLUDED_DINGO_DBMANALBUMOBSERVER_H

namespace Dingo {
  class DBManAlbumObserver {
    public:
      virtual void updateChangesInCoreAlbums() = 0;
  };
}

#endif //INCLUDED_DINGO_DBMANALBUMOBSERVER_H
