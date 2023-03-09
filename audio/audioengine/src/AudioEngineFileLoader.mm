#include "AudioEngineFileLoader.h"

namespace linguine::audio {

AVAudioFile* AudioEngineFileLoader::getAudioFileForEffect(EffectType effectType) {
  auto existing = _loadedFiles.find(effectType);

  if (existing != _loadedFiles.end()) {
    return existing->second;
  }

  auto url = getUrlForEffect(effectType);

  NSError* error;
  auto file = [[AVAudioFile alloc] initForReading:url
                                            error:&error];

  if (error) {
    NSLog(@"%@", [error localizedDescription]);
    return nil;
  }

  _loadedFiles.insert({effectType, file});

  return file;
}

}  // namespace linguine::audio
