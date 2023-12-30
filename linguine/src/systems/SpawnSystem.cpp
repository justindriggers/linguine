#include "SpawnSystem.h"

#include "components/Asteroid.h"
#include "components/Bomb.h"
#include "components/Circle.h"
#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/PhysicalState.h"
#include "components/PowerUp.h"
#include "components/SpawnPoint.h"
#include "components/StarSpawnPoint.h"
#include "components/Text.h"
#include "components/Transform.h"
#include "components/Trigger.h"
#include "components/TutorialState.h"
#include "components/Velocity.h"

namespace linguine {

void SpawnSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<SpawnPoint, PhysicalState>()->each([this, fixedDeltaTime](const Entity& entity) {
    auto spawnPoint = entity.get<SpawnPoint>();
    auto physicalState = entity.get<PhysicalState>();

    auto isTutorial = false;

    findEntities<TutorialState>()->each([this, fixedDeltaTime, &isTutorial, &entity](Entity& tutorialStateEntity) {
      isTutorial = true;

      auto tutorialState = tutorialStateEntity.get<TutorialState>();
      tutorialState->elapsed += fixedDeltaTime;

      switch (tutorialState->currentState) {
      case TutorialState::State::Movement:
        spawnMovementText(entity.get<PhysicalState>()->currentPosition.y);
        tutorialState->currentState = TutorialState::State::WaitingForMovement;
        tutorialState->elapsed = 0.0f;
        break;
      case TutorialState::State::WaitingForMovement:
        if (tutorialState->hasMoved) {
          tutorialState->currentState = TutorialState::State::Scoring;
          tutorialState->elapsed = 0.0f;
        } else if (tutorialState->elapsed >= 5.0f) {
          spawnMovementText(entity.get<PhysicalState>()->currentPosition.y);
          tutorialState->elapsed = 0.0f;
        }
        break;
      case TutorialState::State::Scoring:
        spawnScoringText(entity.get<PhysicalState>()->currentPosition.y);
        tutorialState->currentState = TutorialState::State::WaitingForScore;
        tutorialState->elapsed = 0.0f;
        break;
      case TutorialState::State::WaitingForScore:
        if (tutorialState->hasScored) {
          tutorialState->currentState = TutorialState::State::Healing;
          tutorialState->elapsed = 0.0f;
        } else if (tutorialState->asteroidsSpawned >= 3) {
          if (tutorialState->elapsed >= 3.0f) {
            spawnScoringText(entity.get<PhysicalState>()->currentPosition.y);
            tutorialState->asteroidsSpawned = 0;
            tutorialState->elapsed = 0.0f;
          }
        } else if (tutorialState->elapsed >= 3.0f) {
          spawnAsteroid(entity.get<PhysicalState>()->currentPosition.y);
          tutorialState->asteroidsSpawned++;
          tutorialState->elapsed = 0.0f;
        }
        break;
      case TutorialState::State::Healing:
        spawnHealingText(entity.get<PhysicalState>()->currentPosition.y);
        tutorialState->currentState = TutorialState::State::WaitingForHeal;
        tutorialState->elapsed = 0.0f;
        break;
      case TutorialState::State::WaitingForHeal:
        if (tutorialState->hasHealed) {
          tutorialState->currentState = TutorialState::State::Evasion;
          tutorialState->elapsed = 0.0f;
        } else if (tutorialState->elapsed >= 5.0f) {
          spawnHealingText(entity.get<PhysicalState>()->currentPosition.y);
          tutorialState->elapsed = 0.0f;
        }
        break;
      case TutorialState::State::Evasion:
        spawnEvasionText(entity.get<PhysicalState>()->currentPosition.y);
        tutorialState->currentState = TutorialState::State::Finished;
        tutorialState->elapsed = 0.0f;
        break;
      case TutorialState::State::Finished:
        if (tutorialState->elapsed >= 2.0f) {
          tutorialStateEntity.destroy();

          auto spawnPoint = entity.get<SpawnPoint>();
          auto physicalState = entity.get<PhysicalState>();

          spawnPoint->lastSpawnPoint = physicalState->currentPosition.y;
        }
        break;
      }
    });

    if (!isTutorial) {
      spawnPoint->powerUpElapsed += fixedDeltaTime;

      while (physicalState->currentPosition.y >= spawnPoint->lastSpawnPoint + spawnPoint->distance) {
        spawnPoint->lastSpawnPoint += spawnPoint->distance;

        auto randomSpawn = std::uniform_real_distribution(0.0f, 1.0f);

        if (randomSpawn(_random) <= spawnPoint->spawnChance) {
          if (spawnPoint->powerUpElapsed >= spawnPoint->powerUpInterval) {
            spawnPoint->powerUpElapsed = 0.0f;
            spawnPowerUp(spawnPoint->lastSpawnPoint);
          } else {
            if (randomSpawn(_random) < 0.65f) {
              spawnObstacles(spawnPoint->lastSpawnPoint);
            } else {
              spawnAsteroid(spawnPoint->lastSpawnPoint);
            }
          }
        }
      }
    }
  });

