#include "IosAudioEngineFileLoader.h"

namespace linguine::scampi {

NSURL* IosAudioEngineFileLoader::getUrlForEffect(EffectType effectType) {
  NSString* name;
  NSString* extension;

  switch (effectType) {
    case Pop: {
      name = @"Balloon Pop 1";
      extension = @"wav";
      break;
    }
    case Select: {
      name = @"Select 1";
      extension = @"wav";
    }
  }

  if (name && extension) {
    return [[NSBundle mainBundle] URLForResource:name
                                   withExtension:extension];
  }

  return nil;
}

}  // namespace linguine::alfredo
