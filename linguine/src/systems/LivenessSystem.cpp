#include "LivenessSystem.h"

#include "components/Alive.h"
#include "components/Attachment.h"
#include "components/CameraFixture.h"
#include "components/Circle.h"
#include "components/CircleCollider.h"
#include "components/EffectTracker.h"
#include "components/Emitter.h"
#include "components/GameOver.h"
#include "components/Health.h"
#include "components/Particle.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/Score.h"
#include "components/Shake.h"
#include "components/ShipPart.h"
#include "components/Transform.h"
#include "components/Velocity.h"
#include "scenes/GameOverScene.h"

namespace linguine {

void LivenessSystem::update(float deltaTime) {
  findEntities<Health, Alive>()->each([this](Entity& entity) {
    auto health = entity.get<Health>();

    if (health->current <= 0) {
      findEntities<EffectTracker>()->each([entity](Entity& effectTrackerEntity) {
        auto effectTracker = effectTrackerEntity.get<EffectTracker>();

        if (effectTracker->targetId == entity.getId()) {
          effectTrackerEntity.destroy();
        }
      });

      entity.remove<Alive>();
    }
  });

  auto living = findEntities<Health, Alive>()->get();

  if (living.empty()) {
    findEntities<Player>()->each([this](Entity& entity) {
      if (entity.has<CircleCollider>()) {
        entity.remove<CircleCollider>();
        _audioManager.play(EffectType::Explosion);
        _audioManager.stopSongs();

        auto player = entity.get<Player>();
        player->acceleration = player->speed / -4.0f;

        entity.add<GameOver>();

        findEntities<ShipPart>()->each([](Entity& entity) {
          entity.destroy();
        });

        findEntities<CameraFixture, Shake>()->each([](const Entity& entity) {
          auto shake = entity.get<Shake>();
          shake->magnitude += 0.5f;
          shake->duration += 1.0f;
        });

        auto emitterEntity = createEntity();
        auto emitterEntityId = emitterEntity->getId();

        emitterEntity->add<Transform>();
        emitterEntity->add<PhysicalState>();

        auto emitterAttachment = emitterEntity->add<Attachment>();
        emitterAttachment->parentId = entity.getId();

        auto emitter = emitterEntity->add<Emitter>([this, emitterEntityId]() {
          auto emitterEntity = getEntityById(emitterEntityId);
          auto emitterTransform = emitterEntity->get<Transform>();

          auto randomScale = std::uniform_real_distribution(0.5f, 0.75f);
          auto randomDirection = std::uniform_real_distribution(0.0f, glm::two_pi<float>());
          auto randomSpeed = std::uniform_real_distribution(4.5f, 5.0f);
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
        emitter->frequency = 0.025f;
        emitter->totalLifetime = 0.65f;
      }
    });

    findEntities<GameOver>()->each([this, deltaTime](const Entity& entity) {
      auto gameOver = entity.get<GameOver>();
      gameOver->elapsed += deltaTime;

      if (gameOver->elapsed >= gameOver->duration) {
        findEntities<Score>()->each([this](const Entity& entity) {
          auto score = entity.get<Score>();
          _serviceLocator.get<SceneManager>().load(std::make_unique<GameOverScene>(_serviceLocator, score->points));
        });
      }
    });
  }
}

}  // namespace linguine
