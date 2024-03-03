#pragma once

#include "SaveManager.h"

#include "jni/SharedPreferences.h"

namespace linguine::carbonara {

class AndroidSaveManager : public SaveManager {
  public:
    explicit AndroidSaveManager(android_app& app)
        : SaveManager(), _sharedPreferences(app, "state", SharedPreferences::Mode::Private) {
      load();
    }

    ~AndroidSaveManager() override = default;

  protected:
    void load() override;

    void save() override;

  private:
    SharedPreferences _sharedPreferences;
};

}  // namespace linguine::carbonara
