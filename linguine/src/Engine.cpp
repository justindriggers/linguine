#include "Engine.h"

#include "entity/archetype/ArchetypeEntityManagerFactory.h"
#include "scenes/TitleScene.h"

namespace linguine {

Engine::Engine(
    const std::shared_ptr<Logger>& logger,
    const std::shared_ptr<AudioManager>& audioManager,
    const std::shared_ptr<InputManager>& inputManager,
    const std::shared_ptr<LeaderboardManager>& leaderboardManager,
    const std::shared_ptr<LifecycleManager>& lifecycleManager,
    const std::shared_ptr<Renderer>& renderer,
    const std::shared_ptr<SaveManager>& saveManager,
    const std::shared_ptr<TimeManager>& timeManager)
    : _entityManagerFactory(std::make_unique<archetype::ArchetypeEntityManagerFactory>()),
      _audioManager(audioManager),
      _inputManager(inputManager),
      _leaderboardManager(leaderboardManager),
      _lifecycleManager(lifecycleManager),
      _logger(logger),
      _renderer(renderer),
      _saveManager(saveManager),
      _timeManager(timeManager) {
  _audioManager->setMusicEnabled(_saveManager->isMusicEnabled());
  _audioManager->setSoundEffectsEnabled(_saveManager->isSoundEffectsEnabled());
  _currentScene = std::make_unique<TitleScene>(*this);
  _currentScene->init();
}

void Engine::tick() {
  if (_lifecycleManager->isPaused()) {
    return;
  }

  _inputManager->pollEvents();

  auto deltaTime = _timeManager->tick();

  while (_timeManager->fixedTick()) {
    fixedUpdate(_timeManager->getFixedTimeStep());
  }

  update(deltaTime);

  _renderer->draw();
  _audioManager->poll();

  if (_pendingScene) {
    _currentScene = std::move(_pendingScene);
    _pendingScene = {};

    _renderer->reset();
    _timeManager->reset();

    _currentScene->init();
  }
}

void Engine::pause() {
  _lifecycleManager->pause();
  _timeManager->pause();
  _audioManager->pause();
}

void Engine::resume() {
  _lifecycleManager->resume();
  _audioManager->resume();
  _timeManager->resume();
}

void Engine::update(float deltaTime) {
  _currentScene->update(deltaTime);
}

void Engine::fixedUpdate(float fixedDeltaTime) {
  _currentScene->fixedUpdate(fixedDeltaTime);
}

}  // namespace linguine
