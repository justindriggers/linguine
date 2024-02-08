#include "ScoringSystem.h"

#include "components/Alive.h"
#include "components/Asteroid.h"
#include "components/Attachment.h"
#include "components/Bomb.h"
#include "components/Boost.h"
#include "components/Circle.h"
#include "components/Emitter.h"
#include "components/Health.h"
#include "components/Hit.h"
#include "components/Level.h"
#include "components/LifeIndicator.h"
#include "components/Lives.h"
#include "components/Particle.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/PointIndicator.h"
#include "components/PowerUp.h"
#include "components/Score.h"
#include "components/Shake.h"
#include "components/Text.h"
#include "components/TimeWarp.h"
#include "components/Toast.h"
#include "components/Transform.h"
#include "components/Velocity.h"
#include "data/Palette.h"
#include "data/upgrades/LevelCurve.h"

namespace linguine {

void ScoringSystem::update(float deltaTime) {
  findEntities<Score, Text, Attachment>()->each([this](const Entity& entity) {
    auto score = entity.get<Score>();
    auto text = entity.get<Text>();

    text->feature->text = std::to_string(score->points);

    auto currentLevel = LevelCurve::getLevelForXp(score->points);

    findEntities<Level, Text>()->each([this, currentLevel](const Entity& levelEntity) {
      auto text = levelEntity.get<Text>();
      text->feature->text = std::to_string(currentLevel);

      auto level = levelEntity.get<Level>();

      auto textEntity = getEntityById(level->textId);
      auto textAttachment = textEntity->get<Attachment>();

      auto width = 21.0f + static_cast<float>(text->feature->text.size()) * 10.0f;

      auto backgroundEntity = getEntityById(level->backgroundId);
      auto backgroundAttachment = backgroundEntity->get<Attachment>();
      backgroundAttachment->offset.x = textAttachment->offset.x + width / 2.0f - 3.0f;

      auto backgroundTransform = backgroundEntity->get<Transform>();
      backgroundTransform->scale.x = width + 8.0f;
    });

    auto scoreTextPosition = 8.0f - (static_cast<float>(text->feature->text.size()) / 2.0f - 0.5f) * 10.0f;

    auto attachment = entity.get<Attachment>();
    attachment->offset.x = scoreTextPosition;

    auto iconEntity = getEntityById(score->iconId);
    auto iconAttachment = iconEntity->get<Attachment>();
    iconAttachment->offset.x = scoreTextPosition - 16.0f;

    auto width = 16.0f + static_cast<float>(text->feature->text.size()) * 10.0f;

    auto backgroundEntity = getEntityById(score->backgroundId);
    auto backgroundAttachment = backgroundEntity->get<Attachment>();
    backgroundAttachment->offset.x = iconAttachment->offset.x + width / 2.0f - 5.0f;

    auto backgroundTransform = backgroundEntity->get<Transform>();
    backgroundTransform->scale.x = width + 8.0f;
  });
}

void ScoringSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Score>()->each([this](const Entity& scoreEntity) {
    auto score = scoreEntity.get<Score>();

    findEntities<Player, Hit>()->each([this, &score](Entity& playerEntity) {
      auto hit = playerEntity.get<Hit>();

      for (const auto& hitEntityId : hit->entityIds) {
        auto hitEntity = getEntityById(hitEntityId);

        if (hitEntity->has<Asteroid>()) {
          auto asteroid = hitEntity->get<Asteroid>();
          auto extraLives = ((score->points + asteroid->points) / 1000) - (score->points / 1000);

          score->points += asteroid->points;

          if (extraLives > 0) {
            _audioManager.play(EffectType::Level);
          } else {
            switch (asteroid->points) {
            case 1:
              _audioManager.play(EffectType::Collect1);
              break;
            case 2:
              _audioManager.play(EffectType::Collect2);
              break;
            case 3:
              _audioManager.play(EffectType::Collect3);
              break;
            case 4:
              _audioManager.play(EffectType::Collect4);
              break;
            case 5:
              _audioManager.play(EffectType::Collect5);
              break;
            default:
              break;
            }
          }

          auto living = findEntities<Health, Alive>()->get();
          auto distribution = std::uniform_int_distribution(0, static_cast<int>(living.size() - 1));

          auto randomIndex = distribution(_random);
          auto partyMember = living[randomIndex];

          auto health = partyMember->get<Health>();
          health->current = glm::clamp(health->current - 50 * asteroid->points, 0, health->max);

          auto magnitude = 0.05f * static_cast<float>(asteroid->points);

          auto shakeEntity = createEntity();
          shakeEntity->add<Shake>(magnitude, 0.5f);

          auto asteroidTransform = hitEntity->get<Transform>();

          findEntities<PointIndicator, Toast, Text>()->each([this, &asteroid, &asteroidTransform](const Entity& entity) {
            auto toast = entity.get<Toast>();
            toast->elapsed = 0.0f;
            toast->startPosition.x = asteroidTransform->position.x * 240.0f / 15.2f;

            switch (_saveManager.getHandedness()) {
            case SaveManager::Handedness::Right:
              toast->startPosition.x -= 1.6f * 240.0f / 15.2f;
              break;
            case SaveManager::Handedness::Left:
              toast->startPosition.x += 1.6f * 240.0f / 15.2f;
              break;
            }

            auto text = entity.get<Text>();
            text->feature->text = "+" + std::to_string(asteroid->points);
          });

          if (extraLives > 0) {
            findEntities<Lives>()->each([extraLives](const Entity& entity) {
              auto lives = entity.get<Lives>();

              if (255 - lives->lives < extraLives) {
                lives->lives = 255;
              } else {
                lives->lives += extraLives;
              }
            });

            findEntities<LifeIndicator, Toast, Text>()->each([this, extraLives, &asteroidTransform](const Entity& entity) {
              auto toast = entity.get<Toast>();
              toast->elapsed = 0.0f;
              toast->startPosition.x = asteroidTransform->position.x * 240.0f / 15.2f;

              switch (_saveManager.getHandedness()) {
              case SaveManager::Handedness::Right:
                toast->startPosition.x -= 1.6f * 240.0f / 15.2f;
                break;
              case SaveManager::Handedness::Left:
                toast->startPosition.x += 1.6f * 240.0f / 15.2f;
                break;
              }

              auto text = entity.get<Text>();
              text->feature->text = std::to_string(extraLives) + "up";
            });
          }

          auto randomScale = std::uniform_real_distribution(0.1f, 0.25f);
          auto randomDirection = std::uniform_real_distribution(0.0f, glm::two_pi<float>());

          for (auto i = 0; i < 10; ++i) {
            auto particleEntity = createEntity();

            auto particle = particleEntity->add<Particle>();
            particle->duration = 0.5f;

            auto transform = particleEntity->add<Transform>();
            transform->scale = glm::vec3(randomScale(_random));
            transform->position = asteroidTransform->position;

            particleEntity->add<PhysicalState>(transform->position, 0.0f);

            auto velocity = particleEntity->add<Velocity>();
            velocity->velocity = glm::angleAxis(randomDirection(_random), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(1.0f, 0.0f, 0.0f) * 10.0f;

            auto circle = particleEntity->add<Circle>();
            circle->feature = new CircleFeature();
            circle->feature->color = Palette::Orange;
            circle->renderable = _renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
            circle.setRemovalListener([circle](const Entity e) {
              circle->renderable->destroy();
            });
          }

          hitEntity->destroy();
        } else if (hitEntity->has<PowerUp>()) {
          auto powerUp = hitEntity->get<PowerUp>();
          _audioManager.play(EffectType::PowerUp);

          auto shakeEntity = createEntity();
          shakeEntity->add<Shake>(0.1f, 0.25f);

          switch (powerUp->type) {
          case PowerUp::Type::MassHeal: {
            _spellDatabase.getSpellById(2).action->execute(playerEntity);

            auto powerUpTransform = hitEntity->get<Transform>();

            auto count = 10;

            for (auto i = 0; i < count; ++i) {
              auto particleEntity = createEntity();

              auto particle = particleEntity->add<Particle>();
              particle->duration = 5.0f;

              auto transform = particleEntity->add<Transform>();
              transform->scale = glm::vec3(0.5f);
              transform->position = powerUpTransform->position;

              particleEntity->add<PhysicalState>(transform->position, 0.0f);

              auto velocity = particleEntity->add<Velocity>();
              velocity->velocity = glm::angleAxis(glm::two_pi<float>() / static_cast<float>(count) * static_cast<float>(i), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(1.0f, 0.0f, 0.0f) * 2.5f;

              auto circle = particleEntity->add<Circle>();
              circle->feature = new CircleFeature();
              circle->feature->color = Palette::Green;
              circle->renderable = _renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
              circle.setRemovalListener([circle](const Entity e) {
                circle->renderable->destroy();
              });
            }
            break;
          }
          case PowerUp::Type::Revive: {
            _spellDatabase.getSpellById(4).action->execute(playerEntity);
            break;
          }
          case PowerUp::Type::SpeedBoost: {
            auto duration = 4.0f;

            findEntities<Player>()->each([duration](Entity& entity) {
              if (entity.has<Boost>()) {
                auto boost = entity.get<Boost>();
                boost->magnitude = 10.0f;
                boost->elapsed = 0.0f;
                boost->duration = duration;
              } else {
                auto boost = entity.add<Boost>();
                boost->magnitude = 10.0f;
                boost->duration = duration;
              }
            });

            auto boostShakeEntity = createEntity();
            boostShakeEntity->add<Shake>(0.15f, duration);

            break;
          }
          case PowerUp::Type::TimeWarp: {
            auto timeWarpEntity = createEntity();
            timeWarpEntity->add<TimeWarp>(0.5f, 1.0f);
            break;
          }
          }

          hitEntity->destroy();
        } else if (hitEntity->has<Bomb>()) {
          _spellDatabase.getSpellById(3).action->execute(playerEntity);
          _audioManager.play(EffectType::Detonate);

          ++score->mines;

          auto shakeEntity = createEntity();
          shakeEntity->add<Shake>(0.5f, 0.65f);

          auto bombTransform = hitEntity->get<Transform>();

          auto emitterEntity = createEntity();
          auto emitterEntityId = emitterEntity->getId();

          auto emitterTransform = emitterEntity->add<Transform>();
          emitterTransform->position = bombTransform->position;
          emitterTransform->position.z = 0.0f;

          auto emitter = emitterEntity->add<Emitter>([this, emitterEntityId]() {
            auto emitterEntity = getEntityById(emitterEntityId);
            auto emitterTransform = emitterEntity->get<Transform>();

            auto randomScale = std::uniform_real_distribution(0.5f, 0.75f);
            auto randomDirection = std::uniform_real_distribution(0.0f, glm::two_pi<float>());
            auto randomSpeed = std::uniform_real_distribution(3.5f, 4.0f);
            auto randomColor = std::uniform_int_distribution(0, 3);

            auto particleEntity = createEntity();

            auto particle = particleEntity->add<Particle>();
            particle->duration = 2.0f;
            particle->scalePerSecond = -0.5f;

            auto transform = particleEntity->add<Transform>();
            transform->scale = glm::vec3(randomScale(_random));
            transform->position = emitterTransform->position;

            particleEntity->add<PhysicalState>(transform->position, 0.0f);

            auto velocity = particleEntity->add<Velocity>();
            velocity->velocity = glm::angleAxis(randomDirection(_random), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(1.0f, 0.0f, 0.0f) * randomSpeed(_random);

            auto circle = particleEntity->add<Circle>();
            circle->feature = new CircleFeature();

            switch (randomColor(_random)) {
            case 0:
              circle->feature->color = Palette::Red;
              particle->scalePerSecond = -2.0f;
              break;
            case 1:
              circle->feature->color = Palette::Orange;
              particle->scalePerSecond = -2.0f;
              break;
            case 2:
              circle->feature->color = Palette::Gray;
              break;
            }

            circle->renderable = _renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
            circle.setRemovalListener([circle](const Entity e) {
              circle->renderable->destroy();
            });
          });
          emitter->frequency = 0.02f;
          emitter->totalLifetime = 0.5f;

          hitEntity->destroy();
        }
      }
    });
  });
}

}  // namespace linguine
