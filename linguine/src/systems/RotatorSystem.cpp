#include "RotatorSystem.h"

#include <glm/gtc/constants.hpp>

#include "components/PhysicalState.h"
#include "components/Rotating.h"
#include "components/Tapped.h"

namespace linguine {

void RotatorSystem::update(float deltaTime) {
  findEntities<Rotating, Tapped>()->each([this](const Entity& entity) {
    const auto rotating = entity.get<Rotating>();
    rotating->speed = -rotating->speed;
    _audioManager.play(EffectType::Select);
  });
}

void RotatorSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Rotating, PhysicalState>()->each([fixedDeltaTime](const Entity& entity) {
    const auto rotating = entity.get<Rotating>();
    const auto physicalState = entity.get<PhysicalState>();

    physicalState->currentRotation += glm::two_pi<float>() * rotating->speed * fixedDeltaTime;
  });
}

}  // namespace linguine