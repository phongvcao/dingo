#ifndef INCLUDED_DINGO_ENGINENAVIGATIONOBSERVER_H
#define INCLUDED_DINGO_ENGINENAVIGATIONOBSERVER_H

namespace Dingo {
  class EngineNavigationObserver {
    public:
      virtual void updateEngineTrackNext() = 0;
      virtual void updateEngineTrackPrevious() = 0;
      virtual void updateEngineTrackCurrent() = 0;
  };
}

#endif //INCLUDED_DINGO_ENGINENAVIGATIONOBSERVER_H
