#pragma once

#include "SaveManager.h"

namespace linguine::scampi {

class IosSaveManager : public SaveManager {
  public:
    IosSaveManager();

    ~IosSaveManager() override = default;

  protected:
    void load() override;

    void save() override;
};

}  // namespace linguine::scampi
