#pragma once

#include "SaveManager.h"

namespace linguine::carbonara {

class AndroidSaveManager : public SaveManager {
  public:
    AndroidSaveManager() : SaveManager() {
      load();
    }

    ~AndroidSaveManager() override = default;

  protected:
    void load() override {
      _points = 0;
      _lives = 0;
      _isMusicEnabled = true;
      _isSoundEffectsEnabled = true;
      _isScreenShakeEnabled = true;
      _handedness = Handedness::Right;
      _isNewPlayer = true;
    }

    void save() override {}
};

}  // namespace linguine::carbonara
