#include "UnitCreationSystem.h"

#include "components/Cooldown.h"
#include "components/Drawable.h"
#include "components/Selectable.h"
#include "components/Tapped.h"
#include "components/Tile.h"
#include "components/UnitSelector.h"

namespace linguine {

void UnitCreationSystem::update(float deltaTime) {
  findEntities<UnitSelector, Tapped, Cooldown>()->each([this](const Entity& tappedSelectorEntity) {
    auto cooldown = tappedSelectorEntity.get<Cooldown>();

    if (cooldown->elapsed < cooldown->total) {
      return;
    }

    findEntities<UnitSelector>()->each([this, &tappedSelectorEntity](const Entity& selectorEntity) {
      auto unitSelector = selectorEntity.get<UnitSelector>();

      if (selectorEntity.getId() == tappedSelectorEntity.getId()) {
        unitSelector->isSelected = !unitSelector->isSelected;

        findEntities<Tile, Drawable, Selectable>()->each([&unitSelector](const Entity& tileEntity) {
          auto drawable = tileEntity.get<Drawable>();
          auto selectable = tileEntity.get<Selectable>();

          if (unitSelector->isSelected) {
            drawable->feature->color = glm::vec3(0.06f, 0.06f, 0.06f);
            selectable->renderable->setEnabled(true);
          } else {
            drawable->feature->color = glm::vec3(0.54f, 0.75f, 0.04f);
            selectable->renderable->setEnabled(false);
          }
        });
      } else {
        unitSelector->isSelected = false;
      }
    });
  });
}

}  // namespace linguine
