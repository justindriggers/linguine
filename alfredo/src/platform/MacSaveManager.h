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
    }

    void save() override {}
};

}  // namespace linguine::alfredo
