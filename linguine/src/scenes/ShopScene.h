#pragma once

#include "Scene.h"

#include "ServiceLocator.h"
#include "components/CameraFixture.h"
#include "components/Drawable.h"
#include "components/Enabled.h"
#include "components/Selectable.h"
#include "components/Text.h"
#include "components/Transform.h"
#include "components/shop/PlayButton.h"
#include "components/shop/Wallet.h"
#include "systems/CameraSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/shop/UpgradeSystem.h"
#include "systems/shop/WalletSystem.h"

namespace linguine {

class ShopScene : public Scene {
  public:
    explicit ShopScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()),
          _upgradeDatabase(std::make_unique<UpgradeDatabase>()) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));

      registerSystem(std::make_unique<UpgradeSystem>(getEntityManager(), serviceLocator.get<Renderer>(), serviceLocator.get<SaveManager>(), serviceLocator.get<SceneManager>(), serviceLocator, *_upgradeDatabase));
      registerSystem(std::make_unique<WalletSystem>(getEntityManager(), serviceLocator.get<SaveManager>()));

      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();

      {
        auto uiCameraEntity = createEntity();

        auto transform = uiCameraEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

        auto fixture = uiCameraEntity->add<CameraFixture>();
        fixture->size = 240.0f;
        fixture->type = CameraFixture::Measurement::Width;
        fixture->camera = renderer.createCamera();
        fixture->camera->clearColor = { 0.0f, 0.0f, 0.0f };
        fixture->camera->layer = UI;
        fixture.setRemovalListener([fixture](const Entity& e) {
          fixture->camera->destroy();
        });
      }

      {
        auto walletEntity = createEntity();
        walletEntity->add<Wallet>();

        auto walletTransform = walletEntity->add<Transform>();
        walletTransform->position = { 72.0f, 180.0f, 0.0f };
        walletTransform->scale = { 16.0f, 16.0f, 1.0f };

        auto walletText = walletEntity->add<Text>();
        walletText->feature = new TextFeature();
        walletText->feature->text = "0";
        walletText->renderable = renderer.create(std::unique_ptr<TextFeature>(walletText->feature), UI);
        walletText.setRemovalListener([walletText](const Entity e) {
          walletText->renderable->destroy();
        });
      }

      {
        auto nextButtonEntity = createEntity();

        auto nextButtonTransform = nextButtonEntity->add<Transform>();
        nextButtonTransform->position = { 72.0f, -180.0f, 0.0f };
        nextButtonTransform->scale = { 80.0f, 32.0f, 1.0f };

        auto nextButtonDrawable = nextButtonEntity->add<Drawable>();
        nextButtonDrawable->feature = new ColoredFeature();
        nextButtonDrawable->feature->color = { 0.97345f, 0.36625f, 0.00561f };
        nextButtonDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(nextButtonDrawable->feature), UI);
        nextButtonDrawable.setRemovalListener([nextButtonDrawable](const Entity e) {
          nextButtonDrawable->renderable->destroy();
        });
        nextButtonDrawable->renderable->setEnabled(false);

        {
          auto nextButtonTextEntity = createEntity();

          auto nextButtonTextTransform = nextButtonTextEntity->add<Transform>();
          nextButtonTextTransform->position = { 50.0f, -180.0f, 0.0f };
          nextButtonTextTransform->scale = { 16.0f, 16.0f, 1.0f };

          auto nextButtonText = nextButtonTextEntity->add<Text>();
          nextButtonText->feature = new TextFeature();
          nextButtonText->feature->text = "Next";
          nextButtonText->renderable = renderer.create(std::unique_ptr<TextFeature>(nextButtonText->feature), UI);
          nextButtonText.setRemovalListener([nextButtonText](const Entity e) {
            nextButtonText->renderable->destroy();
          });
          nextButtonText->renderable->setEnabled(false);
        }
      }

      {
        auto prevButtonEntity = createEntity();

        auto prevButtonTransform = prevButtonEntity->add<Transform>();
        prevButtonTransform->position = { -72.0f, -180.0f, 0.0f };
        prevButtonTransform->scale = { 80.0f, 32.0f, 1.0f };

        auto prevButtonDrawable = prevButtonEntity->add<Drawable>();
        prevButtonDrawable->feature = new ColoredFeature();
        prevButtonDrawable->feature->color = { 0.97345f, 0.36625f, 0.00561f };
        prevButtonDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(prevButtonDrawable->feature), UI);
        prevButtonDrawable.setRemovalListener([prevButtonDrawable](const Entity e) {
          prevButtonDrawable->renderable->destroy();
        });
        prevButtonDrawable->renderable->setEnabled(false);

        {
          auto prevButtonTextEntity = createEntity();

          auto prevButtonTextTransform = prevButtonTextEntity->add<Transform>();
          prevButtonTextTransform->position = { -94.0f, -180.0f, 0.0f };
          prevButtonTextTransform->scale = { 16.0f, 16.0f, 1.0f };

          auto prevButtonText = prevButtonTextEntity->add<Text>();
          prevButtonText->feature = new TextFeature();
          prevButtonText->feature->text = "Prev";
          prevButtonText->renderable = renderer.create(std::unique_ptr<TextFeature>(prevButtonText->feature), UI);
          prevButtonText.setRemovalListener([prevButtonText](const Entity e) {
            prevButtonText->renderable->destroy();
          });
          prevButtonText->renderable->setEnabled(false);
        }
      }

      {
        auto playButtonEntity = createEntity();
        playButtonEntity->add<PlayButton>();
        playButtonEntity->add<Enabled>();

        auto playButtonTransform = playButtonEntity->add<Transform>();
        playButtonTransform->position = { 0.0f, -180.0f, 0.0f };
        playButtonTransform->scale = { 80.0f, 32.0f, 1.0f };

        auto playButtonDrawable = playButtonEntity->add<Drawable>();
        playButtonDrawable->feature = new ColoredFeature();
        playButtonDrawable->feature->color = { 0.007f, 0.01521f, 0.04667f };
        playButtonDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(playButtonDrawable->feature), UI);
        playButtonDrawable.setRemovalListener([playButtonDrawable](const Entity e) {
          playButtonDrawable->renderable->destroy();
        });

        auto playButtonSelectable = playButtonEntity->add<Selectable>();
        playButtonSelectable->feature = new SelectableFeature();
        playButtonSelectable->feature->entityId = playButtonEntity->getId();
        playButtonSelectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(playButtonSelectable->feature), UI);
        playButtonSelectable.setRemovalListener([playButtonSelectable](const Entity e) {
          playButtonSelectable->renderable->destroy();
        });

        {
          auto playButtonTextEntity = createEntity();

          auto playButtonTextTransform = playButtonTextEntity->add<Transform>();
          playButtonTextTransform->position = { -8.0f, -180.0f, 0.0f };
          playButtonTextTransform->scale = { 16.0f, 16.0f, 1.0f };

          auto playButtonText = playButtonTextEntity->add<Text>();
          playButtonText->feature = new TextFeature();
          playButtonText->feature->text = "Go";
          playButtonText->renderable = renderer.create(std::unique_ptr<TextFeature>(playButtonText->feature), UI);
          playButtonText.setRemovalListener([playButtonText](const Entity e) {
            playButtonText->renderable->destroy();
          });
        }
      }
    }

  private:
    std::unique_ptr<UpgradeDatabase> _upgradeDatabase;
};

}  // namespace linguine
