#pragma once

#include "EffectType.h"

namespace linguine {

class AudioManager {
  public:
    virtual ~AudioManager() = default;

    virtual void play(EffectType effectType) = 0;
};

}  // namespace linguine
