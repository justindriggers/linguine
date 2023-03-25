#pragma once

#include "Scene.h"

#include "components/CameraFixture.h"
#include "components/Drawable.h"
#include "components/Selectable.h"
#include "components/Tile.h"
#include "components/Transform.h"
#include "systems/CameraSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/TileSelectionSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class TowerPrototypeScene : public Scene {
  public:
    explicit TowerPrototypeScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<TileSelectionSystem>(getEntityManager(), serviceLocator.get<Renderer>()));
      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();
      renderer.getCamera().clearColor = glm::vec3(0.34f, 0.55f, 0.04f);

      auto cameraEntity = createEntity();
      cameraEntity->add<CameraFixture>();
      cameraEntity->add<Transform>();

      constexpr auto boardWidth = 5;
      constexpr auto boardHeight = 10;

      constexpr auto entityWidth = 1.25f;
      constexpr auto entityHeight = 1.25f;

      for (auto x = 0; x < boardWidth; ++x) {
        for (auto y = 0; y < boardHeight; ++y) {
          auto entity = createEntity();
          entity->add<Tile>();

          auto transform = entity->add<Transform>();
          transform->position = glm::vec3((-static_cast<float>(boardWidth) / 2.0f + 0.5f + static_cast<float>(x)) * entityWidth,
                                          (-static_cast<float>(boardHeight) / 2.0f + 0.5f + static_cast<float>(y)) * entityHeight,
                                          1.0f);
          transform->scale = glm::vec3(0.9f);

          auto drawable = entity->add<Drawable>();
          drawable->feature = new ColoredFeature();
          drawable->feature->meshType = Quad;
          drawable->feature->color = glm::vec3(0.06f, 0.06f, 0.06f);
          drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
          drawable.setRemovalListener([drawable](const Entity e) {
            drawable->renderable->destroy();
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
      }
    }
};

}  // namespace linguine
