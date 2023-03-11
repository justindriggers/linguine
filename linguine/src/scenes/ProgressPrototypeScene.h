#pragma once

#include "Scene.h"

#include "components/CameraFixture.h"
#include "components/Progressable.h"
#include "components/Selectable.h"
#include "components/Transform.h"
#include "systems/CameraSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/ProgressTestSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class ProgressPrototypeScene : public Scene {
  public:
    explicit ProgressPrototypeScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<ProgressTestSystem>(getEntityManager()));
      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();

      auto cameraEntity = createEntity();
      cameraEntity->add<CameraFixture>();
      cameraEntity->add<Transform>();

      auto entity = createEntity();
      entity->add<Transform>();

      auto progressable = entity->add<Progressable>();
      progressable->feature = new ProgressFeature();
      progressable->feature->meshType = Quad;
      progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature));
      progressable.setRemovalListener([progressable](const Entity e) {
        progressable->renderable->destroy();
      });

      auto selectable = entity->add<Selectable>();
      selectable->feature = new SelectableFeature();
      selectable->feature->entityId = entity->getId();
      selectable->feature->meshType = Quad;
      selectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature));
      selectable.setRemovalListener([selectable](const Entity e) {
        selectable->renderable->destroy();
      });
    }
};

}  // namespace linguine