  findEntities<StarSpawnPoint, PhysicalState>()->each([this](const Entity& entity) {
    auto starSpawnPoint = entity.get<StarSpawnPoint>();
    auto physicalState = entity.get<PhysicalState>();

    while (physicalState->currentPosition.y >= starSpawnPoint->lastSpawnPoint + starSpawnPoint->distance) {
      starSpawnPoint->lastSpawnPoint += starSpawnPoint->distance;
      spawnStars(starSpawnPoint->lastSpawnPoint);
    }
  });
}

void SpawnSystem::spawnPowerUp(float y) {
  auto powerUpEntity = createEntity();
  powerUpEntity->add<PowerUp>();

  auto randomX = std::uniform_int_distribution(0, 2);

  auto transform = powerUpEntity->add<Transform>();
  transform->position = glm::vec3(0.0f, y, 1.0f);
  transform->scale = glm::vec3(1.25f);

  switch (randomX(_random)) {
  case 0:
    transform->position.x -= 4.0f;
    break;
  case 2:
    transform->position.x += 4.0f;
    break;
  }

  powerUpEntity->add<PhysicalState>(transform->position, 0.0f);
  powerUpEntity->add<CircleCollider>()->radius = 0.625f;
  powerUpEntity->add<Velocity>()->velocity = { 0.0f, -3.0f };

  auto drawable = powerUpEntity->add<Drawable>();
  drawable->feature = new ColoredFeature();
  drawable->feature->meshType = Plus;
  drawable->feature->color = { 0.0f, 1.0f, 0.0f };
  drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
  drawable.setRemovalListener([drawable](const Entity e) {
    drawable->renderable->destroy();
  });
}

void SpawnSystem::spawnAsteroid(float y) {
  auto asteroidEntity = createEntity();

  auto randomSize = std::uniform_int_distribution(1, 5);

  auto asteroid = asteroidEntity->add<Asteroid>();
  asteroid->points = randomSize(_random);

  auto randomX = std::uniform_int_distribution(0, 2);

  auto transform = asteroidEntity->add<Transform>();
  transform->position = glm::vec3(0.0f, y, 1.0f);

  switch (randomX(_random)) {
  case 0:
    transform->position.x -= 4.0f;
    break;
  case 2:
    transform->position.x += 4.0f;
    break;
  }

  transform->scale = glm::vec3(0.5f + 0.5f * static_cast<float>(asteroid->points));

  asteroidEntity->add<PhysicalState>(transform->position, 0.0f);
  asteroidEntity->add<CircleCollider>()->radius = transform->scale.x / 2.0f;
  asteroidEntity->add<Velocity>()->velocity = { 0.0f, -3.0f };

  auto circle = asteroidEntity->add<Circle>();
  circle->feature = new CircleFeature();
  circle->feature->color = { 0.97345f, 0.36625f, 0.00561f };
  circle->renderable = _renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
  circle.setRemovalListener([circle](const Entity e) {
    circle->renderable->destroy();
  });
}

