#include "PointAndClickMovementSystem.h"

#include "components/GridPosition.h"
#include "components/Player.h"
#include "components/Tapped.h"
#include "components/Tile.h"
#include "components/Transform.h"

namespace linguine {

void PointAndClickMovementSystem::update(float deltaTime) {
  findEntities<Tile, Tapped, Transform>()->each([this](const Entity& tileEntity) {
    auto transform = tileEntity.get<Transform>();

    findEntities<Player, GridPosition>()->each([this, &transform](const Entity& playerEntity) {
      auto gridPosition = playerEntity.get<GridPosition>();
      gridPosition->finalDestination = _grid.getGridPosition(transform->position);
    });
  });
}

}  // namespace linguine
