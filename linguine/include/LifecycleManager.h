#pragma once

namespace linguine {
  class LifecycleManager {
    public:
      virtual bool isRunning() const = 0;
  };
}
