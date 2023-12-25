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
    const std::shared_ptr<SaveManager>& saveManager,
    const std::shared_ptr<TimeManager>& timeManager)
    : _entityManagerFactory(std::make_unique<archetype::ArchetypeEntityManagerFactory>()),
      _audioManager(audioManager),
      _inputManager(inputManager),
      _lifecycleManager(lifecycleManager),
      _logger(logger),
      _renderer(renderer),
      _saveManager(saveManager),
      _timeManager(timeManager),
      _currentScene(std::make_unique<ShopScene>(*this)) {}

void Engine::run() {
  while (_lifecycleManager->isRunning()) {
    tick();
  }
}

void Engine::tick() {
  _inputManager->pollEvents();

  auto deltaTime = _timeManager->tick();

  while (_timeManager->fixedTick()) {
    fixedUpdate(_timeManager->getFixedTimeStep());
  }

  update(deltaTime);

  _renderer->draw();

  if (_pendingScene) {
    _currentScene = std::move(_pendingScene);
    _pendingScene = {};

    _renderer->reset();
    _timeManager->reset();
  }
}

void Engine::update(float deltaTime) {
  _currentScene->update(deltaTime);
}

void Engine::fixedUpdate(float fixedDeltaTime) {
  _currentScene->fixedUpdate(fixedDeltaTime);
}

}  // namespace linguine
