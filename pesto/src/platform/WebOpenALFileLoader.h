#pragma once

#include <OpenALFileLoader.h>

namespace linguine::pesto {

class WebOpenALFileLoader : public audio::OpenALFileLoader {
  public:
    [[nodiscard]] std::vector<std::byte> getAudioDataForEffect(EffectType effectType) const override;

    [[nodiscard]] std::vector<std::byte> getAudioDataForSong(SongType songType) const override;
};

}  // namespace linguine::pesto
