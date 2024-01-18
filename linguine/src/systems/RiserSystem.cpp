#include "RiserSystem.h"

#include "components/Falling.h"
#include "components/Rising.h"
#include "components/Tapped.h"
#include "components/Transform.h"

namespace linguine {

void RiserSystem::update(float deltaTime) {
  findEntities<Rising, Tapped>()->each([this](Entity& entity) {
    const auto speed = entity.get<Rising>()->speed;
    entity.remove<Rising>();

    auto falling = entity.add<Falling>();
    falling->speed = speed;

    _audioManager.play(EffectType::ButtonDown);
  });

  findEntities<Rising, Transform>()->each([deltaTime](const Entity& entity) {
    const auto rising = entity.get<Rising>();
    const auto transform = entity.get<Transform>();

    transform->position += glm::vec3(0.0f, 1.0f, 0.0f) * rising->speed * deltaTime;
  });
}

}  // namespace linguine