void SpawnSystem::spawnObstacles(float y) {
  auto obstacleEntity = createEntity();
  obstacleEntity->add<Bomb>();

  auto randomX = std::uniform_int_distribution(0, 2);
  auto randomRotation = std::uniform_real_distribution(0.0f, glm::two_pi<float>());

  auto transform = obstacleEntity->add<Transform>();
  transform->scale = glm::vec3(1.25f);
  transform->position = glm::vec3(0.0f, y, 1.0f);

  switch (randomX(_random)) {
  case 0:
    transform->position.x -= 4.0f;
    break;
  case 2:
    transform->position.x += 4.0f;
    break;
  default:
    break;
  }

  transform->rotation = glm::angleAxis(randomRotation(_random), glm::vec3(0.0f, 0.0f, 1.0f));

  obstacleEntity->add<PhysicalState>(transform->position, transform->rotation.z);
  obstacleEntity->add<CircleCollider>()->radius = 0.625f;
  obstacleEntity->add<Velocity>()->velocity = { 0.0f, -3.0f };

  auto drawable = obstacleEntity->add<Drawable>();
  drawable->feature = new ColoredFeature();
  drawable->feature->color = { 1.0f, 0.0f, 0.0f };
  drawable->feature->meshType = Mine;
  drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
  drawable.setRemovalListener([drawable](const Entity e) {
    drawable->renderable->destroy();
  });
}

void SpawnSystem::spawnStars(float y) {
  auto randomCount = std::uniform_int_distribution(1, 3);
  auto count = randomCount(_random);

  for (auto i = 0; i < count; ++i) {
    auto starEntity = createEntity();

    auto randomX = std::uniform_real_distribution(-6.0f, 6.0f);

    auto transform = starEntity->add<Transform>();
    transform->position = { randomX(_random), y, 10.0f };

    transform->scale = glm::vec3(0.1875f);

    starEntity->add<PhysicalState>(transform->position, 0.0f);
    starEntity->add<CircleCollider>()->radius = transform->scale.x / 2.0f;
    starEntity->add<Trigger>();

    auto circle = starEntity->add<Circle>();
    circle->feature = new CircleFeature();
    circle->renderable = _renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
    circle.setRemovalListener([circle](const Entity e) {
      circle->renderable->destroy();
    });
  }
}

void SpawnSystem::spawnMovementText(float y) {
  auto tutorialEntity1 = createEntity();

  auto transform1 = tutorialEntity1->add<Transform>();
  transform1->scale = glm::vec3(0.5f);
  transform1->position = { -4.5f, y + 1.5f, 7.0f };

  tutorialEntity1->add<PhysicalState>(transform1->position, 0.0f);
  tutorialEntity1->add<CircleCollider>();
  tutorialEntity1->add<Trigger>();
  tutorialEntity1->add<Velocity>()->velocity = { 0.0f, -3.0f };

  auto text1 = tutorialEntity1->add<Text>();
  text1->feature = new TextFeature();
  text1->feature->text = "Swipe left or right";
  text1->renderable = _renderer.create(std::unique_ptr<TextFeature>(text1->feature));
  text1.setRemovalListener([text1](const Entity& entity) {
    text1->renderable->destroy();
  });

  auto tutorialEntity2 = createEntity();

  auto transform2 = tutorialEntity2->add<Transform>();
  transform2->scale = glm::vec3(0.5f);
  transform2->position = { -1.5f, y, 7.0f };

  tutorialEntity2->add<PhysicalState>(transform2->position, 0.0f);
  tutorialEntity2->add<CircleCollider>();
  tutorialEntity2->add<Trigger>();
  tutorialEntity2->add<Velocity>()->velocity = { 0.0f, -3.0f };

  auto text2 = tutorialEntity2->add<Text>();
  text2->feature = new TextFeature();
  text2->feature->text = "to move";
  text2->renderable = _renderer.create(std::unique_ptr<TextFeature>(text2->feature));
  text2.setRemovalListener([text2](const Entity& entity) {
    text2->renderable->destroy();
  });
}

