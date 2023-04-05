#include "LivenessSystem.h"

#include "components/Alive.h"
#include "components/GridPosition.h"
#include "components/Health.h"

namespace linguine {

void LivenessSystem::update(float deltaTime) {
  findEntities<Alive, Health, GridPosition>()->each([this](Entity& entity) {
    auto health = entity.get<Health>();

    if (health->current <= 0) {
      auto gridPosition = entity.get<GridPosition>();
      _grid.removeObstruction(glm::round(gridPosition->position), gridPosition->dimensions);
      entity.destroy();
    }
  });
}

}  // namespace linguine
