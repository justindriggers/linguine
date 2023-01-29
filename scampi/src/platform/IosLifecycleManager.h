#pragma once

#include <LifecycleManager.h>

namespace linguine::scampi {
  class IosLifecycleManager : public LifecycleManager {
    public:
      [[nodiscard]] bool isRunning() const override;
  };
}
