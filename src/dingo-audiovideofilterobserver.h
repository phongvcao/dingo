#ifndef INCLUDED_DINGO_AUDIOVIDEOFILTEROBSERVER_H
#define INCLUDED_DINGO_AUDIOVIDEOFILTEROBSERVER_H

namespace Dingo {
  class AudioVideoFilterObserver {
    public:
      virtual void updateAudioFilterToggled() = 0;
      virtual void updateVideoFilterToggled() = 0;
  };
}

#endif //INCLUDED_DINGO_AUDIOVIDEOFILTEROBSERVER_H
