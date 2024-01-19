#include "FooterSystem.h"

#include "components/CameraFixture.h"
#include "components/Footer.h"
#include "components/Transform.h"

namespace linguine {

void FooterSystem::update(float deltaTime) {
  findEntities<CameraFixture>()->each([this](const Entity& cameraEntity) {
    auto cameraFixture = cameraEntity.get<CameraFixture>();

    if (cameraFixture->camera->layer == UI) {
      auto height = cameraFixture->size;

      if (cameraFixture->type == CameraFixture::Measurement::Width) {
        height /= _renderer.getViewport().getAspectRatio();
      }

      findEntities<Footer, Transform>()->each([height](const Entity& entity) {
        auto transform = entity.get<Transform>();
        transform->position.y = -height / 2.0f + transform->scale.y / 2.0f;
      });
    }
  });
}

}  // namespace linguine
