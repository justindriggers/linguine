#pragma once

#include <unordered_map>

#import <AVFoundation/AVAudioFile.h>

#include <audio/EffectType.h>

namespace linguine::audio {

class AudioEngineFileLoader {
public:
  virtual ~AudioEngineFileLoader() = default;

  AVAudioFile* getAudioFileForEffect(EffectType effectType);

  virtual NSURL* getUrlForEffect(EffectType effectType) = 0;

private:
  std::unordered_map<EffectType, AVAudioFile*> _loadedFiles;
};

}  // namespace linguine::audio
