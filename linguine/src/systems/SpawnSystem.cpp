#include "SpawnSystem.h"

#include "components/Asteroid.h"
#include "components/Bomb.h"
#include "components/Circle.h"
#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/PhysicalState.h"
#include "components/PowerUp.h"
#include "components/SpawnPoint.h"
#include "components/Text.h"
#include "components/Transform.h"
#include "components/Trigger.h"
#include "components/TutorialState.h"
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

    auto isTutorial = false;

    findEntities<TutorialState>()->each([this, fixedDeltaTime, &isTutorial, &entity](Entity& tutorialStateEntity) {
      isTutorial = true;

      auto tutorialState = tutorialStateEntity.get<TutorialState>();
      tutorialState->elapsed += fixedDeltaTime;

      switch (tutorialState->currentState) {
      case TutorialState::State::Movement:
        spawnMovementText(entity.get<Transform>()->position);
        tutorialState->currentState = TutorialState::State::WaitingForMovement;
        tutorialState->elapsed = 0.0f;
        break;
      case TutorialState::State::WaitingForMovement:
        if (tutorialState->hasMoved) {
          tutorialState->currentState = TutorialState::State::Scoring;
          tutorialState->elapsed = 0.0f;
        } else if (tutorialState->elapsed >= 5.0f) {
          spawnMovementText(entity.get<Transform>()->position);
          tutorialState->elapsed = 0.0f;
        }
        break;
      case TutorialState::State::Scoring:
        spawnScoringText(entity.get<Transform>()->position);
        tutorialState->currentState = TutorialState::State::WaitingForScore;
        tutorialState->elapsed = 0.0f;
        break;
      case TutorialState::State::WaitingForScore:
        if (tutorialState->hasScored) {
          tutorialState->currentState = TutorialState::State::Healing;
          tutorialState->elapsed = 0.0f;
        } else if (tutorialState->asteroidsSpawned >= 3) {
          if (tutorialState->elapsed >= 3.0f) {
            spawnScoringText(entity.get<Transform>()->position);
            tutorialState->asteroidsSpawned = 0;
            tutorialState->elapsed = 0.0f;
          }
        } else if (tutorialState->elapsed >= 3.0f) {
          spawnAsteroid(entity.get<Transform>()->position);
          tutorialState->asteroidsSpawned++;
          tutorialState->elapsed = 0.0f;
        }
        break;
      case TutorialState::State::Healing:
        spawnHealingText(entity.get<Transform>()->position);
        tutorialState->currentState = TutorialState::State::WaitingForHeal;
        tutorialState->elapsed = 0.0f;
        break;
      case TutorialState::State::WaitingForHeal:
        if (tutorialState->hasHealed) {
          tutorialState->currentState = TutorialState::State::Evasion;
          tutorialState->elapsed = 0.0f;
        } else if (tutorialState->elapsed >= 5.0f) {
          spawnHealingText(entity.get<Transform>()->position);
          tutorialState->elapsed = 0.0f;
        }
        break;
      case TutorialState::State::Evasion:
        spawnEvasionText(entity.get<Transform>()->position);
        tutorialState->currentState = TutorialState::State::Finished;
        tutorialState->elapsed = 0.0f;
        break;
      case TutorialState::State::Finished:
        if (tutorialState->elapsed >= 2.0f) {
          tutorialStateEntity.destroy();
        }
        break;
      }
    });

    if (!isTutorial) {
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
  drawable->feature->meshType = Plus;
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
  auto randomRotation = std::uniform_real_distribution(0.0f, glm::two_pi<float>());

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

  transform->rotation = glm::angleAxis(randomRotation(_random), glm::vec3(0.0f, 0.0f, 1.0f));

  obstacleEntity->add<PhysicalState>(transform->position, transform->rotation.z);
  obstacleEntity->add<CircleCollider>();

  auto drawable = obstacleEntity->add<Drawable>();
  drawable->feature = new ColoredFeature();
  drawable->feature->color = { 1.0f, 0.0f, 0.0f };
  drawable->feature->meshType = Mine;
  drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
  drawable.setRemovalListener([drawable](const Entity e) {
    drawable->renderable->destroy();
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

void SpawnSystem::spawnMovementText(glm::vec3 spawnPointPosition) {
  auto tutorialEntity1 = createEntity();

  auto transform1 = tutorialEntity1->add<Transform>();
  transform1->scale = glm::vec3(0.5f);
  transform1->position = { -4.5f, spawnPointPosition.y + 1.5f, 7.0f };

  tutorialEntity1->add<PhysicalState>(transform1->position, 0.0f);
  tutorialEntity1->add<CircleCollider>();
  tutorialEntity1->add<Trigger>();

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
  transform2->position = { -1.5f, spawnPointPosition.y, 7.0f };

  tutorialEntity2->add<PhysicalState>(transform2->position, 0.0f);
  tutorialEntity2->add<CircleCollider>();
  tutorialEntity2->add<Trigger>();

  auto text2 = tutorialEntity2->add<Text>();
  text2->feature = new TextFeature();
  text2->feature->text = "to move";
  text2->renderable = _renderer.create(std::unique_ptr<TextFeature>(text2->feature));
  text2.setRemovalListener([text2](const Entity& entity) {
    text2->renderable->destroy();
  });
}

void SpawnSystem::spawnScoringText(glm::vec3 spawnPointPosition) {
  auto tutorialEntity1 = createEntity();

  auto transform1 = tutorialEntity1->add<Transform>();
  transform1->scale = glm::vec3(0.5f);
  transform1->position = { -5.25f, spawnPointPosition.y + 1.5f, 7.0f };

  tutorialEntity1->add<PhysicalState>(transform1->position, 0.0f);
  tutorialEntity1->add<CircleCollider>();
  tutorialEntity1->add<Trigger>();

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
  transform2->position = { -3.25f, spawnPointPosition.y, 7.0f };

  tutorialEntity2->add<PhysicalState>(transform2->position, 0.0f);
  tutorialEntity2->add<CircleCollider>();
  tutorialEntity2->add<Trigger>();

  auto text2 = tutorialEntity2->add<Text>();
  text2->feature = new TextFeature();
  text2->feature->text = "to gain points";
  text2->feature->color = { 0.97345f, 0.36625f, 0.00561f };
  text2->renderable = _renderer.create(std::unique_ptr<TextFeature>(text2->feature));
  text2.setRemovalListener([text2](const Entity& entity) {
    text2->renderable->destroy();
  });
}

void SpawnSystem::spawnHealingText(glm::vec3 spawnPointPosition) {
  auto tutorialEntity1 = createEntity();

  auto transform1 = tutorialEntity1->add<Transform>();
  transform1->scale = glm::vec3(0.5f);
  transform1->position = { -3.75f, spawnPointPosition.y + 1.5f, 7.0f };

  tutorialEntity1->add<PhysicalState>(transform1->position, 0.0f);
  tutorialEntity1->add<CircleCollider>();
  tutorialEntity1->add<Trigger>();

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
  transform2->position = { -4.25f, spawnPointPosition.y, 7.0f };

  tutorialEntity2->add<PhysicalState>(transform2->position, 0.0f);
  tutorialEntity2->add<CircleCollider>();
  tutorialEntity2->add<Trigger>();

  auto text2 = tutorialEntity2->add<Text>();
  text2->feature = new TextFeature();
  text2->feature->text = "to regenerate them";
  text2->feature->color = { 0.0f, 1.0f, 0.0f };
  text2->renderable = _renderer.create(std::unique_ptr<TextFeature>(text2->feature));
  text2.setRemovalListener([text2](const Entity& entity) {
    text2->renderable->destroy();
  });
}

void SpawnSystem::spawnEvasionText(glm::vec3 spawnPointPosition) {
  auto tutorialEntity1 = createEntity();

  auto transform1 = tutorialEntity1->add<Transform>();
  transform1->scale = glm::vec3(0.5f);
  transform1->position = { -3.75f, spawnPointPosition.y, 7.0f };

  tutorialEntity1->add<PhysicalState>(transform1->position, 0.0f);
  tutorialEntity1->add<CircleCollider>();
  tutorialEntity1->add<Trigger>();

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
