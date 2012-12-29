#ifndef INCLUDED_DINGO_MOUSEMOTIONOBSERVER_H
#define INCLUDED_DINGO_MOUSEMOTIONOBSERVER_H

namespace Dingo {
  class MouseMotionObserver {
    public:
      virtual void updateMouseHoverOnFullscreen() = 0;
  };
}

#endif //INCLUDED_DINGO_MOUSEMOTIONOBSERVER_H
