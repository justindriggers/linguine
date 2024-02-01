#include "IosSaveManager.h"

#include <Foundation/Foundation.h>

namespace linguine::scampi {

IosSaveManager::IosSaveManager() : SaveManager() {
  NSDictionary *appDefaults = @{
    @"level" : @1,
    @"music" : @TRUE,
    @"sfx" : @TRUE,
    @"shake" : @TRUE,
    @"handedness" : @0,
    @"stars" : @[]
  };

  [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];

  load();
}

void IosSaveManager::load() {
  NSDictionary *values = [[NSUserDefaults standardUserDefaults] dictionaryWithValuesForKeys:@[
    @"level",
    @"music",
    @"sfx",
    @"shake",
    @"handedness",
    @"stars"
  ]];

  _level = [values[@"level"] intValue];
  _isMusicEnabled = [values[@"music"] boolValue];
  _isSoundEffectsEnabled = [values[@"sfx"] boolValue];
  _isScreenShakeEnabled = [values[@"shake"] boolValue];
  _handedness = [values[@"handedness"] intValue] == 0 ? SaveManager::Handedness::Right : SaveManager::Handedness::Left;

  NSArray *stars = values[@"stars"];
  auto index = 0;

  for (id level in stars) {
    _stars[index++] = [level unsignedCharValue];
  }
}

void IosSaveManager::save() {
  NSMutableArray *stars = [[NSMutableArray alloc] init];

  for (auto level : _stars) {
    [stars addObject:[NSNumber numberWithUnsignedChar:level]];
  }

  NSDictionary *values = @{
    @"level" : [NSNumber numberWithInt:getCurrentLevel()],
    @"music" : [NSNumber numberWithBool:isMusicEnabled()],
    @"sfx" : [NSNumber numberWithBool:isSoundEffectsEnabled()],
    @"shake" : [NSNumber numberWithBool:isScreenShakeEnabled()],
    @"handedness" : [NSNumber numberWithInt:getHandedness() == SaveManager::Handedness::Right ? 0 : 1],
    @"stars" : stars
  };

  [[NSUserDefaults standardUserDefaults] setValuesForKeysWithDictionary:values];
}

}  // namespace linguine::scampi
