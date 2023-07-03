#pragma once

namespace linguine {

class LifecycleManager {
  public:
    virtual ~LifecycleManager() = default;

    [[nodiscard]] virtual bool isRunning() const = 0;
};

}  // namespace linguine
