#include "CameraSystem.h"

#include "components/CameraFixture.h"
#include "components/Transform.h"

namespace linguine {

void CameraSystem::update(float deltaTime) {
  const auto& viewport = _renderer.getViewport();

  findEntities<Transform, CameraFixture>()->each([&viewport](const Entity& entity) {
    const auto transform = entity.get<Transform>();
    const auto cameraFixture = entity.get<CameraFixture>();

    if (cameraFixture->camera->layer == UI) {
      cameraFixture->height = 240.0f / viewport.getAspectRatio();
    }

    auto cameraModelMatrix = glm::translate(glm::mat4(1.0f), transform->position)
                             * glm::mat4_cast(transform->rotation);

    cameraFixture->camera->viewMatrix = glm::inverse(cameraModelMatrix);
    cameraFixture->camera->projectionMatrix = glm::ortho(
        -cameraFixture->height / 2.0f * viewport.getAspectRatio(),
        cameraFixture->height / 2.0f * viewport.getAspectRatio(),
        -cameraFixture->height / 2.0f,
        cameraFixture->height / 2.0f,
        0.0f,
        10.0f
    );

    cameraFixture->camera->viewProjectionMatrix = cameraFixture->camera->projectionMatrix * cameraFixture->camera->viewMatrix;
  });
}

}  // namespace linguine
