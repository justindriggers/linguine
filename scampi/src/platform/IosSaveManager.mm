#include "IosSaveManager.h"

#include <Foundation/Foundation.h>

namespace linguine::scampi {

IosSaveManager::IosSaveManager() : SaveManager() {
  NSDictionary *appDefaults = @{
    @"points" : @0,
    @"upgrade_0" : @0U,
    @"upgrade_1" : @0U,
    @"upgrade_2" : @0U,
    @"upgrade_3" : @0U
  };

  [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];

  load();
}

void IosSaveManager::load() {
  NSDictionary *values = [[NSUserDefaults standardUserDefaults] dictionaryWithValuesForKeys:@[
    @"points",
    @"upgrade_0",
    @"upgrade_1",
    @"upgrade_2",
    @"upgrade_3"
  ]];

  _points = [values[@"points"] intValue];
  _upgradeRanks[0] = [values[@"upgrade_0"] unsignedIntValue];
  _upgradeRanks[1] = [values[@"upgrade_1"] unsignedIntValue];
  _upgradeRanks[2] = [values[@"upgrade_2"] unsignedIntValue];
  _upgradeRanks[3] = [values[@"upgrade_3"] unsignedIntValue];
}

void IosSaveManager::save() {
  NSDictionary *values = @{
    @"points" : [NSNumber numberWithInt:getPoints()],
    @"upgrade_0" : [NSNumber numberWithUnsignedInt:getRank(0)],
    @"upgrade_1" : [NSNumber numberWithUnsignedInt:getRank(1)],
    @"upgrade_2" : [NSNumber numberWithUnsignedInt:getRank(2)],
    @"upgrade_3" : [NSNumber numberWithUnsignedInt:getRank(3)]
  };

  [[NSUserDefaults standardUserDefaults] setValuesForKeysWithDictionary:values];
}

}  // namespace linguine::scampi
