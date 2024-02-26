#pragma once

#include <audio/EffectType.h>
#include <audio/SongType.h>

#include <unordered_map>
#include <vector>

namespace linguine::audio {

class AAudioFileLoader {
  public:
    virtual ~AAudioFileLoader() = default;

    [[nodiscard]] int32_t getSongLoopPoint(SongType songType) const {
      return _songLoopPoints.at(songType);
    }

    [[nodiscard]] virtual std::vector<std::byte> getAudioDataForEffect(EffectType effectType) const = 0;

    [[nodiscard]] virtual std::vector<std::byte> getAudioDataForSong(SongType songType) const = 0;

  private:
    const std::unordered_map<SongType, int32_t> _songLoopPoints = {
        { SongType::Title, 1058400 },
        { SongType::Theme, 2646000 },
        { SongType::GameOver, 529200 }
    };
};

}  // namespace linguine::audio