void SpawnSystem::spawnScoringText(float y) {
  auto tutorialEntity1 = createEntity();

  auto transform1 = tutorialEntity1->add<Transform>();
  transform1->scale = glm::vec3(0.5f);
  transform1->position = { -5.25f, y + 1.5f, 7.0f };

  tutorialEntity1->add<PhysicalState>(transform1->position, 0.0f);
  tutorialEntity1->add<CircleCollider>();
  tutorialEntity1->add<Trigger>();
  tutorialEntity1->add<Velocity>()->velocity = { 0.0f, -3.0f };

  auto text1 = tutorialEntity1->add<Text>();
  text1->feature = new TextFeature();
  text1->feature->text = "Collide with asteroids";
  text1->feature->color = { 0.97345f, 0.36625f, 0.00561f };
  text1->renderable = _renderer.create(std::unique_ptr<TextFeature>(text1->feature));
  text1.setRemovalListener([text1](const Entity& entity) {
    text1->renderable->destroy();
  });

  auto tutorialEntity2 = createEntity();

  auto transform2 = tutorialEntity2->add<Transform>();
  transform2->scale = glm::vec3(0.5f);
  transform2->position = { -3.25f, y, 7.0f };

  tutorialEntity2->add<PhysicalState>(transform2->position, 0.0f);
  tutorialEntity2->add<CircleCollider>();
  tutorialEntity2->add<Trigger>();
  tutorialEntity2->add<Velocity>()->velocity = { 0.0f, -3.0f };

  auto text2 = tutorialEntity2->add<Text>();
  text2->feature = new TextFeature();
  text2->feature->text = "to gain points";
  text2->feature->color = { 0.97345f, 0.36625f, 0.00561f };
  text2->renderable = _renderer.create(std::unique_ptr<TextFeature>(text2->feature));
  text2.setRemovalListener([text2](const Entity& entity) {
    text2->renderable->destroy();
  });
}

void SpawnSystem::spawnHealingText(float y) {
  auto tutorialEntity1 = createEntity();

  auto transform1 = tutorialEntity1->add<Transform>();
  transform1->scale = glm::vec3(0.5f);
  transform1->position = { -3.75f, y + 1.5f, 7.0f };

  tutorialEntity1->add<PhysicalState>(transform1->position, 0.0f);
  tutorialEntity1->add<CircleCollider>();
  tutorialEntity1->add<Trigger>();
  tutorialEntity1->add<Velocity>()->velocity = { 0.0f, -3.0f };

  auto text1 = tutorialEntity1->add<Text>();
  text1->feature = new TextFeature();
  text1->feature->text = "Tap your shields";
  text1->feature->color = { 0.0f, 1.0f, 0.0f };
  text1->renderable = _renderer.create(std::unique_ptr<TextFeature>(text1->feature));
  text1.setRemovalListener([text1](const Entity& entity) {
    text1->renderable->destroy();
  });

  auto tutorialEntity2 = createEntity();

  auto transform2 = tutorialEntity2->add<Transform>();
  transform2->scale = glm::vec3(0.5f);
  transform2->position = { -4.25f, y, 7.0f };

  tutorialEntity2->add<PhysicalState>(transform2->position, 0.0f);
  tutorialEntity2->add<CircleCollider>();
  tutorialEntity2->add<Trigger>();
  tutorialEntity2->add<Velocity>()->velocity = { 0.0f, -3.0f };

  auto text2 = tutorialEntity2->add<Text>();
  text2->feature = new TextFeature();
  text2->feature->text = "to regenerate them";
  text2->feature->color = { 0.0f, 1.0f, 0.0f };
  text2->renderable = _renderer.create(std::unique_ptr<TextFeature>(text2->feature));
  text2.setRemovalListener([text2](const Entity& entity) {
    text2->renderable->destroy();
  });
}

void SpawnSystem::spawnEvasionText(float y) {
  auto tutorialEntity1 = createEntity();

  auto transform1 = tutorialEntity1->add<Transform>();
  transform1->scale = glm::vec3(0.5f);
  transform1->position = { -3.75f, y, 7.0f };

  tutorialEntity1->add<PhysicalState>(transform1->position, 0.0f);
  tutorialEntity1->add<CircleCollider>();
  tutorialEntity1->add<Trigger>();
  tutorialEntity1->add<Velocity>()->velocity = { 0.0f, -3.0f };

  auto text1 = tutorialEntity1->add<Text>();
  text1->feature = new TextFeature();
  text1->feature->text = "Avoid the mines!";
  text1->feature->color = { 1.0f, 0.0f, 0.0f };
  text1->renderable = _renderer.create(std::unique_ptr<TextFeature>(text1->feature));
  text1.setRemovalListener([text1](const Entity& entity) {
    text1->renderable->destroy();
  });
}

}  // namespace linguine
