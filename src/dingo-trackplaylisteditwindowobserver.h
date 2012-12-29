#ifndef INCLUDED_DINGO_TRACKPLAYLISTEDITWINDOWOBSERVER_H
#define INCLUDED_DINGO_TRACKPLAYLISTEDITWINDOWOBSERVER_H

namespace Dingo {
  class TrackPlaylistEditWindowObserver {
    public:
      virtual void updateTrackPlaylistEditInfoSaved() = 0;
  };
}

#endif //INCLUDED_DINGO_TRACKPLAYLISTEDITWINDOWOBSERVER_H
