#ifndef INCLUDED_DINGO_PROCESSGRIDOBSERVER_H
#define INCLUDED_DINGO_PROCESSGRIDOBSERVER_H

#include "dingo-header.h"

namespace Dingo {
  class ProcessGridObserver {
    public:
      virtual void updateProcessFinished(ProcessID process_id) = 0;
  };
}

#endif //INCLUDED_DINGO_PROCESSGRIDOBSERVER_H
