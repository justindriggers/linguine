#include "FpsSystem.h"

namespace linguine {

void FpsSystem::update(float deltaTime) {
  _dtAccumulator += deltaTime;
  _updateCounter++;

  while (_dtAccumulator >= 1.0f) {
    _logger.log("update(): " + std::to_string(_updateCounter) + " fps");

    _dtAccumulator -= 1.0f;
    _updateCounter = 0;
  }
}

void FpsSystem::fixedUpdate(float fixedDeltaTime) {
  _fdtAccumulator += fixedDeltaTime;
  _fixedUpdateCounter++;

  while (_fdtAccumulator >= 1.0f) {
    _logger.log("fixedUpdate(): " + std::to_string(_fixedUpdateCounter) + " fps");

    _fdtAccumulator -= 1.0f;
    _fixedUpdateCounter = 0;
  }
}

}  // namespace linguine
