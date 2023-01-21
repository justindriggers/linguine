#include <iostream>

#include "Engine.h"

void linguine::Engine::run() {
  const auto fixedDeltaTime = 0.02f;

  auto currentTime = _timeManager->currentTime();
  auto accumulator = 0.0f;

  while (_lifecycleManager->isRunning()) {
    _inputManager->pollEvents();

    auto newTime = _timeManager->currentTime();
    auto deltaTime = _timeManager->durationInSeconds(currentTime, newTime);
    currentTime = newTime;
    accumulator += deltaTime;

    while (accumulator >= fixedDeltaTime) {
      fixedUpdate(fixedDeltaTime);
      accumulator -= fixedDeltaTime;
    }

    update(deltaTime);

    // TODO render();
  }
}

void linguine::Engine::update(float deltaTime) {
  _dtAccumulator += deltaTime;
  _updateCounter++;

  while (_dtAccumulator >= 1.0f) {
    std::cout << "update(): " << _updateCounter << " fps" << std::endl;

    _dtAccumulator -= 1.0f;
    _updateCounter = 0;
  }
}

void linguine::Engine::fixedUpdate(float fixedDeltaTime) {
  _fdtAccumulator += fixedDeltaTime;
  _fixedUpdateCounter++;

  while (_fdtAccumulator >= 1.0f) {
    std::cout << "fixedUpdate(): " << _fixedUpdateCounter << " fps" << std::endl;

    _fdtAccumulator -= 1.0f;
    _fixedUpdateCounter = 0;
  }
}
