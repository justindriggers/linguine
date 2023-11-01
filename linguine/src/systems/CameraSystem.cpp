#include "CameraSystem.h"

#include "components/CameraFixture.h"
#include "components/Transform.h"

namespace linguine {

void CameraSystem::update(float deltaTime) {
  const auto& viewport = _renderer.getViewport();

  findEntities<Transform, CameraFixture>()->each([&viewport](const Entity& entity) {
    const auto transform = entity.get<Transform>();
    const auto cameraFixture = entity.get<CameraFixture>();

    auto cameraModelMatrix = glm::translate(glm::mat4(1.0f), transform->position)
                             * glm::mat4_cast(transform->rotation);

    cameraFixture->camera->viewMatrix = glm::inverse(cameraModelMatrix);

    switch (cameraFixture->type) {
    case CameraFixture::Measurement::Height:
      cameraFixture->camera->projectionMatrix = glm::ortho(
          -cameraFixture->size / 2.0f * viewport.getAspectRatio(),
          cameraFixture->size / 2.0f * viewport.getAspectRatio(),
          -cameraFixture->size / 2.0f,
          cameraFixture->size / 2.0f,
          0.0f,
          10.0f
      );
      break;
    case CameraFixture::Measurement::Width:
      cameraFixture->camera->projectionMatrix = glm::ortho(
          -cameraFixture->size / 2.0f,
          cameraFixture->size / 2.0f,
          -cameraFixture->size / 2.0f / viewport.getAspectRatio(),
          cameraFixture->size / 2.0f / viewport.getAspectRatio(),
          0.0f,
          10.0f
      );
      break;
    }

    cameraFixture->camera->viewProjectionMatrix = cameraFixture->camera->projectionMatrix * cameraFixture->camera->viewMatrix;
  });
}

}  // namespace linguine
