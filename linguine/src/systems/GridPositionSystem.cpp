#include "GridPositionSystem.h"

#include <glm/gtx/norm.hpp>

#include "components/GridPosition.h"
#include "components/PhysicalState.h"

namespace linguine {

void GridPositionSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<GridPosition, PhysicalState>()->each([this, fixedDeltaTime](const Entity& entity) {
    auto gridPosition = entity.get<GridPosition>();

    if (gridPosition->destination) {
      auto difference = glm::vec2(*gridPosition->destination) - gridPosition->position;
      auto frameDistance = gridPosition->speed * fixedDeltaTime;

      if (glm::length2(difference) <= frameDistance * frameDistance) {
        gridPosition->position = *gridPosition->destination;
        gridPosition->destination = {};
      } else {
        auto direction = glm::normalize(difference);
        gridPosition->position += direction * frameDistance;
      }
    }

    auto physicalState = entity.get<PhysicalState>();
    physicalState->currentPosition = _grid.getWorldPosition(gridPosition->position);
  });
}

}  // namespace linguine
