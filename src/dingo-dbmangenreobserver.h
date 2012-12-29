#ifndef INCLUDED_DINGO_DBMANGENREOBSERVER_H
#define INCLUDED_DINGO_DBMANGENREOBSERVER_H

namespace Dingo {
  class DBManGenreObserver {
    public:
      virtual void updateChangesInCoreGenres() = 0;
  };
}

#endif //INCLUDED_DINGO_DBMANGENREOBSERVER_H
