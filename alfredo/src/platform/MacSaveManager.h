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
      _upgradeRanks[0] = 0;
      _upgradeRanks[1] = 0;
      _upgradeRanks[2] = 0;
      _upgradeRanks[3] = 0;
      _isNewPlayer = true;
    }

    void save() override {}
};

}  // namespace linguine::alfredo
