#ifndef INCLUDED_DINGO_YEARTREEVIEWOBSERVER_H
#define INCLUDED_DINGO_YEARTREEVIEWOBSERVER_H

namespace Dingo {
  class YearTreeViewObserver {
    public:
      virtual void updateYearsSelected() = 0;
  };
}

#endif //INCLUDED_DINGO_YEARTREEVIEWOBSERVER_H
