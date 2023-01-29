#include "Engine.h"

#include "Logger.h"

void linguine::Engine::run() {
  _currentTime = _timeManager->currentTime();

  while (_lifecycleManager->isRunning()) {
    tick();
  }
}

void linguine::Engine::tick() {
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

void linguine::Engine::update(float deltaTime) {
  _dtAccumulator += deltaTime;
  _updateCounter++;

  while (_dtAccumulator >= 1.0f) {
    _logger->log("update(): " + std::to_string(_updateCounter) + " fps");

    _dtAccumulator -= 1.0f;
    _updateCounter = 0;
  }
}

void linguine::Engine::fixedUpdate(float fixedDeltaTime) {
  _fdtAccumulator += fixedDeltaTime;
  _fixedUpdateCounter++;

  while (_fdtAccumulator >= 1.0f) {
    _logger->log("fixedUpdate(): " + std::to_string(_fixedUpdateCounter) + " fps");

    _fdtAccumulator -= 1.0f;
    _fixedUpdateCounter = 0;
  }
}
