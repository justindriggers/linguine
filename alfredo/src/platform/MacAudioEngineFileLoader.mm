#include "MacAudioEngineFileLoader.h"

namespace linguine::alfredo {

NSURL* MacAudioEngineFileLoader::getUrlForEffect(EffectType effectType) {
  NSString* path;

  switch (effectType) {
  case EffectType::Pop:
    path = @"Balloon Pop 1.wav";
    break;
  case EffectType::Select:
    path = @"Select 1.wav";
    break;
  }

  if (path) {
    return [NSURL fileURLWithPath:path
                      isDirectory:false];
  }

  return nil;
}

NSURL* MacAudioEngineFileLoader::getUrlForSong(SongType songType) {
  NSString* path;

  switch (songType) {
  case SongType::Theme:
    path = @"aegis4-16bit-44_1khz.wav";
    break;
  case SongType::Title:
    path = @"aegis4_title-16bit-44_1khz.wav";
    break;
  case SongType::GameOver:
    path = @"aegis4_gameover-16bit-44_1khz.wav";
    break;
  }

  if (path) {
    return [NSURL fileURLWithPath:path
                      isDirectory:false];
  }

  return nil;
}

}  // namespace linguine::alfredo
