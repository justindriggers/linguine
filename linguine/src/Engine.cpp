#include "Engine.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Logger.h"
#include "renderer/features/QuadFeature.h"

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

  auto camera = _renderer->getCamera();
  auto cameraModelMatrix = glm::mat4(1.0f);
  cameraModelMatrix = glm::translate(cameraModelMatrix, glm::vec3(-0.75f, 0.0f, 0.0f));
  camera->viewMatrix = glm::inverse(cameraModelMatrix);
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

  const auto height = 5.0f;
  const auto viewport = _renderer->getViewport();

  auto camera = _renderer->getCamera();
  camera->projectionMatrix = glm::ortho(
      -height / 2.0f * viewport->getAspectRatio(),
      height / 2.0f * viewport->getAspectRatio(),
      -height / 2.0f,
      height / 2.0f,
      0.0f,
      10.0f
  );

  camera->viewProjectionMatrix = camera->projectionMatrix * camera->viewMatrix;

  if (_renderable->hasFeature<QuadFeature>()) {
    auto feature = _renderable->getFeature<QuadFeature>();
    feature->value += deltaTime;

    if (feature->value > 1.0f) {
      feature->value = 1.0f;
    }

    feature->modelMatrix = glm::translate(feature->modelMatrix, glm::vec3(0.0f, 0.25f, 0.0f) * deltaTime);
  }

  while (_dtAccumulator >= 1.0f) {
    _logger->log("update(): " + std::to_string(_updateCounter) + " fps");

    _dtAccumulator -= 1.0f;
    _updateCounter = 0;
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
