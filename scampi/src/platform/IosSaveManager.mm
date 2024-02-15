#include "IosSaveManager.h"

#include <Foundation/Foundation.h>

namespace linguine::scampi {

IosSaveManager::IosSaveManager() : SaveManager() {
  NSDictionary *appDefaults = @{
    @"points" : @0,
    @"lives" : @0,
    @"music" : @TRUE,
    @"sfx" : @TRUE,
    @"shake" : @TRUE,
    @"handedness" : @0,
    @"new" : @TRUE
  };

  [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];

  load();
}

void IosSaveManager::load() {
  NSDictionary *values = [[NSUserDefaults standardUserDefaults] dictionaryWithValuesForKeys:@[
    @"points",
    @"lives",
    @"music",
    @"sfx",
    @"shake",
    @"handedness",
    @"new"
  ]];

  _points = [values[@"points"] intValue];
  _lives = [values[@"lives"] unsignedCharValue];
  _isMusicEnabled = [values[@"music"] boolValue];
  _isSoundEffectsEnabled = [values[@"sfx"] boolValue];
  _isScreenShakeEnabled = [values[@"shake"] boolValue];
  _handedness = [values[@"handedness"] intValue] == 0 ? SaveManager::Handedness::Right : SaveManager::Handedness::Left;
  _isNewPlayer = [values[@"new"] boolValue];
}

void IosSaveManager::save() {
  NSDictionary *values = @{
    @"points" : [NSNumber numberWithInt:getPoints()],
    @"lives" : [NSNumber numberWithUnsignedChar:getLives()],
    @"music" : [NSNumber numberWithBool:isMusicEnabled()],
    @"sfx" : [NSNumber numberWithBool:isSoundEffectsEnabled()],
    @"shake" : [NSNumber numberWithBool:isScreenShakeEnabled()],
    @"handedness" : [NSNumber numberWithInt:getHandedness() == SaveManager::Handedness::Right ? 0 : 1],
    @"new" : [NSNumber numberWithBool:isNewPlayer()]
  };

  [[NSUserDefaults standardUserDefaults] setValuesForKeysWithDictionary:values];
}

}  // namespace linguine::scampi
