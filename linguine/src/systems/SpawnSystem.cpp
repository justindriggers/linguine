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
      case TutorialState::State::WaitingForScore:
        if (tutorialState->hasScored) {
          tutorialState->currentState = TutorialState::State::WaitingForHeal;
        }
        break;
      case TutorialState::State::WaitingForHeal:
        if (tutorialState->hasHealed) {
          tutorialState->currentState = TutorialState::State::Finished;
        }
        break;
      case TutorialState::State::Finished:
        if (tutorialState->hasFinished) {
          tutorialStateEntity.destroy();

          auto spawnPoint = entity.get<SpawnPoint>();
          auto physicalState = entity.get<PhysicalState>();

          spawnPoint->lastSpawnPoint = physicalState->currentPosition.y - spawnPoint->distance;
        }
        return;
      }

      if (tutorialState->elapsed >= 4.0f) {
        auto playerPosition = 0;

        findEntities<Player>()->each([&playerPosition](const Entity& playerEntity) {
          auto player = playerEntity.get<Player>();

          switch (player->state) {
          case Player::Left:
          case Player::CenterToLeft:
            playerPosition = 0;
            break;
          case Player::Center:
          case Player::LeftToCenter:
          case Player::RightToCenter:
            playerPosition = 1;
            break;
          case Player::Right:
          case Player::CenterToRight:
            playerPosition = 2;
            break;
          }
        });

        auto randomPosition = std::uniform_int_distribution(0, 2);
        int position;

        do {
          position = randomPosition(_random);
        } while (position == playerPosition);

        auto randomSize = std::uniform_int_distribution(3, 5);
        spawnAsteroid(entity.get<PhysicalState>()->currentPosition.y, randomSize(_random), position);
        tutorialState->elapsed = 0.0f;
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
    drawable->feature->meshType = MeshType::Plus;
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
    drawable->feature->meshType = MeshType::Revive;
    drawable->feature->color = Palette::Yellow;
    drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
    drawable.setRemovalListener([drawable](const Entity e) {
      drawable->renderable->destroy();
    });

    break;
  }
  case PowerUp::Type::SpeedBoost: {
    auto drawable = powerUpEntity->add<Drawable>();
    drawable->feature = new ColoredFeature();
    drawable->feature->meshType = MeshType::SpeedBoost;
    drawable->feature->color = Palette::Yellow;
    drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
    drawable.setRemovalListener([drawable](const Entity e) {
      drawable->renderable->destroy();
    });

    break;
  }
  case PowerUp::Type::TimeWarp: {
    auto drawable = powerUpEntity->add<Drawable>();
    drawable->feature = new ColoredFeature();
    drawable->feature->meshType = MeshType::TimeWarp;
    drawable->feature->color = Palette::Cyan;
    drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
    drawable.setRemovalListener([drawable](const Entity e) {
      drawable->renderable->destroy();
    });

    break;
  }
  }

  return true;
}

void SpawnSystem::spawnAsteroid(float y, int size, int position) {
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

  auto transform = asteroidEntity->add<Transform>();
  transform->position = glm::vec3(0.0f, y, 1.0f);

  if (position < 0) {
    auto randomX = std::uniform_int_distribution(0, 2);
    position = randomX(_random);
  }

  switch (position) {
  case 0:
    transform->position.x -= 4.0f;
    break;
  case 2:
    transform->position.x += 4.0f;
    break;
  default:
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
  drawable->feature->meshType = MeshType::Mine;
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

}  // namespace linguine
