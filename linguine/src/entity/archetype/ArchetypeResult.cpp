#include "ArchetypeResult.h"

#include "ArchetypeEntity.h"

namespace linguine::archetype {

void ArchetypeResult::each(std::function<void(Entity&)> function) {
  for (const auto* archetype : _archetypes) {
    for (const auto& id : archetype->getEntityIds()) {
      if (id == Archetype::InvalidEntityId) {
        continue;
      }

      auto entity = ArchetypeEntity(_entityManager, id);
      function(entity);
    }
  }
}

std::vector<std::shared_ptr<Entity>> ArchetypeResult::get() {
  auto results = std::vector<std::shared_ptr<Entity>>();

  for (const auto* archetype : _archetypes) {
    for (const auto& id : archetype->getEntityIds()) {
      if (id == Archetype::InvalidEntityId) {
        continue;
      }

      results.push_back(std::make_shared<ArchetypeEntity>(_entityManager, id));
    }
  }

  return results;
}

}  // namespace linguine::archetype
