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
      _lives = 0;
      _isMusicEnabled = true;
      _isSoundEffectsEnabled = true;
      _isScreenShakeEnabled = true;
      _handedness = Handedness::Right;
    }

    void save() override {}
};

}  // namespace linguine::alfredo
