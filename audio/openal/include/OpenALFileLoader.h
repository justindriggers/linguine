#pragma once

#include <audio/EffectType.h>

#include <vector>

namespace linguine::audio {

class OpenALFileLoader {
  public:
    virtual ~OpenALFileLoader() = default;

    virtual std::vector<std::byte> getAudioDataForEffect(EffectType effectType) const = 0;
};

}  // namespace linguine::audio
