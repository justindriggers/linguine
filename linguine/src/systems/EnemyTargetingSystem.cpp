#include "EnemyTargetingSystem.h"

#include "components/Alive.h"
#include "components/Friendly.h"
#include "components/Hostile.h"
#include "components/Unit.h"

namespace linguine {

void EnemyTargetingSystem::fixedUpdate(float fixedDeltaTime) {
  auto friendlies = findEntities<Friendly, Alive, GridPosition>()->get();

  findEntities<Hostile, Unit, Alive, GridPosition, Targeting>()->each([this, &friendlies](const Entity& entity) {
    auto targeting = entity.get<Targeting>();
    auto gridPosition = entity.get<GridPosition>();

    if (!gridPosition->transientDestination) {
      if (targeting->current) {
        clearTargetIfDead(targeting);
      }

      if (friendlies.empty()) {
        return;
      }

      selectTarget(targeting, gridPosition, friendlies);

      if (targeting->current) {
        moveTowardTarget(targeting, gridPosition);
      }
    }
  });
}

void EnemyTargetingSystem::clearTargetIfDead(Component<Targeting>& targeting) {
  auto targetId = *targeting->current;
  auto target = getEntityById(targetId);

  if (!target->has<Alive>()) {
    targeting->current = {};
  }
}

void EnemyTargetingSystem::selectTarget(Component<Targeting>& targeting,
                                        Component<GridPosition>& gridPosition,
                                        const std::vector<std::shared_ptr<Entity>>& availableTargets) {
  switch (targeting->strategy) {
    case Targeting::Random: {
      if (!targeting->current) {
        auto randomEntity = std::uniform_int_distribution<>(0, static_cast<int>(availableTargets.size() - 1));
        const auto targetIndex = randomEntity(_random);
        const auto& target = availableTargets[targetIndex];
        targeting->current = target->getId();
      }
      break;
    }
    case Targeting::Nearest: {
      auto nearest = INT_MAX;

      if (targeting->current) {
        auto target = getEntityById(*targeting->current);
        auto targetPosition = target->get<GridPosition>()->position;
        auto path = _grid.search(glm::round(gridPosition->position), targetPosition);

        if (!path.empty()) {
          nearest = static_cast<int>(path.size());
        }
      }

      for (const auto& target : availableTargets) {
        auto targetPosition = target->get<GridPosition>()->position;
        auto path = _grid.search(glm::round(gridPosition->position), targetPosition);

        if (!path.empty()) {
          auto distance = static_cast<int>(path.size());

          if (distance < nearest) {
            nearest = distance;
            targeting->current = target->getId();
          }
        }
      }
      break;
    }
  }
}

void EnemyTargetingSystem::moveTowardTarget(Component<Targeting>& targeting,
                                            Component<GridPosition>& gridPosition) {
  auto targetId = *targeting->current;
  auto target = getEntityById(targetId);
  auto targetPosition = target->get<GridPosition>()->position;

  auto currentPosition = glm::round(gridPosition->position);
  auto path = _grid.search(currentPosition, targetPosition);

  if (path.size() > 1) {
    auto newPosition = *std::next(path.begin());
    gridPosition->transientDestination = newPosition;

    _grid.removeObstruction(currentPosition, gridPosition->dimensions);
    _grid.addObstruction(newPosition, gridPosition->dimensions);
  }
}

}  // namespace linguine
