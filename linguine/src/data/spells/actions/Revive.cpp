#include "Revive.h"

#include <glm/common.hpp>

#include "components/Alive.h"
#include "components/Health.h"

namespace linguine {

void Revive::execute(linguine::Entity& target) {
  auto dead = std::vector<uint64_t>();

  _entityManager.find<Health>()->each([&dead](const Entity& entity) {
    if (!entity.has<Alive>()) {
      dead.push_back(entity.getId());
    }
  });

  if (!dead.empty()) {
    auto randomEntity = std::uniform_int_distribution(0, static_cast<int>(dead.size() - 1));
    auto i = randomEntity(_random);

    auto entity = _entityManager.getById(dead[i]);
    auto health = entity->get<Health>();
    health->current = static_cast<int32_t>(glm::round(static_cast<float>(health->max) * 0.25f));
    entity->add<Alive>();
  }
}

}  // namespace linguine
