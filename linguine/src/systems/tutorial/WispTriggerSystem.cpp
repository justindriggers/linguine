#include "WispTriggerSystem.h"

#include "components/Ability.h"
#include "components/Health.h"
#include "components/Hit.h"
#include "components/Party.h"
#include "components/Player.h"

namespace linguine {

void WispTriggerSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Player, Party>()->each([this](const Entity& playerEntity) {
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
  });
}

}  // namespace linguine
