#include "TileSelectionSystem.h"

#include "components/Alive.h"
#include "components/CircleCollider.h"
#include "components/Friendly.h"
#include "components/GridPosition.h"
#include "components/Health.h"
#include "components/PhysicalState.h"
#include "components/Progressable.h"
#include "components/Selectable.h"
#include "components/SpawnUnit.h"
#include "components/Tapped.h"
#include "components/Tile.h"
#include "components/Transform.h"
#include "components/Unit.h"

namespace linguine {

void TileSelectionSystem::update(float deltaTime) {
  findEntities<Tile, Tapped>()->each([](Entity& entity) {
    entity.add<SpawnUnit>();
  });

  findEntities<Transform, SpawnUnit>()->each([this](Entity& entity) {
    auto position = entity.get<Transform>()->position;
    entity.destroy();

    auto unitEntity = createEntity();
    unitEntity->add<Friendly>();

    auto transform = unitEntity->add<Transform>();
    transform->position = glm::vec3(position.x, position.y, 1.0f);

    unitEntity->add<Unit>();

    auto physicalState = unitEntity->add<PhysicalState>();
    physicalState->previousPosition = glm::vec2(transform->position);
    physicalState->currentPosition = physicalState->previousPosition;

    auto gridPosition = unitEntity->add<GridPosition>();
    gridPosition->position = _grid.getGridPosition(position);

    _grid.addObstruction(gridPosition->position);

    unitEntity->add<CircleCollider>();

    auto progressable = unitEntity->add<Progressable>();
    progressable->feature = new ProgressFeature();
    progressable->feature->meshType = Quad;
    progressable->renderable = _renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature));
    progressable.setRemovalListener([progressable](const Entity e) {
      progressable->renderable->destroy();
    });

    auto selectable = unitEntity->add<Selectable>();
    selectable->feature = new SelectableFeature();
    selectable->feature->entityId = unitEntity->getId();
    selectable->feature->meshType = Quad;
    selectable->renderable = _renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature));
    selectable.setRemovalListener([selectable](const Entity e) {
      selectable->renderable->destroy();
    });

    auto health = unitEntity->add<Health>();
    health->current = 1'000;
    health->max = 1'000;

    unitEntity->add<Alive>();
  });
}

}  // namespace linguine
