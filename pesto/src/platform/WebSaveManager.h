#pragma once

#include "SaveManager.h"

namespace linguine::pesto {

class WebSaveManager : public SaveManager {
  public:
    WebSaveManager() : SaveManager() {
      load();
    }

    ~WebSaveManager() override = default;

  protected:
    void load() override {
      _points = 0;
      _upgradeRanks[0] = 0;
      _upgradeRanks[1] = 0;
      _upgradeRanks[2] = 0;
      _upgradeRanks[3] = 0;
    }

    void save() override {}
};

}  // namespace linguine::pesto
