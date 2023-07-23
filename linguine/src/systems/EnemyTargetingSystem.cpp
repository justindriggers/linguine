#include "EnemyTargetingSystem.h"

#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

#include "components/Alive.h"
#include "components/Friendly.h"
#include "components/Hostile.h"
#include "components/Unit.h"

namespace linguine {

void EnemyTargetingSystem::fixedUpdate(float fixedDeltaTime) {
  auto friendlies = findEntities<Friendly, Alive, PhysicalState>()->get();

  findEntities<Hostile, Unit, Alive, PhysicalState, Targeting>()->each([this, &friendlies, fixedDeltaTime](const Entity& entity) {
    auto targeting = entity.get<Targeting>();
    auto physicalState = entity.get<PhysicalState>();

    if (targeting->current) {
      clearTargetIfDead(targeting);
    }

    if (friendlies.empty()) {
      return;
    }

    selectTarget(targeting, physicalState, friendlies);

    if (targeting->current) {
      moveTowardTarget(targeting, physicalState, fixedDeltaTime);
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
                                        Component<PhysicalState>& physicalState,
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
      auto nearest = 9999.0f;

      if (targeting->current) {
        auto target = getEntityById(*targeting->current);
        auto targetPosition = target->get<PhysicalState>()->currentPosition;

        nearest = glm::distance(physicalState->currentPosition, targetPosition);
      }

      for (const auto& target : availableTargets) {
        auto targetPosition = target->get<PhysicalState>()->currentPosition;

        auto distance = glm::distance(physicalState->currentPosition, targetPosition);

        if (distance < nearest) {
          nearest = distance;
          targeting->current = target->getId();
        }
      }
      break;
    }
    case Targeting::Adjacent: {
      throw std::runtime_error("Unsupported targeting mode");
    }
  }
}

void EnemyTargetingSystem::moveTowardTarget(Component<Targeting>& targeting,
                                            Component<PhysicalState>& physicalState,
                                            float fixedDeltaTime) {
  auto targetId = *targeting->current;
  auto target = getEntityById(targetId);
  auto targetPosition = target->get<PhysicalState>()->currentPosition;

  auto direction = glm::normalize(targetPosition - physicalState->currentPosition);
  physicalState->currentPosition += direction * fixedDeltaTime;
  physicalState->currentRotation = glm::atan(direction.y, direction.x) - glm::half_pi<float>();
}

}  // namespace linguine
