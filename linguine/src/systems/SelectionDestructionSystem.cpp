#include "SelectionDestructionSystem.h"

#include "components/LongPressed.h"

namespace linguine {

void SelectionDestructionSystem::update(float deltaTime) {
  findEntities<LongPressed>()->each([](Entity& entity) {
    entity.destroy();
  });
}

}  // namespace linguine
