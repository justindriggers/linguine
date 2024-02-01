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
      _level = 1;
      _isMusicEnabled = true;
      _isSoundEffectsEnabled = true;
      _isScreenShakeEnabled = true;
      _handedness = Handedness::Right;
      _stars = {};
    }

    void save() override {}
};

}  // namespace linguine::alfredo
