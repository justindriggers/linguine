#include "IosAudioEngineFileLoader.h"

namespace linguine::scampi {

NSURL* IosAudioEngineFileLoader::getUrlForEffect(EffectType effectType) {
  NSString* name;
  NSString* extension;

  switch (effectType) {
    case EffectType::Pop: {
      name = @"Balloon Pop 1";
      extension = @"wav";
      break;
    }
    case EffectType::Select: {
      name = @"Select 1";
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
