#pragma once

#include <LifecycleManager.h>

namespace linguine::alfredo {
  class MacLifecycleManager : public LifecycleManager {
    public:
      [[nodiscard]] bool isRunning() const override;
  };
}
