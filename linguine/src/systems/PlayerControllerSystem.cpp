#include "PlayerControllerSystem.h"

#include <glm/gtx/compatibility.hpp>

#include "components/Ability.h"
#include "components/Alive.h"
#include "components/CameraFixture.h"
#include "components/Cast.h"
#include "components/GameOver.h"
#include "components/GlobalCooldown.h"
#include "components/HealthBar.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/Tapped.h"
#include "components/Transform.h"

namespace linguine {

void PlayerControllerSystem::update(float deltaTime) {
  findEntities<Player>()->each([this](const Entity& entity) {
    if (entity.has<GameOver>()) {
      return;
    }

    auto player = entity.get<Player>();

    switch (player->state) {
    case Player::Left:
      if (_inputManager.isKeyPressed(Key::D)
          || _inputManager.isSwipeDetected(InputManager::Direction::Right)) {
        player->state = Player::LeftToCenter;
      }
      break;
    case Player::Center:
      if ((_inputManager.isKeyPressed(Key::D) && !_inputManager.isKeyPressed(Key::A))
          || _inputManager.isSwipeDetected(InputManager::Direction::Right)) {
        player->state = Player::CenterToRight;
      } else if ((_inputManager.isKeyPressed(Key::A) && !_inputManager.isKeyPressed(Key::D))
                 || _inputManager.isSwipeDetected(InputManager::Direction::Left)) {
        player->state = Player::CenterToLeft;
      }
      break;
    case Player::Right:
      if (_inputManager.isKeyPressed(Key::A)
          || _inputManager.isSwipeDetected(InputManager::Direction::Left)) {
        player->state = Player::RightToCenter;
      }
      break;
    default:
      break;
    }
  });

  findEntities<HealthBar, Tapped, Transform>()->each([this](Entity& healthBarEntity) {
    auto healthBar = healthBarEntity.get<HealthBar>();

    findEntities<Cast>()->each([this, &healthBar](const Entity& castEntity) {
      auto cast = castEntity.get<Cast>();
      auto isCanceled = true;

      findEntities<Ability>()->each([this, &cast, &healthBar, &isCanceled](const Entity& abilityEntity) {
        auto ability = abilityEntity.get<Ability>();

        if (!cast->abilityEntityId && ability->remainingCooldown <= 0.0f) {
          auto target = getEntityById(healthBar->entityId);

          if (target->has<Alive>()) {
            findEntities<GlobalCooldown>()->each([&cast, &healthBar, &isCanceled, &abilityEntity](const Entity& entity) {
              auto globalCooldown = entity.get<GlobalCooldown>();

              if (globalCooldown->remaining <= 0.0f) {
                cast->abilityEntityId = abilityEntity.getId();
                cast->targetEntityId = healthBar->entityId;
                globalCooldown->remaining = globalCooldown->total;
                isCanceled = false;
              }
            });
          }
        } else if (cast->abilityEntityId == abilityEntity.getId()) {
          isCanceled = false;
        }
      });

      if (cast->abilityEntityId && isCanceled) {
        cast->abilityEntityId = {};
        cast->targetEntityId = {};
        cast->elapsed = 0.0f;

        findEntities<GlobalCooldown>()->each([](const Entity& entity) {
          auto globalCooldown = entity.get<GlobalCooldown>();
          globalCooldown->remaining = 0.0f;
        });
      }
    });
  });
}

void PlayerControllerSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Player, PhysicalState>()->each([fixedDeltaTime](const Entity& entity) {
    auto player = entity.get<Player>();
    auto physicalState = entity.get<PhysicalState>();

    switch (player->state) {
    case Player::Left:
      physicalState->currentPosition.x = -4.0f;
      break;
    case Player::Center:
      physicalState->currentPosition.x = 0.0f;
      break;
    case Player::Right:
      physicalState->currentPosition.x = 4.0f;
      break;
    case Player::LeftToCenter:
      player->lerp = glm::clamp(player->lerp + 10.0f * fixedDeltaTime, 0.0f, 1.0f);
      physicalState->currentPosition.x = glm::lerp(-4.0f, 0.0f, player->lerp);

      if (player->lerp >= 1.0f) {
        player->state = Player::Center;
        player->lerp = 0.0f;
      }
      break;
    case Player::CenterToRight:
      player->lerp = glm::clamp(player->lerp + 10.0f * fixedDeltaTime, 0.0f, 1.0f);
      physicalState->currentPosition.x = glm::lerp(0.0f, 4.0f, player->lerp);

      if (player->lerp >= 1.0f) {
        player->state = Player::Right;
        player->lerp = 0.0f;
      }
      break;
    case Player::RightToCenter:
      player->lerp = glm::clamp(player->lerp + 10.0f * fixedDeltaTime, 0.0f, 1.0f);
      physicalState->currentPosition.x = glm::lerp(4.0f, 0.0f, player->lerp);

      if (player->lerp >= 1.0f) {
        player->state = Player::Center;
        player->lerp = 0.0f;
      }
      break;
    case Player::CenterToLeft:
      player->lerp = glm::clamp(player->lerp + 10.0f * fixedDeltaTime, 0.0f, 1.0f);
      physicalState->currentPosition.x = glm::lerp(0.0f, -4.0f, player->lerp);

      if (player->lerp >= 1.0f) {
        player->state = Player::Left;
        player->lerp = 0.0f;
      }
      break;
    }
  });
}

}  // namespace linguine
