#ifndef INCLUDED_DINGO_GENRETREEVIEWOBSERVER_H
#define INCLUDED_DINGO_GENRETREEVIEWOBSERVER_H

namespace Dingo {
  class GenreTreeViewObserver {
    public:
      virtual void updateGenresSelected() = 0;
  };
}

#endif //INCLUDED_DINGO_GENRETREEVIEWOBSERVER_H
