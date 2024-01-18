#include "IosAudioEngineFileLoader.h"

namespace linguine::scampi {

NSURL* IosAudioEngineFileLoader::getUrlForEffect(EffectType effectType) {
  NSString* name;
  NSString* extension;

  switch (effectType) {
    case EffectType::ButtonDown: {
      name = @"select-16bit-44_1khz_1";
      extension = @"wav";
      break;
    }
    case EffectType::ButtonUp: {
      name = @"select-16bit-44_1khz_2";
      extension = @"wav";
      break;
    }
    case EffectType::Collect1: {
      name = @"collect1-16bit-44_1khz";
      extension = @"wav";
      break;
    }
    case EffectType::Collect2: {
      name = @"collect2-16bit-44_1khz";
      extension = @"wav";
      break;
    }
    case EffectType::Collect3: {
      name = @"collect3-16bit-44_1khz";
      extension = @"wav";
      break;
    }
    case EffectType::Collect4: {
      name = @"collect4-16bit-44_1khz";
      extension = @"wav";
      break;
    }
    case EffectType::Collect5: {
      name = @"collect5-16bit-44_1khz";
      extension = @"wav";
      break;
    }
    case EffectType::Detonate: {
      name = @"detonation-16bit-44_1khz";
      extension = @"wav";
      break;
    }
    case EffectType::Explosion: {
      name = @"explosion-16bit-44_1khz";
      extension = @"wav";
      break;
    }
    case EffectType::Heal: {
      name = @"heal-16bit-44_1khz";
      extension = @"wav";
      break;
    }
    case EffectType::Level: {
      name = @"level-16bit-44_1khz";
      extension = @"wav";
      break;
    }
    case EffectType::PowerUp: {
      name = @"powerup-16bit-44_1khz";
      extension = @"wav";
      break;
    }
    case EffectType::Swoop: {
      name = @"swoop-16bit-44_1khz";
      extension = @"wav";
      break;
    }
    case EffectType::Xp: {
      name = @"xp-16bit-44_1khz";
      extension = @"wav";
      break;
    }
  }

  if (name && extension) {
    return [[NSBundle mainBundle] URLForResource:name
                                   withExtension:extension];
  }

  return nil;
}

NSURL* IosAudioEngineFileLoader::getUrlForSong(SongType songType) {
  NSString* name;
  NSString* extension;

  switch (songType) {
    case SongType::Theme: {
      name = @"aegis4-16bit-44_1khz";
      extension = @"wav";
      break;
    }
    case SongType::Title: {
      name = @"aegis4_title-16bit-44_1khz";
      extension = @"wav";
      break;
    }
    case SongType::GameOver: {
      name = @"aegis4_gameover-16bit-44_1khz";
      extension = @"wav";
      break;
    }
  }

  if (name && extension) {
    return [[NSBundle mainBundle] URLForResource:name
                                   withExtension:extension];
  }

  return nil;
}

}  // namespace linguine::alfredo
