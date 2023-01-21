#pragma once

#include <memory>

#include "InputManager.h"
#include "LifecycleManager.h"
#include "TimeManager.h"

namespace linguine {
  class Engine {
    public:
      Engine(const std::shared_ptr<InputManager>& inputManager,
             const std::shared_ptr<LifecycleManager>& lifecycleManager,
             const std::shared_ptr<TimeManager>& timeManager)
          : _inputManager(inputManager),
            _lifecycleManager(lifecycleManager),
            _timeManager(timeManager) {}

      void run();

    private:
      const std::shared_ptr<InputManager> _inputManager;
      const std::shared_ptr<LifecycleManager> _lifecycleManager;
      const std::shared_ptr<TimeManager> _timeManager;

      void update(float deltaTime);

      void fixedUpdate(float fixedDeltaTime);

      float _dtAccumulator = 0.0f;
      int _updateCounter = 0;

      float _fdtAccumulator = 0.0f;
      int _fixedUpdateCounter = 0;
  };
}
