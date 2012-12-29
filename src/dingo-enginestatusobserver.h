#ifndef INCLUDED_DINGO_ENGINESTATUSOBSERVER_H
#define INCLUDED_DINGO_ENGINESTATUSOBSERVER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class EngineStatusObserver {
    public:
      virtual void updateEnginePlaying() = 0;
      virtual void updateEnginePaused() = 0;
      virtual void updateEngineStopped() = 0;
      virtual void updateEngineNewPlaying() = 0;
  };
}

#endif //INCLUDED_DINGO_ENGINESTATUSOBSERVER_H
