#pragma once

#include <LifecycleManager.h>

namespace linguine::pesto {

class WebLifecycleManager : public LifecycleManager {
  public:
    ~WebLifecycleManager() override = default;

    [[nodiscard]] bool isRunning() const override {
      return true;
    }
};

}  // namespace linguine::pesto
