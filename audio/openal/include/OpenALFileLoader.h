#pragma once

#include <audio/EffectType.h>
#include <audio/SongType.h>

#include <vector>

namespace linguine::audio {

class OpenALFileLoader {
  public:
    virtual ~OpenALFileLoader() = default;

    [[nodiscard]] float getSongLoopPoint(SongType songType) const {
      return _songLoopPoints.at(songType);
    }

    [[nodiscard]] virtual std::vector<std::byte> getAudioDataForEffect(EffectType effectType) const = 0;

    [[nodiscard]] virtual std::vector<std::byte> getAudioDataForSong(SongType songType) const = 0;

  private:
    const std::unordered_map<SongType, float> _songLoopPoints = {
        { SongType::Title, 24.0f },
        { SongType::Theme, 60.0f },
        { SongType::GameOver, 12.0f }
    };
};

}  // namespace linguine::audio
