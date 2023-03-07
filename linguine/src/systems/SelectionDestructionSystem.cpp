#include "SelectionDestructionSystem.h"

#include "components/LongPressed.h"

namespace linguine {

void SelectionDestructionSystem::update(float deltaTime) {
  findEntities<LongPressed>()->each([this](Entity& entity) {
    _audioManager.play();
    entity.destroy();
  });
}

}  // namespace linguine
