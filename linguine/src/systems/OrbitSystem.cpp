#include "OrbitSystem.h"

#include "components/Orbiter.h"
#include "components/Transform.h"

namespace linguine {

void OrbitSystem::update(float deltaTime) {
  auto orbiters = findEntities<Orbiter, Transform>()->get();

  if (!orbiters.empty()) {
    auto speed = 0.25f + 0.5f / static_cast<float>(orbiters.size());

    auto startAngle = _timeManager.runtimeInSeconds() * glm::two_pi<float>() * speed;
    auto spacing = glm::two_pi<float>() / static_cast<float>(orbiters.size());

    for (auto i = 0; i < orbiters.size(); ++i) {
      auto orbiterEntity = orbiters[i];

      auto orbiter = orbiterEntity->get<Orbiter>();
      auto center = getEntityById(orbiter->centerId);
      auto centerPosition = center->get<Transform>()->position;

      auto angle = startAngle + spacing * static_cast<float>(i);

      auto transform = orbiterEntity->get<Transform>();
      transform->position = centerPosition
                            + glm::vec3(
                                  glm::sin(angle) * orbiter->radius,
                                  glm::cos(angle) * orbiter->radius,
                                  0.0f
                              );
    }
  }
}

}  // namespace linguine
