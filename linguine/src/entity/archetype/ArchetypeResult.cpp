#include "ArchetypeResult.h"

#include "ArchetypeEntity.h"

namespace linguine::archetype {

void ArchetypeResult::each(const std::function<void(Entity&)>& function) const {
  for (const auto* archetype : _archetypes) {
    archetype->each([this, function](uint64_t id) {
      auto entity = ArchetypeEntity(_entityManager, id);
      function(entity);
    });
  }
}

std::vector<std::shared_ptr<Entity>> ArchetypeResult::get() const {
  auto results = std::vector<std::shared_ptr<Entity>>();

  for (const auto* archetype : _archetypes) {
    archetype->each([this, &results](uint64_t id) {
      results.push_back(std::make_shared<ArchetypeEntity>(_entityManager, id));
    });
  }

  return results;
}

}  // namespace linguine::archetype
