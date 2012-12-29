#ifndef INCLUDED_DINGO_SEARCHENTRYOBSERVER_H
#define INCLUDED_DINGO_SEARCHENTRYOBSERVER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class SearchEntryObserver {
    public:
      virtual void updateSearchEntryChanged() = 0;
  };
}

#endif //INCLUDED_DINGO_SEARCHENTRYOBSERVER_H
