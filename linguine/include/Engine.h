#pragma once

#include "ServiceLocator.h"

#include "InputManager.h"
#include "LifecycleManager.h"
#include "Logger.h"
#include "Scene.h"
#include "TimeManager.h"
#include "entity/EntityManagerFactory.h"
#include "renderer/Renderer.h"

namespace linguine {

class Engine : public ServiceLocator {
  public:
    Engine(const std::shared_ptr<Logger>& logger,
         const std::shared_ptr<InputManager>& inputManager,
         const std::shared_ptr<LifecycleManager>& lifecycleManager,
         const std::shared_ptr<Renderer>& renderer,
         const std::shared_ptr<TimeManager>& timeManager);

    void run();

    void tick();

  private:
    EntityManagerFactory& getEntityManagerFactory() override {
      return *_entityManagerFactory;
    }

    InputManager& getInputManager() override {
      return *_inputManager;
    }

    LifecycleManager& getLifecycleManager() override {
      return *_lifecycleManager;
    }

    Logger& getLogger() override {
      return *_logger;
    }

    Renderer& getRenderer() override {
      return *_renderer;
    }

    TimeManager& getTimeManager() override {
      return *_timeManager;
    }

    const std::unique_ptr<EntityManagerFactory> _entityManagerFactory;
    const std::shared_ptr<InputManager> _inputManager;
    const std::shared_ptr<LifecycleManager> _lifecycleManager;
    const std::shared_ptr<Logger> _logger;
    const std::shared_ptr<Renderer> _renderer;
    const std::shared_ptr<TimeManager> _timeManager;

    std::unique_ptr<Scene> _currentScene;

    void update(float deltaTime);

    void fixedUpdate(float fixedDeltaTime);

    constexpr static float _fixedDeltaTime = 0.02f;

    time_t _currentTime = _timeManager->currentTime();
    float _accumulator = 0.0f;
};

}  // namespace linguine
