#include "WispTriggerSystem.h"

#include "components/Ability.h"
#include "components/Cast.h"
#include "components/Health.h"
#include "components/HealthBar.h"
#include "components/Hit.h"
#include "components/Hovered.h"
#include "components/Party.h"
#include "components/Player.h"
#include "components/Text.h"
#include "components/tutorial/TutorialState.h"
#include "components/tutorial/TutorialText1.h"
#include "components/tutorial/TutorialText2.h"

namespace linguine {

void WispTriggerSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<TutorialState>()->each([this](const Entity& tutorialStateEntity) {
    auto tutorialState = tutorialStateEntity.get<TutorialState>();

    if (tutorialState->stage == Stage::TeachTargeting) {
      findEntities<HealthBar, Hovered>()->each([&tutorialState](const Entity& healthBarEntity) {
        tutorialState->stage = Stage::TeachCasting;
      });
    }

    if (tutorialState->stage == Stage::TeachCasting) {
      findEntities<Cast>()->each([&tutorialState](const Entity& castEntity) {
        auto cast = castEntity.get<Cast>();

        if (cast->abilityEntityId) {
          tutorialState->stage = Stage::Completed;
        }
      });
    }

    bool isPartyEmpty = true;

    findEntities<Player, Party>()->each([this, &isPartyEmpty](const Entity& playerEntity) {
      auto party = playerEntity.get<Party>();
      party->memberIds.clear();

      findEntities<Hit, Ability, Health>()->each([&playerEntity, &party](const Entity& triggerEntity) {
        auto hit = triggerEntity.get<Hit>();

        for (auto entityId : hit->entityIds) {
          if (entityId == playerEntity.getId()) {
            party->memberIds.push_back(triggerEntity.getId());
          }
        }
      });

      isPartyEmpty = party->memberIds.empty();
    });

    findEntities<TutorialText1, Text>()->each([&tutorialState, &isPartyEmpty](const Entity& tutorialTextEntity) {
      auto text = tutorialTextEntity.get<Text>();
      text->renderable->setEnabled(tutorialState->stage == Stage::TeachTargeting && !isPartyEmpty);
    });

    findEntities<TutorialText2, Text>()->each([&tutorialState, &isPartyEmpty](const Entity& tutorialTextEntity) {
      auto text = tutorialTextEntity.get<Text>();
      text->renderable->setEnabled(tutorialState->stage == Stage::TeachCasting && !isPartyEmpty);
    });
  });
}

}  // namespace linguine
