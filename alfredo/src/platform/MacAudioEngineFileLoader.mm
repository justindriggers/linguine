#include "MacAudioEngineFileLoader.h"

namespace linguine::alfredo {

NSURL* MacAudioEngineFileLoader::getUrlForEffect(EffectType effectType) {
  NSString* path;

  switch (effectType) {
  case EffectType::ButtonDown:
    path = @"select-16bit-44_1khz_1.wav";
    break;
  case EffectType::ButtonUp:
    path = @"select-16bit-44_1khz_2.wav";
    break;
  case EffectType::Collect1:
    path = @"collect1-16bit-44_1khz.wav";
    break;
  case EffectType::Collect2:
    path = @"collect2-16bit-44_1khz.wav";
    break;
  case EffectType::Collect3:
    path = @"collect3-16bit-44_1khz.wav";
    break;
  case EffectType::Collect4:
    path = @"collect4-16bit-44_1khz.wav";
    break;
  case EffectType::Collect5:
    path = @"collect5-16bit-44_1khz.wav";
    break;
  case EffectType::Detonate:
    path = @"detonation-16bit-44_1khz.wav";
    break;
  case EffectType::Explosion:
    path = @"explosion-16bit-44_1khz.wav";
    break;
  case EffectType::Heal:
    path = @"heal-16bit-44_1khz.wav";
    break;
  case EffectType::Level:
    path = @"level-16bit-44_1khz.wav";
    break;
  case EffectType::PowerUp:
    path = @"powerup-16bit-44_1khz.wav";
    break;
  case EffectType::Swoop:
    path = @"swoop-16bit-44_1khz.wav";
    break;
  case EffectType::Xp:
    path = @"xp-16bit-44_1khz.wav";
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
