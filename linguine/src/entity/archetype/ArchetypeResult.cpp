#include "ArchetypeResult.h"

namespace linguine::archetype {

void ArchetypeResult::each(const std::function<void(Entity&)>& function) const {
  auto results = std::vector<Entity>();

  for (const auto* archetype : _archetypes) {
    archetype->each([this, &results](uint64_t id) {
      results.emplace_back(_entityManager, id);
    });
  }

  for (auto& entity : results) {
    function(entity);
  }
}

std::vector<std::shared_ptr<Entity>> ArchetypeResult::get() const {
  auto results = std::vector<std::shared_ptr<Entity>>();

  for (const auto* archetype : _archetypes) {
    archetype->each([this, &results](uint64_t id) {
      results.push_back(std::make_shared<Entity>(_entityManager, id));
    });
  }

  return results;
}

}  // namespace linguine::archetype
