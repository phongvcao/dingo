#ifndef INCLUDED_DINGO_DBMANYEAROBSERVER_H
#define INCLUDED_DINGO_DBMANYEAROBSERVER_H

namespace Dingo {
  class DBManYearObserver {
    public:
      virtual void updateChangesInCoreYears() = 0;
  };
}

#endif //INCLUDED_DINGO_DBMANYEAROBSERVER_H
