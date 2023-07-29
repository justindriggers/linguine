#include "EnemyTargetingSystem.h"

#include <glm/geometric.hpp>

#include "components/Alive.h"
#include "components/Attachment.h"
#include "components/Friendly.h"
#include "components/GridPosition.h"
#include "components/Hostile.h"
#include "components/Unit.h"

namespace linguine {

void EnemyTargetingSystem::fixedUpdate(float fixedDeltaTime) {
  auto friendlies = findEntities<Friendly, Alive, PhysicalState>()->get();

  findEntities<Hostile, Unit, Alive, PhysicalState, Targeting>()->each([this, &friendlies, fixedDeltaTime](Entity& entity) {
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
      moveTowardTarget(entity, targeting, physicalState, fixedDeltaTime);
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

void EnemyTargetingSystem::moveTowardTarget(Entity& entity,
                                            Component<Targeting>& targeting,
                                            Component<PhysicalState>& physicalState,
                                            float fixedDeltaTime) {
  auto targetId = *targeting->current;
  auto target = getEntityById(targetId);
  auto targetPosition = target->get<PhysicalState>()->currentPosition;

  auto raycasters = findEntities<Attachment, Raycaster, PhysicalState>()->get();

  for (const auto& raycasterEntity : raycasters) {
    auto raycaster = raycasterEntity->get<Raycaster>();
    auto raycasterPhysicalState = raycasterEntity->get<PhysicalState>();
    raycaster->direction = glm::normalize(targetPosition - raycasterPhysicalState->currentPosition);
  }

  auto isPlayerInSight = std::all_of(raycasters.begin(), raycasters.end(), [this](const std::shared_ptr<Entity>& raycasterEntity) {
    auto raycaster = raycasterEntity->get<Raycaster>();

    if (raycaster->nearest) {
      auto nearest = getEntityById(raycaster->nearest->entityId);
      return nearest->has<Friendly>();
    }

    return false;
  });

  const auto speed = 2.0f;

  if (isPlayerInSight) {
    if (entity.has<GridPosition>()) {
      entity.remove<GridPosition>();
    }

    auto direction = glm::normalize(targetPosition - physicalState->currentPosition);
    physicalState->currentPosition += direction * speed * fixedDeltaTime;
    physicalState->currentRotation = glm::atan(direction.y, direction.x) - glm::half_pi<float>();
  } else {
    if (!entity.has<GridPosition>()) {
      entity.add<GridPosition>();
    }

    auto gridPosition = entity.get<GridPosition>();
    gridPosition->position = _grid.getGridPosition(physicalState->currentPosition);
    gridPosition->speed = speed;
    gridPosition->finalDestination = glm::round(_grid.getGridPosition(targetPosition));
  }
}

}  // namespace linguine
