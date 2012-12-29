#ifndef INCLUDED_DINGO_PLAYORDEROBSERVER_H
#define INCLUDED_DINGO_PLAYORDEROBSERVER_H

namespace Dingo {
  class PlayOrderObserver {
    public:
      virtual void updatePlayOrderNormal() = 0;
      virtual void updatePlayOrderShuffle() = 0;
  };
}

#endif //INCLUDED_DINGO_PLAYORDEROBSERVER_H
