#pragma once

#include "Scene.h"

#include <random>

#include "components/CameraFixture.h"
#include "components/Drawable.h"
#include "components/Rising.h"
#include "components/Rotating.h"
#include "components/Selectable.h"
#include "components/Transform.h"
#include "systems/CameraSystem.h"
#include "systems/FallerSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/RiserSystem.h"
#include "systems/RotatorSystem.h"
#include "systems/SelectionDestructionSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class TestScene : public Scene {
  public:
    explicit TestScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<SelectionDestructionSystem>(getEntityManager()));
      registerSystem(std::make_unique<FallerSystem>(getEntityManager()));
      registerSystem(std::make_unique<RiserSystem>(getEntityManager()));
      registerSystem(std::make_unique<RotatorSystem>(getEntityManager()));
      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();

      // Camera
      auto cameraEntity = createEntity();
      cameraEntity->add<CameraFixture>();

      auto cameraTransform = cameraEntity->add<Transform>();
      cameraTransform->position = glm::vec3(0.0f, 0.0f, 0.0f);

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

        auto drawable = entity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->color = glm::vec3(normalDist(random), normalDist(random), normalDist(random));

        auto selectable = entity->add<Selectable>();
        selectable->feature = new SelectableFeature();
        selectable->feature->entityId = entity->getId();

        if (componentDist(random) > 0) {
          drawable->feature->meshType = Quad;
          selectable->feature->meshType = Quad;
        } else {
          drawable->feature->meshType = Triangle;
          selectable->feature->meshType = Triangle;
        }

        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });

        selectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature));
        selectable.setRemovalListener([selectable](const Entity e) {
          selectable->renderable->destroy();
        });

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
