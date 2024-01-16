#include "AudioEngineFileLoader.h"

namespace linguine::audio {

AVAudioFile* AudioEngineFileLoader::getAudioFileForEffect(EffectType effectType) {
  auto existing = _loadedEffects.find(effectType);

  if (existing != _loadedEffects.end()) {
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

  _loadedEffects.insert({effectType, file});

  return file;
}

AVAudioFile* AudioEngineFileLoader::getAudioFileForSong(SongType songType) {
  auto existing = _loadedSongs.find(songType);

  if (existing != _loadedSongs.end()) {
    return existing->second;
  }

  auto url = getUrlForSong(songType);

  NSError* error;
  auto file = [[AVAudioFile alloc] initForReading:url
                                            error:&error];

  if (error) {
    NSLog(@"%@", [error localizedDescription]);
    return nil;
  }

  _loadedSongs.insert({songType, file});

  return file;
}

}  // namespace linguine::audio
