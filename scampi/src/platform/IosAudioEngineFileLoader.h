#pragma once

#include <AudioEngineFileLoader.h>

#import <Foundation/NSURL.h>

namespace linguine::scampi {

class IosAudioEngineFileLoader : public linguine::audio::AudioEngineFileLoader {
  public:
    NSURL* getUrlForEffect(EffectType effectType) override;
};

}  // namespace linguine::alfredo
