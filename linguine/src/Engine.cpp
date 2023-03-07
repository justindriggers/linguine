#include "Engine.h"

#include "entity/archetype/ArchetypeEntityManagerFactory.h"
#include "scenes/TestScene.h"

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
      _currentScene(std::make_unique<TestScene>(*this)) {}

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

  while (_accumulator >= _fixedDeltaTime) {
    fixedUpdate(_fixedDeltaTime);
    _accumulator -= _fixedDeltaTime;
  }

  update(deltaTime);

  _renderer->draw();
}

void Engine::update(float deltaTime) {
  _currentScene->update(deltaTime);
}

void Engine::fixedUpdate(float fixedDeltaTime) {
  _currentScene->fixedUpdate(fixedDeltaTime);
}

}  // namespace linguine
