#include "LivenessSystem.h"

#include "components/Alive.h"
#include "components/GridPosition.h"
#include "components/Health.h"

namespace linguine {

void LivenessSystem::update(float deltaTime) {
  findEntities<Alive, Health>()->each([this](Entity& entity) {
    auto health = entity.get<Health>();

    if (health->current <= 0) {
      if (entity.has<GridPosition>()) {
        auto gridPosition = entity.get<GridPosition>();

        if (gridPosition->transientDestination) {
          _grid.removeObstruction(*gridPosition->transientDestination, gridPosition->dimensions);
        } else {
          _grid.removeObstruction(glm::round(gridPosition->position), gridPosition->dimensions);
        }
      }

      entity.destroy();
    }
  });
}

}  // namespace linguine
