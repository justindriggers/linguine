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
      _isMusicEnabled = true;
      _isSoundEffectsEnabled = true;
      _isScreenShakeEnabled = true;
      _handedness = Handedness::Right;
    }

    void save() override {}
};

}  // namespace linguine::pesto
