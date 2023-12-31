#include "IosSaveManager.h"

#include <Foundation/Foundation.h>

namespace linguine::scampi {

IosSaveManager::IosSaveManager() : SaveManager() {
  NSDictionary *appDefaults = @{
    @"points" : @0
  };

  [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];

  load();
}

void IosSaveManager::load() {
  NSDictionary *values = [[NSUserDefaults standardUserDefaults] dictionaryWithValuesForKeys:@[
    @"points"
  ]];

  _points = [values[@"points"] intValue];
}

void IosSaveManager::save() {
  NSDictionary *values = @{
    @"points" : [NSNumber numberWithInt:getPoints()]
  };

  [[NSUserDefaults standardUserDefaults] setValuesForKeysWithDictionary:values];
}

}  // namespace linguine::scampi
