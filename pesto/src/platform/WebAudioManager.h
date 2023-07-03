#pragma once

#include <audio/AudioManager.h>

namespace linguine::pesto {

class WebAudioManager : public AudioManager {
  public:
    ~WebAudioManager() override = default;

    void play(EffectType effectType) override {}
};

}  // namespace linguine::pesto
