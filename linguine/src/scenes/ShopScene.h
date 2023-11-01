#pragma once

#include "Scene.h"

#include "ServiceLocator.h"
#include "components/CameraFixture.h"
#include "components/Transform.h"
#include "systems/CameraSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class ShopScene : public Scene {
  public:
    explicit ShopScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));

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
        fixture->camera->clearColor = { 0.0f, 0.0f, 1.0f };
        fixture->camera->layer = UI;
        fixture.setRemovalListener([fixture](const Entity& e) {
          fixture->camera->destroy();
        });
      }
    }
};

}  // namespace linguine
