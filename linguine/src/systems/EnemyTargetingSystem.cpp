#include "EnemyTargetingSystem.h"

#include "components/Alive.h"
#include "components/Dead.h"
#include "components/Friendly.h"
#include "components/Hostile.h"
#include "components/Unit.h"

namespace linguine {

void EnemyTargetingSystem::fixedUpdate(float fixedDeltaTime) {
  auto friendlies = findEntities<Friendly, Alive, GridPosition>()->get();

  findEntities<Hostile, Unit, Alive, GridPosition, Targeting>()->each([this, &friendlies](const Entity& entity) {
    auto targeting = entity.get<Targeting>();
    auto gridPosition = entity.get<GridPosition>();

    if (!gridPosition->destination) {
      if (targeting->current) {
        clearTargetIfDead(targeting);
      }

      if (friendlies.empty()) {
        return;
      }

      if (!targeting->current) {
        selectNewTarget(targeting, friendlies);
      }

      moveTowardTarget(targeting, gridPosition);
    }
  });
}

void EnemyTargetingSystem::clearTargetIfDead(Component<Targeting>& targeting) {
  auto targetId = *targeting->current;
  auto target = getEntityById(targetId);

  if (target->has<Dead>()) {
    targeting->current = {};
  }
}

void EnemyTargetingSystem::selectNewTarget(Component<Targeting>& targeting,
                                           const std::vector<std::shared_ptr<Entity>>& availableTargets) {
  auto randomEntity = std::uniform_int_distribution<>(0, static_cast<int>(availableTargets.size() - 1));

  switch (targeting->strategy) {
    case Targeting::Random: {
      const auto targetIndex = randomEntity(_random);
      const auto& target = availableTargets[targetIndex];
      targeting->current = target->getId();
      break;
    }
  }
}

void EnemyTargetingSystem::moveTowardTarget(Component<Targeting>& targeting,
                                            Component<GridPosition>& gridPosition) {
  auto targetId = *targeting->current;
  auto target = getEntityById(targetId);
  auto targetPosition = target->get<GridPosition>()->position;

  auto path = _grid.search(glm::round(gridPosition->position), targetPosition);

  if (path.size() > 1) {
    gridPosition->destination = *std::next(path.begin());
  }
}

}  // namespace linguine
