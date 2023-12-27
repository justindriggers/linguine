#pragma once

#include "SaveManager.h"

namespace linguine::alfredo {

class MacSaveManager : public SaveManager {
  public:
    MacSaveManager() : SaveManager() {
      load();
    }

    ~MacSaveManager() override = default;

  protected:
    void load() override {
      _points = 0;
      _upgradeRanks[0] = 4;
      _upgradeRanks[1] = 4;
      _upgradeRanks[2] = 4;
      _upgradeRanks[3] = 4;
      _isNewPlayer = false;
    }

    void save() override {}
};

}  // namespace linguine::alfredo
