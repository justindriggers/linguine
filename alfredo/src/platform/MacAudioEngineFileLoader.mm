#include "MacAudioEngineFileLoader.h"

namespace linguine::alfredo {

NSURL* MacAudioEngineFileLoader::getUrlForEffect(EffectType effectType) {
  NSString* path;

  switch (effectType) {
  case Pop:
    path = @"Balloon Pop 1.wav";
    break;
  case Select:
    path = @"Select 1.wav";
    break;
  }

  if (path) {
    return [NSURL fileURLWithPath:path
                      isDirectory:false];
  }

  return nil;
}

}  // namespace linguine::alfredo
