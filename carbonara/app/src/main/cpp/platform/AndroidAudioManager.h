#pragma once

#include <audio/AudioManager.h>

namespace linguine::carbonara {

class AndroidAudioManager : public AudioManager {
  public:
    ~AndroidAudioManager() override = default;

    void poll() override {}

    void setMusicEnabled(bool enabled) override {}

    void setSoundEffectsEnabled(bool enabled) override {}

    void play(EffectType effectType) override {}

    void play(SongType songType, Mode mode) override {}

    [[nodiscard]] std::optional<SongType> getCurrentSongType() const override {
      return {};
    }

    void stopSongs() override {}

    void pause() override {}

    void resume() override {}
};

}  // namespace linguine::carbonara
