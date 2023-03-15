#include "PhysicsInterpolationSystem.h"

#include <glm/gtx/compatibility.hpp>

#include "components/PhysicalState.h"
#include "components/Transform.h"

namespace linguine {

void PhysicsInterpolationSystem::update(float deltaTime) {
  _timeSinceLastFixedUpdate += deltaTime;

  const auto lerpFactor = _timeSinceLastFixedUpdate / _timeManager.getFixedTimeStep();

  findEntities<PhysicalState, Transform>()->each([lerpFactor](const Entity& entity) {
    auto physicalState = entity.get<PhysicalState>();
    auto transform = entity.get<Transform>();

    const auto lerpPosition = glm::lerp(
        physicalState->previousPosition,
        physicalState->currentPosition,
        lerpFactor
    );

    transform->position.x = lerpPosition.x;
    transform->position.y = lerpPosition.y;
  });
}

void PhysicsInterpolationSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<PhysicalState>()->each([](const Entity& entity) {
    auto physicalState = entity.get<PhysicalState>();
    physicalState->previousPosition = physicalState->currentPosition;
  });

  _timeSinceLastFixedUpdate = 0.0f;
}

}  // namespace linguine
