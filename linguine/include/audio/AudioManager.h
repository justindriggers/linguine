#pragma once

#include <optional>

#include "EffectType.h"
#include "Mode.h"
#include "SongType.h"

namespace linguine {

class AudioManager {
  public:
    virtual ~AudioManager() = default;

    virtual void poll() = 0;

    virtual void setMusicEnabled(bool enabled) = 0;

    virtual void setSoundEffectsEnabled(bool enabled) = 0;

    virtual void play(EffectType effectType) = 0;

    virtual void play(SongType songType, Mode mode) = 0;

    [[nodiscard]] virtual std::optional<SongType> getCurrentSongType() const = 0;

    virtual void stopSongs() = 0;

    virtual void pause() = 0;

    virtual void resume() = 0;
};

}  // namespace linguine
