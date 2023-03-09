#include "SelectionDestructionSystem.h"

#include "components/LongPressed.h"

namespace linguine {

void SelectionDestructionSystem::update(float deltaTime) {
  findEntities<LongPressed>()->each([this](Entity& entity) {
    _audioManager.play(EffectType::Pop);
    entity.destroy();
  });
}

}  // namespace linguine
