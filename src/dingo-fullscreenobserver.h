#ifndef INCLUDED_DINGO_FULLSCREENOBSERVER_H
#define INCLUDED_DINGO_FULLSCREENOBSERVER_H

namespace Dingo {
  class FullscreenObserver {
    public:
      virtual void updateFullscreenActivated() = 0;
      virtual void updateUnFullscreenActivated() = 0;
  };
}

#endif //INCLUDED_DINGO_FULLSCREENOBSERVER_H
