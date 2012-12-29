#ifndef INCLUDED_DINGO_DBMANARTISTOBSERVER_H
#define INCLUDED_DINGO_DBMANARTISTOBSERVER_H

namespace Dingo {
  class DBManArtistObserver {
    public:
      virtual void updateChangesInCoreArtists() = 0;
  };
}

#endif //INCLUDED_DINGO_DBMANARTISTOBSERVER_H
