#pragma once

namespace linguine {

class AudioManager {
  public:
    virtual ~AudioManager() = default;

    virtual void play() = 0;
};

}  // namespace linguine
