#include "Engine.h"

#include "Logger.h"
#include "renderer/features/QuadFeature.h"
#include "renderer/features/TriangleFeature.h"

namespace linguine {

Engine::Engine(
    const std::shared_ptr<Logger>& logger,
    const std::shared_ptr<InputManager>& inputManager,
    const std::shared_ptr<LifecycleManager>& lifecycleManager,
    const std::shared_ptr<Renderer>& renderer,
    const std::shared_ptr<TimeManager>& timeManager)
    : _logger(logger), _inputManager(inputManager),
      _lifecycleManager(lifecycleManager), _renderer(renderer),
      _timeManager(timeManager) {
  auto feature = std::make_shared<QuadFeature>();
  _renderable = _renderer->create(feature);
}

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
  _dtAccumulator += deltaTime;
  _updateCounter++;

  if (_renderable->hasFeature<QuadFeature>()) {
    auto feature = _renderable->getFeature<QuadFeature>();
    feature->value += deltaTime;

    if (feature->value > 1.0f) {
      feature->value = 1.0f;
    }
  }

  while (_dtAccumulator >= 1.0f) {
    _logger->log("update(): " + std::to_string(_updateCounter) + " fps");

    _dtAccumulator -= 1.0f;
    _updateCounter = 0;

    if (_renderable->hasFeature<QuadFeature>()) {
      auto feature = std::make_shared<TriangleFeature>();
      _renderable->setFeature(feature);
    }
  }
}

void Engine::fixedUpdate(float fixedDeltaTime) {
  _fdtAccumulator += fixedDeltaTime;
  _fixedUpdateCounter++;

  while (_fdtAccumulator >= 1.0f) {
    _logger->log("fixedUpdate(): " + std::to_string(_fixedUpdateCounter) + " fps");

    _fdtAccumulator -= 1.0f;
    _fixedUpdateCounter = 0;
  }
}

}  // namespace linguine
