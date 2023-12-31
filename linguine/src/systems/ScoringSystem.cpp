#include "ScoringSystem.h"

#include "components/Alive.h"
#include "components/Asteroid.h"
#include "components/Bomb.h"
#include "components/CameraFixture.h"
#include "components/Circle.h"
#include "components/Emitter.h"
#include "components/Health.h"
#include "components/Hit.h"
#include "components/Particle.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/PowerUp.h"
#include "components/Score.h"
#include "components/Shake.h"
#include "components/Text.h"
#include "components/Toast.h"
#include "components/Transform.h"
#include "components/Velocity.h"

namespace linguine {

void ScoringSystem::update(float deltaTime) {
  findEntities<Score, Text>()->each([](const Entity& entity) {
    auto score = entity.get<Score>();
    auto text = entity.get<Text>();

    text->feature->text = std::to_string(score->points);
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
          score->points += asteroid->points;

          auto living = findEntities<Health, Alive>()->get();
          auto distribution = std::uniform_int_distribution(0, static_cast<int>(living.size() - 1));

          auto randomIndex = distribution(_random);
          auto partyMember = living[randomIndex];

          auto health = partyMember->get<Health>();
          health->current = glm::clamp(health->current - 50 * asteroid->points, 0, health->max);

          auto magnitude = 0.05f * static_cast<float>(asteroid->points);

          findEntities<CameraFixture, Shake>()->each([magnitude](const Entity& entity) {
            auto shake = entity.get<Shake>();
            shake->magnitude += magnitude;
            shake->duration += 0.5f;
          });

          auto asteroidTransform = hitEntity->get<Transform>();

          findEntities<Toast, Text>()->each([&asteroid, &asteroidTransform](const Entity& entity) {
            auto toast = entity.get<Toast>();
            toast->elapsed = 0.0f;
            toast->startPosition.x = asteroidTransform->position.x * 20.0f;

            auto text = entity.get<Text>();
            text->feature->text = "+" + std::to_string(asteroid->points);
          });

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
            circle->feature->color = { 0.97345f, 0.36625f, 0.00561f };
            circle->renderable = _renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
            circle.setRemovalListener([circle](const Entity e) {
              circle->renderable->destroy();
            });
          }

          hitEntity->destroy();
        } else if (hitEntity->has<PowerUp>()) {
          auto powerUp = hitEntity->get<PowerUp>();

          findEntities<CameraFixture, Shake>()->each([](const Entity& entity) {
            auto shake = entity.get<Shake>();
            shake->magnitude += 0.1f;
            shake->duration += 0.25f;
          });

          switch (powerUp->type) {
          case PowerUp::MassHeal:
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
              circle->feature->color = { 0.0f, 1.0f, 0.0f };
              circle->renderable = _renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
              circle.setRemovalListener([circle](const Entity e) {
                circle->renderable->destroy();
              });
            }
            break;
          }

          hitEntity->destroy();
        } else if (hitEntity->has<Bomb>()) {
          _spellDatabase.getSpellById(3).action->execute(playerEntity);

          findEntities<CameraFixture, Shake>()->each([](const Entity& entity) {
            auto shake = entity.get<Shake>();
            shake->magnitude += 0.5f;
            shake->duration += 0.65f;
          });

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
              circle->feature->color = { 1.0f, 0.0f, 0.0f };
              particle->scalePerSecond = -2.0f;
              break;
            case 1:
              circle->feature->color = { 0.97345f, 0.36625f, 0.00561f };
              particle->scalePerSecond = -2.0f;
              break;
            case 2:
              circle->feature->color = { 0.78354f, 0.78354f, 0.78354f };
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
