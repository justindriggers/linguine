#pragma once

#include <OpenALFileLoader.h>

namespace linguine::pesto {

class WebOpenALFileLoader : public audio::OpenALFileLoader {
  public:
    std::vector<std::byte> getAudioDataForEffect(EffectType effectType) const override;
};

}  // namespace linguine::pesto
