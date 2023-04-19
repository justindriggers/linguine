#include "GridPositionSystem.h"

#include <glm/gtx/norm.hpp>

#include "components/GridPosition.h"
#include "components/PhysicalState.h"
#include "components/Targeting.h"

namespace linguine {

void GridPositionSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<GridPosition, PhysicalState>()->each([this, fixedDeltaTime](const Entity& entity) {
    auto gridPosition = entity.get<GridPosition>();

    if (gridPosition->transientDestination) {
      auto difference = glm::vec2(*gridPosition->transientDestination) - gridPosition->position;
      auto frameDistance = gridPosition->speed * fixedDeltaTime;

      if (glm::length2(difference) <= frameDistance * frameDistance) {
        gridPosition->position = *gridPosition->transientDestination;
        gridPosition->transientDestination = {};
      } else {
        auto direction = glm::normalize(difference);
        gridPosition->position += direction * frameDistance;
      }
    }

    if (!gridPosition->transientDestination && gridPosition->finalDestination) {
      auto currentPosition = glm::round(gridPosition->position);
      auto path = _grid.search(gridPosition->position, *gridPosition->finalDestination);

      if (path.size() > 1) {
        auto newPosition = *std::next(path.begin());
        gridPosition->transientDestination = newPosition;

        _grid.removeObstruction(currentPosition, gridPosition->dimensions);
        _grid.addObstruction(newPosition, gridPosition->dimensions);
      } else {
        gridPosition->finalDestination = {};
      }
    }

    auto physicalState = entity.get<PhysicalState>();
    physicalState->currentPosition = _grid.getWorldPosition(gridPosition->position + glm::vec2(gridPosition->dimensions) / 2.0f - 0.5f);

    if (gridPosition->transientDestination) {
      auto direction = glm::normalize(glm::vec2(*gridPosition->transientDestination) - gridPosition->position);
      physicalState->currentRotation = glm::atan(direction.y, direction.x) - glm::half_pi<float>();
    } else if (entity.has<Targeting>()) {
      auto target = entity.get<Targeting>();

      if (target->current) {
        auto targetEntity = getEntityById(*target->current);

        if (targetEntity->has<GridPosition>()) {
          auto targetGridPosition = targetEntity->get<GridPosition>();
          auto direction = glm::normalize(targetGridPosition->position - gridPosition->position);
          physicalState->currentRotation = glm::atan(direction.y, direction.x) - glm::half_pi<float>();
        }
      }
    }
  });
}

}  // namespace linguine
