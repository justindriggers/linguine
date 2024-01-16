#pragma once

#include <AudioEngineFileLoader.h>

#import <Foundation/NSURL.h>

namespace linguine::alfredo {

class MacAudioEngineFileLoader : public audio::AudioEngineFileLoader {
  public:
    NSURL* getUrlForEffect(EffectType effectType) override;

    NSURL* getUrlForSong(SongType songType) override;
};

}  // namespace linguine::alfredo
