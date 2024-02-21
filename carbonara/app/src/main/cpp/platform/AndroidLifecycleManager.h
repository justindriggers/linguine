#pragma once

#include <LifecycleManager.h>

namespace linguine::carbonara {

class AndroidLifecycleManager : public LifecycleManager {
  public:
    ~AndroidLifecycleManager() override = default;

    [[nodiscard]] bool isRunning() const override {
      return true;
    }
};

}  // namespace linguine::carbonara
