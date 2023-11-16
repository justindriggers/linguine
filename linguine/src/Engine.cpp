#include "Engine.h"

#include "entity/archetype/ArchetypeEntityManagerFactory.h"
#include "scenes/ShopScene.h"

namespace linguine {

Engine::Engine(
    const std::shared_ptr<Logger>& logger,
    const std::shared_ptr<AudioManager>& audioManager,
    const std::shared_ptr<InputManager>& inputManager,
    const std::shared_ptr<LifecycleManager>& lifecycleManager,
    const std::shared_ptr<Renderer>& renderer,
    const std::shared_ptr<TimeManager>& timeManager)
    : _entityManagerFactory(std::make_unique<archetype::ArchetypeEntityManagerFactory>()),
      _audioManager(audioManager),
      _inputManager(inputManager),
      _lifecycleManager(lifecycleManager),
      _logger(logger),
      _renderer(renderer),
      _timeManager(timeManager),
      _currentScene(std::make_unique<ShopScene>(*this, 0)) {}

void Engine::run() {
  _currentTime = _timeManager->currentTime();

  while (_lifecycleManager->isRunning()) {
    tick();
  }
}

void Engine::tick() {
  _inputManager->pollEvents();

  auto newTime = _timeManager->currentTime();
  auto deltaTime = _timeManager->durationInSeconds(_currentTime, newTime);
  _currentTime = newTime;
  _accumulator += deltaTime;

  const auto fixedDeltaTime = _timeManager->getFixedTimeStep();

  while (_accumulator >= fixedDeltaTime) {
    fixedUpdate(fixedDeltaTime);
    _accumulator -= fixedDeltaTime;
  }

  update(deltaTime);

  _renderer->draw();

  if (_pendingScene) {
    _currentScene = std::move(_pendingScene);
    _pendingScene = {};

    // Render a single frame in order to clear the current frame buffers
    _renderer->draw();
  }
}

void Engine::update(float deltaTime) {
  _currentScene->update(deltaTime);
}

void Engine::fixedUpdate(float fixedDeltaTime) {
  _currentScene->fixedUpdate(fixedDeltaTime);
}

}  // namespace linguine
