#include "PhysicsInterpolationSystem.h"

#include <glm/gtx/compatibility.hpp>

#include "components/PhysicalState.h"
#include "components/Transform.h"

namespace linguine {

void PhysicsInterpolationSystem::update(float deltaTime) {
  findEntities<PhysicalState, Transform>()->each([this](const Entity& entity) {
    auto physicalState = entity.get<PhysicalState>();
    auto transform = entity.get<Transform>();

    const auto lerpPosition = glm::lerp(
        physicalState->previousPosition,
        physicalState->currentPosition,
        _timeManager.getAccumulatorProgress()
    );

    transform->position.x = lerpPosition.x;
    transform->position.y = lerpPosition.y;

    const auto lerpRotation = glm::lerp(
        physicalState->previousRotation,
        physicalState->currentRotation,
        _timeManager.getAccumulatorProgress()
    );

    transform->rotation = glm::angleAxis(lerpRotation, glm::vec3(0.0f, 0.0f, 1.0f));
  });
}

void PhysicsInterpolationSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<PhysicalState>()->each([](const Entity& entity) {
    auto physicalState = entity.get<PhysicalState>();
    physicalState->previousPosition = physicalState->currentPosition;
    physicalState->previousRotation = physicalState->currentRotation;
  });
}

}  // namespace linguine
