#include "PlayerTargetingSystem.h"

#include "components/Alive.h"
#include "components/Hostile.h"
#include "components/Player.h"

namespace linguine {

void PlayerTargetingSystem::fixedUpdate(float fixedDeltaTime) {
  auto enemies = findEntities<Hostile, Alive, GridPosition>()->get();

  findEntities<Player, Alive, GridPosition, Targeting>()->each([this, &enemies](const Entity& entity) {
    auto targeting = entity.get<Targeting>();
    auto gridPosition = entity.get<GridPosition>();

    if (targeting->current) {
      clearTargetIfDead(targeting);
    }

    if (enemies.empty()) {
      return;
    }

    selectTarget(targeting, gridPosition, enemies);
  });
}

void PlayerTargetingSystem::clearTargetIfDead(Component<Targeting>& targeting) {
  auto targetId = *targeting->current;
  auto target = getEntityById(targetId);

  if (!target->has<Alive>()) {
    targeting->current = {};
  }
}

void PlayerTargetingSystem::selectTarget(Component<Targeting>& targeting,
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

}  // namespace linguine
