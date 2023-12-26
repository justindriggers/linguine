#include "TutorialSystem.h"

#include "components/GlobalCooldown.h"
#include "components/Player.h"
#include "components/Score.h"
#include "components/TutorialState.h"

namespace linguine {

void TutorialSystem::update(float deltaTime) {
  findEntities<TutorialState>()->each([this](const Entity& tutorialStateEntity) {
    auto tutorialState = tutorialStateEntity.get<TutorialState>();

    switch (tutorialState->currentState) {
    case TutorialState::State::WaitingForMovement:
      findEntities<Player>()->each([&tutorialState](const Entity& playerEntity) {
        auto player = playerEntity.get<Player>();

        if (player->state != Player::Center) {
          tutorialState->hasMoved = true;
        }
      });
      break;
    case TutorialState::State::WaitingForScore:
      findEntities<Score>()->each([&tutorialState](const Entity& scoreEntity) {
        auto score = scoreEntity.get<Score>();

        if (score->points > 0) {
          tutorialState->hasScored = true;
        }
      });
      break;
    case TutorialState::State::WaitingForHeal:
      findEntities<GlobalCooldown>()->each([&tutorialState](const Entity& globalCooldownEntity) {
        auto globalCooldown = globalCooldownEntity.get<GlobalCooldown>();

        if (globalCooldown->remaining > 0.0f) {
          tutorialState->hasHealed = true;
        }
      });
      break;
    default:
      break;
    }
  });
}

}  // namespace linguine
