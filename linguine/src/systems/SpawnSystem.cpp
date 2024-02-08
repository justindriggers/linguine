#include "SpawnSystem.h"

#include "components/Alive.h"
#include "components/Asteroid.h"
#include "components/Bomb.h"
#include "components/Circle.h"
#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/Health.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/PowerUp.h"
#include "components/Score.h"
#include "components/SpawnPoint.h"
#include "components/StarSpawnPoint.h"
#include "components/Text.h"
#include "components/Transform.h"
#include "components/Trigger.h"
#include "components/TutorialState.h"
#include "components/Velocity.h"
#include "data/Palette.h"

namespace linguine {

void SpawnSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<SpawnPoint, PhysicalState>()->each([this, fixedDeltaTime](const Entity& entity) {
    auto spawnPoint = entity.get<SpawnPoint>();

    for (auto& cooldown : spawnPoint->powerUpCooldowns) {
      cooldown.second += fixedDeltaTime;
    }

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
        if (tutorialState->hasMoved && tutorialState->elapsed >= 2.0f) {
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
          if (tutorialState->elapsed >= 4.0f) {
            spawnScoringText(entity.get<PhysicalState>()->currentPosition.y);
            tutorialState->asteroidsSpawned = 0;
            tutorialState->elapsed = 0.0f;
          }
        } else if (tutorialState->elapsed >= 4.0f) {
          spawnAsteroid(entity.get<PhysicalState>()->currentPosition.y, 4);
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
        } else if (tutorialState->elapsed >= 5.0f) {
          spawnHealingText(entity.get<PhysicalState>()->currentPosition.y);
          tutorialState->elapsed = 0.0f;
        }
        break;
      case TutorialState::State::Evasion:
        if (tutorialState->elapsed >= 2.0f) {
          spawnEvasionText(entity.get<PhysicalState>()->currentPosition.y);
          tutorialState->currentState = TutorialState::State::Finished;
          tutorialState->elapsed = 0.0f;
        }
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

      while (spawnPoint->spawnChance > 0.0f && physicalState->currentPosition.y >= spawnPoint->lastSpawnPoint + spawnPoint->distance) {
        spawnPoint->lastSpawnPoint += spawnPoint->distance;

        auto randomSpawn = std::uniform_real_distribution(0.0f, 1.0f);

        if (randomSpawn(_random) <= spawnPoint->spawnChance) {
          if (spawnPoint->powerUpElapsed >= spawnPoint->powerUpInterval
              && spawnPowerUp(spawnPoint)) {
            spawnPoint->powerUpElapsed = 0.0f;
          } else {
            if (randomSpawn(_random) < spawnPoint->obstacleSpawnChance) {
              if (randomSpawn(_random) < spawnPoint->wallSpawnChance) {
                spawnObstacle(-4.0f, spawnPoint->lastSpawnPoint);
                spawnObstacle(0.0f, spawnPoint->lastSpawnPoint);
                spawnObstacle(4.0f, spawnPoint->lastSpawnPoint);
              } else {
                auto randomX = std::uniform_int_distribution(0, 2);
                auto x = 0.0f;

                switch (randomX(_random)) {
                case 0:
                  x -= 4.0f;
                  break;
                case 2:
                  x += 4.0f;
                  break;
                default:
                  break;
                }

                spawnObstacle(x, spawnPoint->lastSpawnPoint);
              }
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

bool SpawnSystem::spawnPowerUp(Component<SpawnPoint>& point) {
  std::unordered_map<PowerUp::Type, int> weights {};
  auto total = 0;

  // Speed Boost
  if (point->speedBoostEnabled) {
    auto weight = 5;

    weights[PowerUp::Type::SpeedBoost] = weight;
    total += weight;
  }

  // Mass Heal
  if (point->regenEnabled) {
    auto massHealCooldown = point->powerUpCooldowns.find(2);

    if (massHealCooldown == point->powerUpCooldowns.end() || massHealCooldown->second >= _spellDatabase.getSpellById(2).cooldown) {
      auto weight = 0;

      findEntities<Health>()->each([&weight](const Entity& entity) {
        auto health = entity.get<Health>();

        if (health->current < static_cast<int32_t>(glm::round(static_cast<float>(health->max) * 0.8f))) {
          weight += 10;
        }
      });

      weights[PowerUp::Type::MassHeal] = weight;
      total += weight;
    }
  }

  // Revive
  if (point->reviveEnabled) {
    auto reviveCooldown = point->powerUpCooldowns.find(4);

    if (reviveCooldown == point->powerUpCooldowns.end() || reviveCooldown->second >= _spellDatabase.getSpellById(4).cooldown) {
      auto weight = 0;

      findEntities<Health>()->each([&weight](const Entity& entity) {
        if (!entity.has<Alive>()) {
          weight += 30;
        }
      });

      weights[PowerUp::Type::Revive] = weight;
      total += weight;
    }
  }

  // Time Warp
  if (point->timeWarpEnabled) {
    auto isTravelingFast = false;

    findEntities<Player>()->each([&isTravelingFast](const Entity& entity) {
      auto player = entity.get<Player>();
      isTravelingFast = player->speed >= 15.0f;
    });

    if (isTravelingFast) {
      auto weight = 0;

      findEntities<Health>()->each([&weight](const Entity& entity) {
        auto health = entity.get<Health>();

        if (health->current < static_cast<int32_t>(glm::round(static_cast<float>(health->max) * 0.8f))) {
          weight += 10;
        }
      });

      weights[PowerUp::Type::TimeWarp] = weight;
      total += weight;
    }
  }

  if (total == 0) {
    return false;
  }

  auto powerUpEntity = createEntity();

  auto powerUp = powerUpEntity->add<PowerUp>();

  auto randomX = std::uniform_int_distribution(0, 2);

  auto transform = powerUpEntity->add<Transform>();
  transform->position = glm::vec3(0.0f, point->lastSpawnPoint, 1.0f);
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

  auto randomPowerUp = std::uniform_int_distribution(0, total);
  auto roll = randomPowerUp(_random);

  for (auto& option : weights) {
    if (roll < option.second) {
      powerUp->type = option.first;
      break;
    }

    roll -= option.second;
  }

  switch (powerUp->type) {
  case PowerUp::Type::MassHeal: {
    point->powerUpCooldowns[2] = 0.0f;

    auto drawable = powerUpEntity->add<Drawable>();
    drawable->feature = new ColoredFeature();
    drawable->feature->meshType = Plus;
    drawable->feature->color = Palette::Green;
    drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
    drawable.setRemovalListener([drawable](const Entity e) {
      drawable->renderable->destroy();
    });

    break;
  }
  case PowerUp::Type::Revive: {
    point->powerUpCooldowns[4] = 0.0f;

    auto drawable = powerUpEntity->add<Drawable>();
    drawable->feature = new ColoredFeature();
    drawable->feature->meshType = Quad;
    drawable->feature->color = { 0.0f, 0.0f, 1.0f, 1.0f };
    drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
    drawable.setRemovalListener([drawable](const Entity e) {
      drawable->renderable->destroy();
    });

    break;
  }
  case PowerUp::Type::SpeedBoost: {
    auto drawable = powerUpEntity->add<Drawable>();
    drawable->feature = new ColoredFeature();
    drawable->feature->meshType = Quad;
    drawable->feature->color = { 1.0f, 1.0f, 0.0f, 1.0f };
    drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
    drawable.setRemovalListener([drawable](const Entity e) {
      drawable->renderable->destroy();
    });

    break;
  }
  case PowerUp::Type::TimeWarp: {
    auto drawable = powerUpEntity->add<Drawable>();
    drawable->feature = new ColoredFeature();
    drawable->feature->meshType = Quad;
    drawable->feature->color = { 0.0f, 1.0f, 1.0f, 1.0f };
    drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
    drawable.setRemovalListener([drawable](const Entity e) {
      drawable->renderable->destroy();
    });

    break;
  }
  }

  return true;
}

void SpawnSystem::spawnAsteroid(float y, int size) {
  auto asteroidEntity = createEntity();

  auto asteroid = asteroidEntity->add<Asteroid>();

  if (size > 0) {
    asteroid->points = size;
  } else {
    auto randomSize = std::uniform_int_distribution(1, 5);
    asteroid->points = randomSize(_random);
  }

  findEntities<Score>()->each([asteroid](const Entity& entity) {
    auto score = entity.get<Score>();
    score->possiblePoints += asteroid->points;
  });

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
  circle->feature->color = { 0.97345f, 0.36625f, 0.00561f, 0.5f };
  circle->renderable = _renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
  circle.setRemovalListener([circle](const Entity e) {
    circle->renderable->destroy();
  });
}

void SpawnSystem::spawnObstacle(float x, float y) {
  auto obstacleEntity = createEntity();
  obstacleEntity->add<Bomb>();

  findEntities<Score>()->each([](const Entity& entity) {
    auto score = entity.get<Score>();
    ++score->possibleMines;
  });

  auto randomRotation = std::uniform_real_distribution(0.0f, glm::two_pi<float>());

  auto transform = obstacleEntity->add<Transform>();
  transform->scale = glm::vec3(1.25f);
  transform->position = glm::vec3(x, y, 1.0f);

  transform->rotation = glm::angleAxis(randomRotation(_random), glm::vec3(0.0f, 0.0f, 1.0f));

  obstacleEntity->add<PhysicalState>(transform->position, transform->rotation.z);
  obstacleEntity->add<CircleCollider>()->radius = 0.625f;
  obstacleEntity->add<Velocity>()->velocity = { 0.0f, -3.0f };

  auto drawable = obstacleEntity->add<Drawable>();
  drawable->feature = new ColoredFeature();
  drawable->feature->color = Palette::Red;
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

    auto randomX = std::uniform_real_distribution(-7.6f, 7.6f);

    auto randomOffset = std::uniform_real_distribution(-1.0f, 1.0f);

    auto transform = starEntity->add<Transform>();
    transform->position = { randomX(_random), y + randomOffset(_random), 10.0f };

    transform->scale = glm::vec3(0.1875f);

    starEntity->add<PhysicalState>(transform->position, 0.0f);
    starEntity->add<CircleCollider>()->radius = transform->scale.x / 2.0f;
    starEntity->add<Trigger>();

    auto randomSpeed = std::uniform_real_distribution(0.0f, -0.05f);
    starEntity->add<Velocity>()->velocity = { 0.0f, randomSpeed(_random) };

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
