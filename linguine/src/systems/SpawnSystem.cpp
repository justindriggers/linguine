#include "SpawnSystem.h"

#include "components/Asteroid.h"
#include "components/Bomb.h"
#include "components/Circle.h"
#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/PhysicalState.h"
#include "components/PowerUp.h"
#include "components/SpawnPoint.h"
#include "components/Transform.h"
#include "components/Trigger.h"
#include "components/Velocity.h"

namespace linguine {

SpawnSystem::SpawnSystem(linguine::EntityManager& entityManager, linguine::Renderer& renderer)
    : System(entityManager), _renderer(renderer) {
  for (auto i = 0; i < 20; ++i) {
    spawnStars({ 0.0f, -10.0f + 1.25f * static_cast<float>(i), 0.0f });
  }
}

void SpawnSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<SpawnPoint, Transform>()->each([this, fixedDeltaTime](const Entity& entity) {
    auto spawnPoint = entity.get<SpawnPoint>();
    auto physicalState = entity.get<PhysicalState>();

    spawnPoint->powerUpElapsed += fixedDeltaTime;

    if (physicalState->currentPosition.y >= spawnPoint->lastSpawnPoint + spawnPoint->distance) {
      spawnPoint->lastSpawnPoint = physicalState->currentPosition.y;

      auto randomSpawn = std::uniform_real_distribution(0.0f, 1.0f);

      if (randomSpawn(_random) <= spawnPoint->spawnChance) {
        if (spawnPoint->powerUpElapsed >= spawnPoint->powerUpInterval) {
          spawnPoint->powerUpElapsed = 0.0f;
          spawnPowerUp(entity.get<Transform>()->position);
        } else {
          if (randomSpawn(_random) < 0.65f) {
            spawnObstacles(entity.get<Transform>()->position);
          } else {
            spawnAsteroid(entity.get<Transform>()->position);
          }
        }
      }
    }

    if (physicalState->currentPosition.y >= spawnPoint->lastStarSpawnPoint + spawnPoint->starDistance) {
      spawnPoint->lastStarSpawnPoint = physicalState->currentPosition.y;

      auto randomSpawn = std::uniform_real_distribution(0.0f, 1.0f);

      if (randomSpawn(_random) <= spawnPoint->spawnChance) {
        spawnStars(entity.get<Transform>()->position);
      }
    }
  });
}

void SpawnSystem::spawnPowerUp(glm::vec3 spawnPointPosition) {
  auto powerUpEntity = createEntity();
  powerUpEntity->add<PowerUp>();

  auto randomX = std::uniform_int_distribution(0, 2);

  auto transform = powerUpEntity->add<Transform>();

  switch (randomX(_random)) {
  case 0:
    transform->position = spawnPointPosition + glm::vec3(-4.0f, 0.0f, 1.0f);
    break;
  case 1:
    transform->position = spawnPointPosition + glm::vec3(0.0f, 0.0f, 1.0f);
    break;
  case 2:
    transform->position = spawnPointPosition + glm::vec3(4.0f, 0.0f, 1.0f);
    break;
  }

  powerUpEntity->add<PhysicalState>(transform->position, 0.0f);
  powerUpEntity->add<CircleCollider>();

  auto drawable = powerUpEntity->add<Drawable>();
  drawable->feature = new ColoredFeature();
  drawable->feature->meshType = Quad;
  drawable->feature->color = { 0.0f, 1.0f, 0.0f };
  drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
  drawable.setRemovalListener([drawable](const Entity e) {
    drawable->renderable->destroy();
  });
}

void SpawnSystem::spawnAsteroid(glm::vec3 spawnPointPosition) {
  auto asteroidEntity = createEntity();

  auto randomSize = std::uniform_int_distribution(1, 5);

  auto asteroid = asteroidEntity->add<Asteroid>();
  asteroid->points = randomSize(_random);

  auto randomX = std::uniform_int_distribution(0, 2);

  auto transform = asteroidEntity->add<Transform>();

  switch (randomX(_random)) {
  case 0:
    transform->position = spawnPointPosition + glm::vec3(-4.0f, 0.0f, 1.0f);
    break;
  case 1:
    transform->position = spawnPointPosition + glm::vec3(0.0f, 0.0f, 1.0f);
    break;
  case 2:
    transform->position = spawnPointPosition + glm::vec3(4.0f, 0.0f, 1.0f);
    break;
  }

  transform->scale = glm::vec3(0.5f * static_cast<float>(asteroid->points));

  asteroidEntity->add<PhysicalState>(transform->position, 0.0f);
  asteroidEntity->add<CircleCollider>()->radius = transform->scale.x / 2.0f;

  auto circle = asteroidEntity->add<Circle>();
  circle->feature = new CircleFeature();
  circle->feature->color = { 0.97345f, 0.36625f, 0.00561f };
  circle->renderable = _renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
  circle.setRemovalListener([circle](const Entity e) {
    circle->renderable->destroy();
  });
}

void SpawnSystem::spawnObstacles(glm::vec3 spawnPointPosition) {
  auto obstacleEntity = createEntity();
  obstacleEntity->add<Bomb>();

  auto randomX = std::uniform_int_distribution(0, 2);

  auto transform = obstacleEntity->add<Transform>();

  switch (randomX(_random)) {
  case 0:
    transform->position = spawnPointPosition + glm::vec3(-4.0f, 0.0f, 1.0f);
    break;
  case 1:
    transform->position = spawnPointPosition + glm::vec3(0.0f, 0.0f, 1.0f);
    break;
  case 2:
    transform->position = spawnPointPosition + glm::vec3(4.0f, 0.0f, 1.0f);
    break;
  default:
    break;
  }

  obstacleEntity->add<PhysicalState>(transform->position, 0.0f);
  obstacleEntity->add<CircleCollider>();

  auto circle = obstacleEntity->add<Circle>();
  circle->feature = new CircleFeature();
  circle->feature->color = { 1.0f, 0.0f, 0.0f };
  circle->renderable = _renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
  circle.setRemovalListener([circle](const Entity e) {
    circle->renderable->destroy();
  });
}

void SpawnSystem::spawnStars(glm::vec3 spawnPointPosition) {
  auto randomCount = std::uniform_int_distribution(1, 3);
  auto count = randomCount(_random);

  for (auto i = 0; i < count; ++i) {
    auto starEntity = createEntity();

    auto randomX = std::uniform_real_distribution(-4.0f, 4.0f);

    auto transform = starEntity->add<Transform>();
    transform->position = {randomX(_random), spawnPointPosition.y, 10.0f};

    transform->scale = glm::vec3(0.15f);

    starEntity->add<PhysicalState>(transform->position, 0.0f);
    starEntity->add<CircleCollider>()->radius = 0.075f;
    starEntity->add<Trigger>();
    starEntity->add<Velocity>()->velocity = {0.0f, 3.0f};

    auto circle = starEntity->add<Circle>();
    circle->feature = new CircleFeature();
    circle->renderable = _renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
    circle.setRemovalListener([circle](const Entity e) { circle->renderable->destroy(); });
  }
}

}  // namespace linguine
