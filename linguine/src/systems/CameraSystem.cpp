#include "CameraSystem.h"

#include "components/Transform.h"
#include "components/HasCamera.h"

namespace linguine {

void CameraSystem::update(float deltaTime) {
  const auto viewport = _renderer.getViewport();

  findEntities<Transform, HasCamera>()->each([viewport](const Entity& entity) {
    const auto transform = entity.get<Transform>();
    const auto hasCamera = entity.get<HasCamera>();
    const auto camera = hasCamera->camera;

    auto cameraModelMatrix = glm::translate(glm::mat4(1.0f), transform->position)
                             * glm::mat4_cast(transform->rotation);

    camera->viewMatrix = glm::inverse(cameraModelMatrix);
    camera->projectionMatrix = glm::ortho(
        -_height / 2.0f * viewport->getAspectRatio(),
        _height / 2.0f * viewport->getAspectRatio(),
        -_height / 2.0f,
        _height / 2.0f,
        0.0f,
        10.0f
    );

    camera->viewProjectionMatrix = camera->projectionMatrix * camera->viewMatrix;
  });
}

}  // namespace linguine
