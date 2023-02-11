#pragma once

#include <memory>

#include "InputManager.h"
#include "LifecycleManager.h"
#include "Logger.h"
#include "TimeManager.h"
#include "renderer/Renderer.h"

namespace linguine {

class Engine {
  public:
    Engine(const std::shared_ptr<Logger>& logger,
         const std::shared_ptr<InputManager>& inputManager,
         const std::shared_ptr<LifecycleManager>& lifecycleManager,
         const std::shared_ptr<Renderer>& renderer,
         const std::shared_ptr<TimeManager>& timeManager);

    void run();

    void tick();

  private:
    const std::shared_ptr<Logger> _logger;

    const std::shared_ptr<InputManager> _inputManager;
    const std::shared_ptr<LifecycleManager> _lifecycleManager;
    const std::shared_ptr<Renderer> _renderer;
    const std::shared_ptr<TimeManager> _timeManager;

    void update(float deltaTime);

    void fixedUpdate(float fixedDeltaTime);

    const float _fixedDeltaTime = 0.02f;

    time_t _currentTime = _timeManager->currentTime();
    float _accumulator = 0.0f;

    float _dtAccumulator = 0.0f;
    int _updateCounter = 0;

    float _fdtAccumulator = 0.0f;
    int _fixedUpdateCounter = 0;

    std::shared_ptr<Renderable> _renderable = nullptr;
    std::shared_ptr<Renderable> _renderable2 = nullptr;
};

}  // namespace linguine
