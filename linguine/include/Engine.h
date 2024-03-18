#pragma once

#include "ServiceLocator.h"

#include "LifecycleManager.h"
#include "Logger.h"
#include "Scene.h"
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
           const std::shared_ptr<LeaderboardManager>& leaderboardManager,
           const std::shared_ptr<LifecycleManager>& lifecycleManager,
           const std::shared_ptr<Renderer>& renderer,
           const std::shared_ptr<SaveManager>& saveManager,
           const std::shared_ptr<TimeManager>& timeManager,
           const std::shared_ptr<UrlHandler>& urlHandler);

    virtual ~Engine() = default;

    void tick();

    void pause();

    void resume();

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

    LeaderboardManager& getLeaderboardManager() override {
      return *_leaderboardManager;
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

    SaveManager& getSaveManager() override {
      return *_saveManager;
    }

    SceneManager& getSceneManager() override {
      return *this;
    }

    TimeManager& getTimeManager() override {
      return *_timeManager;
    }

    UrlHandler& getUrlHandler() override {
      return *_urlHandler;
    }

    const std::unique_ptr<EntityManagerFactory> _entityManagerFactory;
    const std::shared_ptr<AudioManager> _audioManager;
    const std::shared_ptr<InputManager> _inputManager;
    const std::shared_ptr<LeaderboardManager> _leaderboardManager;
    const std::shared_ptr<LifecycleManager> _lifecycleManager;
    const std::shared_ptr<Logger> _logger;
    const std::shared_ptr<Renderer> _renderer;
    const std::shared_ptr<SaveManager> _saveManager;
    const std::shared_ptr<TimeManager> _timeManager;
    const std::shared_ptr<UrlHandler> _urlHandler;

    void load(std::unique_ptr<Scene> scene) override {
      _pendingScene = std::move(scene);
    }

    std::unique_ptr<Scene> _currentScene;
    std::unique_ptr<Scene> _pendingScene{};

    void update(float deltaTime);

    void fixedUpdate(float fixedDeltaTime);
};

}  // namespace linguine
