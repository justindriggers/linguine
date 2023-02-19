#include "Engine.h"

#include <random>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Logger.h"
#include "entity/archetype/ArchetypeEntityManager.h"
#include "renderer/features/QuadFeature.h"

namespace linguine {

struct Transform {
  glm::vec3 position = glm::vec3(0.0f);
  glm::quat rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
};

struct HasCamera {
  std::shared_ptr<Camera> camera;
};

struct Drawable {
  std::shared_ptr<Renderable> renderable;
};

struct Quad {
  std::shared_ptr<QuadFeature> feature;
};

struct Rotating {
  float speed = 1.0f;
};

struct Rising {
  float speed = 1.0f;
};

Engine::Engine(
    const std::shared_ptr<Logger>& logger,
    const std::shared_ptr<InputManager>& inputManager,
    const std::shared_ptr<LifecycleManager>& lifecycleManager,
    const std::shared_ptr<Renderer>& renderer,
    const std::shared_ptr<TimeManager>& timeManager)
    : _logger(logger), _inputManager(inputManager),
      _lifecycleManager(lifecycleManager), _renderer(renderer),
      _timeManager(timeManager), _entityManager(std::make_unique<archetype::ArchetypeEntityManager>()) {
  // Camera
  auto cameraEntity = _entityManager->create();

  auto cameraTransform = cameraEntity->add<Transform>();
  cameraTransform->position = glm::vec3(0.0f, 0.0f, 0.0f);

  auto hasCamera = cameraEntity->add<HasCamera>();
  hasCamera->camera = _renderer->getCamera();

  auto random = std::random_device();
  auto xDist = std::uniform_real_distribution(-2.55f, 2.5f);
  auto yDist = std::uniform_real_distribution(-5.0f, 5.0f);
  auto zDist = std::uniform_real_distribution(0.0f, 10.0f);
  auto normalDist = std::uniform_real_distribution(0.0f, 1.0f);
  auto rotationDist = std::uniform_real_distribution(-1.0f, 1.0f);

  auto componentDist = std::uniform_int_distribution(0, 1);

  for (int i = 0; i < 10'000; ++i) {
    auto entity = _entityManager->create();

    auto transform = entity->add<Transform>();
    transform->position = glm::vec3(xDist(random), yDist(random), zDist(random));

    auto quad = entity->add<Quad>();
    quad->feature = std::make_shared<QuadFeature>();
    quad->feature->color = glm::vec3(normalDist(random), normalDist(random), normalDist(random));

    auto drawable = entity->add<Drawable>();
    drawable->renderable = _renderer->create(quad->feature);

    if (componentDist(random) > 0) {
      auto rising = entity->add<Rising>();
      rising->speed = normalDist(random);
    } else {
      auto rotating = entity->add<Rotating>();
      rotating->speed = rotationDist(random);
    }
  }
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

void Engine::riserSystem(float deltaTime) {
  _entityManager->find<Rising, Transform>()->each([deltaTime](const Entity& entity) {
    const auto rising = entity.get<Rising>();
    const auto transform = entity.get<Transform>();

    transform->position += glm::vec3(0.0f, 1.0f, 0.0f) * rising->speed * deltaTime;
  });
}

void Engine::rotatorSystem(float deltaTime) {
  _entityManager->find<Rotating, Transform>()->each([deltaTime](const Entity& entity) {
    const auto rotating = entity.get<Rotating>();
    const auto transform = entity.get<Transform>();

    transform->rotation *= glm::angleAxis(glm::two_pi<float>() * rotating->speed * deltaTime, glm::vec3(0.0f, 0.0f, 1.0f));
  });
}

void Engine::quadTransformationSystem() {
  _entityManager->find<Transform, Quad>()->each([](const Entity& entity) {
    const auto transform = entity.get<Transform>();
    const auto quad = entity.get<Quad>();

    quad->feature->modelMatrix = glm::translate(glm::mat4(1.0f), transform->position)
                                 * glm::mat4_cast(transform->rotation);
  });
}

void Engine::cameraSystem() {
  const auto height = 10.0f;
  const auto viewport = _renderer->getViewport();

  _entityManager->find<Transform, HasCamera>()->each([height, viewport](const Entity& entity) {
    const auto transform = entity.get<Transform>();
    const auto hasCamera = entity.get<HasCamera>();
    const auto camera = hasCamera->camera;

    auto cameraModelMatrix = glm::translate(glm::mat4(1.0f), transform->position)
                             * glm::mat4_cast(transform->rotation);

    camera->viewMatrix = glm::inverse(cameraModelMatrix);
    camera->projectionMatrix = glm::ortho(
        -height / 2.0f * viewport->getAspectRatio(),
        height / 2.0f * viewport->getAspectRatio(),
        -height / 2.0f,
        height / 2.0f,
        0.0f,
        10.0f
    );

    camera->viewProjectionMatrix = camera->projectionMatrix * camera->viewMatrix;
  });
}

void Engine::update(float deltaTime) {
  _dtAccumulator += deltaTime;
  _updateCounter++;

  riserSystem(deltaTime);
  rotatorSystem(deltaTime);
  quadTransformationSystem();
  cameraSystem();

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
