#pragma once

#include "Scene.h"

#include <random>

#include "components/Drawable.h"
#include "components/HasCamera.h"
#include "components/Quad.h"
#include "components/Rising.h"
#include "components/Rotating.h"
#include "components/Transform.h"
#include "systems/CameraSystem.h"
#include "systems/FpsSystem.h"
#include "systems/QuadTransformationSystem.h"
#include "systems/RiserSystem.h"
#include "systems/RotatorSystem.h"

namespace linguine {

class TestScene : public Scene {
  public:
    explicit TestScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<RiserSystem>(getEntityManager()));
      registerSystem(std::make_unique<RotatorSystem>(getEntityManager()));
      registerSystem(std::make_unique<QuadTransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();

      // Camera
      auto cameraEntity = createEntity();

      auto cameraTransform = cameraEntity->add<Transform>();
      cameraTransform->position = glm::vec3(0.0f, 0.0f, 0.0f);

      auto hasCamera = cameraEntity->add<HasCamera>();
      hasCamera->camera = renderer.getCamera();

      auto random = std::random_device();
      auto xDist = std::uniform_real_distribution(-2.55f, 2.5f);
      auto yDist = std::uniform_real_distribution(-5.0f, 5.0f);
      auto zDist = std::uniform_real_distribution(0.0f, 10.0f);
      auto normalDist = std::uniform_real_distribution(0.0f, 1.0f);
      auto rotationDist = std::uniform_real_distribution(-1.0f, 1.0f);

      auto componentDist = std::uniform_int_distribution(0, 1);

      for (int i = 0; i < 10'000; ++i) {
        auto entity = createEntity();

        auto transform = entity->add<Transform>();
        transform->position = glm::vec3(xDist(random), yDist(random), zDist(random));

        auto quad = entity->add<Quad>();
        quad->feature = std::make_shared<QuadFeature>();
        quad->feature->color = glm::vec3(normalDist(random), normalDist(random), normalDist(random));

        auto drawable = entity->add<Drawable>();
        drawable->renderable = renderer.create(quad->feature);

        if (componentDist(random) > 0) {
          auto rising = entity->add<Rising>();
          rising->speed = normalDist(random);
        } else {
          auto rotating = entity->add<Rotating>();
          rotating->speed = rotationDist(random);
        }
      }
    }
};

}  // namespace linguine
