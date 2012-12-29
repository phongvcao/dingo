#ifndef INCLUDED_DINGO_DBMANTRACKOBSERVER_H
#define INCLUDED_DINGO_DBMANTRACKOBSERVER_H

namespace Dingo {
  class DBManTrackObserver {
    public:
      virtual void updateChangesInCoreTracks() = 0;
  };
}

#endif //INCLUDED_DINGO_DBMANTRACKOBSERVER_H
