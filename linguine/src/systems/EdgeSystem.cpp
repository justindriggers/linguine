#include "EdgeSystem.h"

#include "components/CameraFixture.h"
#include "components/Footer.h"
#include "components/FooterPanel.h"
#include "components/Header.h"
#include "components/Transform.h"

namespace linguine {

void EdgeSystem::update(float deltaTime) {
  findEntities<CameraFixture>()->each([this](const Entity& cameraEntity) {
    auto cameraFixture = cameraEntity.get<CameraFixture>();

    if (cameraFixture->camera->layer == UI) {
      auto& viewport = _renderer.getViewport();
      auto height = cameraFixture->size;

      if (cameraFixture->type == CameraFixture::Measurement::Width) {
        height /= viewport.getAspectRatio();
      }

      auto bottomInset = static_cast<float>(viewport.getBottomInset()) / static_cast<float>(viewport.getHeight()) * height;

      findEntities<Footer, Transform>()->each([this, height, bottomInset](const Entity& entity) {
        auto footer = entity.get<Footer>();
        auto transform = entity.get<Transform>();

        transform->position.y = -height / 2.0f + transform->scale.y / 2.0f + footer->offset;

        if (!footer->ignoreInset) {
          transform->position += bottomInset;
        }

        findEntities<FooterPanel, Transform>()->each([height, bottomInset, footer](const Entity& footerPanelEntity) {
          auto footerPanel = footerPanelEntity.get<FooterPanel>();

          auto transform = footerPanelEntity.get<Transform>();
          transform->position.y = -height / 2.0f;
          transform->scale.y = (footerPanel->padding + footer->offset) * 2.0f;

          if (!footer->ignoreInset) {
            transform->scale.y += bottomInset * 2.0f;
          }
        });
      });

      auto topInset = static_cast<float>(viewport.getTopInset()) / static_cast<float>(viewport.getHeight()) * height;

      findEntities<Header, Transform>()->each([height, topInset](const Entity& entity) {
        auto header = entity.get<Header>();
        auto transform = entity.get<Transform>();

        transform->position.y = height / 2.0f - transform->scale.y / 2.0f - topInset - header->offset;
      });
    }
  });
}

}  // namespace linguine
