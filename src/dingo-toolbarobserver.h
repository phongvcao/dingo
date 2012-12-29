#ifndef INCLUDED_DINGO_TOOLBAROBSERVER_H
#define INCLUDED_DINGO_TOOLBAROBSERVER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class ToolBarObserver {
    public:
      virtual void updateHomeToggleActive() = 0;
      virtual void updateNowPlayingToggleActive() = 0;
      virtual void updatePlayQueueToggleActive() = 0;
  };
}

#endif //INCLUDED_DINGO_TOOLBAROBSERVER_H
