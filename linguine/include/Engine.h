#pragma once

#include "ServiceLocator.h"

#include "LifecycleManager.h"
#include "Logger.h"
#include "TimeManager.h"
#include "audio/AudioManager.h"
#include "entity/EntityManagerFactory.h"
#include "input/InputManager.h"
#include "renderer/Renderer.h"

namespace linguine {

class Engine : public ServiceLocator, SceneManager {
  public:
    Engine(const std::shared_ptr<Logger>& logger,
           const std::shared_ptr<AudioManager>& audioManager,
           const std::shared_ptr<InputManager>& inputManager,
           const std::shared_ptr<LifecycleManager>& lifecycleManager,
           const std::shared_ptr<Renderer>& renderer,
           const std::shared_ptr<TimeManager>& timeManager);

    virtual ~Engine() = default;

    void run();

    void tick();

  private:
    EntityManagerFactory& getEntityManagerFactory() override {
      return *_entityManagerFactory;
    }

    AudioManager& getAudioManager() override {
      return *_audioManager;
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

    SceneManager& getSceneManager() override {
      return *this;
    }

    TimeManager& getTimeManager() override {
      return *_timeManager;
    }

    const std::unique_ptr<EntityManagerFactory> _entityManagerFactory;
    const std::shared_ptr<AudioManager> _audioManager;
    const std::shared_ptr<InputManager> _inputManager;
    const std::shared_ptr<LifecycleManager> _lifecycleManager;
    const std::shared_ptr<Logger> _logger;
    const std::shared_ptr<Renderer> _renderer;
    const std::shared_ptr<TimeManager> _timeManager;

    void load(std::unique_ptr<Scene> scene) override {
      _pendingScene = std::move(scene);
    }

    std::unique_ptr<Scene> _currentScene;
    std::unique_ptr<Scene> _pendingScene{};

    void update(float deltaTime);

    void fixedUpdate(float fixedDeltaTime);

    time_t _currentTime = _timeManager->currentTime();
    float _accumulator = 0.0f;
};

}  // namespace linguine
