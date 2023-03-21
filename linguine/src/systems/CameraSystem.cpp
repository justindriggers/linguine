#include "CameraSystem.h"

#include "components/CameraFixture.h"
#include "components/Transform.h"

namespace linguine {

void CameraSystem::update(float deltaTime) {
  const auto& viewport = _renderer.getViewport();
  auto& camera = _renderer.getCamera();

  findEntities<Transform, CameraFixture>()->each([&viewport, &camera](const Entity& entity) {
    const auto transform = entity.get<Transform>();
    const auto cameraFixture = entity.get<CameraFixture>();

    auto cameraModelMatrix = glm::translate(glm::mat4(1.0f), transform->position)
                             * glm::mat4_cast(transform->rotation);

    camera.viewMatrix = glm::inverse(cameraModelMatrix);
    camera.projectionMatrix = glm::ortho(
        -cameraFixture->height / 2.0f * viewport.getAspectRatio(),
        cameraFixture->height / 2.0f * viewport.getAspectRatio(),
        -cameraFixture->height / 2.0f,
        cameraFixture->height / 2.0f,
        0.0f,
        10.0f
    );

    camera.viewProjectionMatrix = camera.projectionMatrix * camera.viewMatrix;
  });
}

}  // namespace linguine
