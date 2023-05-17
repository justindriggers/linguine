#include "TileSelectionSystem.h"

#include "components/Alive.h"
#include "components/CircleCollider.h"
#include "components/Cooldown.h"
#include "components/Drawable.h"
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
#include "components/UnitSelector.h"

namespace linguine {

void TileSelectionSystem::update(float deltaTime) {
  findEntities<Tile, Tapped>()->each([this](Entity& entity) {
    auto spawnUnit = entity.add<SpawnUnit>();

    findEntities<UnitSelector, Cooldown>()->each([&spawnUnit](const Entity& selectorEntity) {
      auto unitSelector = selectorEntity.get<UnitSelector>();

      if (unitSelector->isSelected) {
        unitSelector->isSelected = false;

        auto cooldown = selectorEntity.get<Cooldown>();
        cooldown->elapsed = 0.0f;

        spawnUnit->type = unitSelector->type;
      }
    });

    findEntities<Tile, Drawable, Selectable>()->each([](const Entity& tileEntity) {
      auto drawable = tileEntity.get<Drawable>();
      drawable->feature->color = glm::vec3(0.54f, 0.75f, 0.04f);

      auto selectable = tileEntity.get<Selectable>();
      selectable->renderable->setEnabled(false);
    });
  });

  findEntities<Transform, SpawnUnit>()->each([this](Entity& entity) {
    auto spawnUnit = entity.get<SpawnUnit>();
    auto unitType = spawnUnit->type;

    auto position = entity.get<Transform>()->position;
    entity.remove<SpawnUnit>();

    switch (unitType) {
      case Default: {
        auto unitEntity = createEntity();
        unitEntity->add<Friendly>();

        auto transform = unitEntity->add<Transform>();
        transform->position = glm::vec3(position.x, position.y, 1.0f);
        transform->scale = glm::vec3(2.0f);

        unitEntity->add<Unit>();

        auto physicalState = unitEntity->add<PhysicalState>();
        physicalState->previousPosition = glm::vec2(transform->position);
        physicalState->currentPosition = physicalState->previousPosition;

        auto gridPosition = unitEntity->add<GridPosition>();
        gridPosition->position = _grid.getGridPosition(position);
        gridPosition->dimensions = { 2, 2 };

        _grid.addObstruction(gridPosition->position, gridPosition->dimensions);

        auto collider = unitEntity->add<CircleCollider>();
        collider->radius = 1.0f;

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

        unitEntity->add<Health>(2'500);
        unitEntity->add<Alive>();
        break;
      }
    }
  });
}

}  // namespace linguine
