#include "IosSaveManager.h"

#include <Foundation/Foundation.h>

namespace linguine::scampi {

IosSaveManager::IosSaveManager() : SaveManager() {
  NSDictionary *appDefaults = @{
    @"points" : @0,
    @"music" : @TRUE,
    @"sfx" : @TRUE,
    @"shake" : @TRUE,
    @"handedness" : @0
  };

  [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];

  load();
}

void IosSaveManager::load() {
  NSDictionary *values = [[NSUserDefaults standardUserDefaults] dictionaryWithValuesForKeys:@[
    @"points",
    @"music",
    @"sfx",
    @"shake"
  ]];

  _points = [values[@"points"] intValue];
  _isMusicEnabled = [values[@"music"] boolValue];
  _isSoundEffectsEnabled = [values[@"sfx"] boolValue];
  _isScreenShakeEnabled = [values[@"shake"] boolValue];
  _handedness = [values[@"handedness"] intValue] == 0 ? SaveManager::Handedness::Right : SaveManager::Handedness::Left;
}

void IosSaveManager::save() {
  NSDictionary *values = @{
    @"points" : [NSNumber numberWithInt:getPoints()],
    @"music" : [NSNumber numberWithBool:isMusicEnabled()],
    @"sfx" : [NSNumber numberWithBool:isSoundEffectsEnabled()],
    @"shake" : [NSNumber numberWithBool:isScreenShakeEnabled()],
    @"handedness" : [NSNumber numberWithInt:getHandedness() == SaveManager::Handedness::Right ? 0 : 1]
  };

  [[NSUserDefaults standardUserDefaults] setValuesForKeysWithDictionary:values];
}

}  // namespace linguine::scampi
