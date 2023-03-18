#pragma once

#include "Scene.h"

#include "components/CameraFixture.h"
#include "components/Drawable.h"
#include "components/Path.h"
#include "components/Transform.h"
#include "data/CubicBezierCurve.h"
#include "data/Spline.h"
#include "systems/CameraSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/PathTraversalSystem.h"
#include "systems/PlayerControllerSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class MovementPrototypeScene : public Scene {
  public:
    explicit MovementPrototypeScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<PlayerControllerSystem>(getEntityManager()));
      registerSystem(std::make_unique<PathTraversalSystem>(getEntityManager()));
      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();
      renderer.getCamera().clearColor = glm::vec3(0.93f, 0.11f, 0.04f);

      auto cameraEntity = createEntity();
      cameraEntity->add<CameraFixture>();
      cameraEntity->add<Transform>();

      auto c0 = CubicBezierCurve{
          {
              { 0.0f, 1.0f },
              { 0.552284749831f, 1.0f },
              { 1.0f, 0.552284749831f },
              { 1.0f, 0.0f }
          }
      };

      auto c1 = CubicBezierCurve{
          {
              { 1.0f, 0.0f },
              { 1.0f, -0.552284749831f },
              { 0.552284749831f, -1.0f },
              { 0.0f, -1.0f }
          }
      };

      auto c2 = CubicBezierCurve{
          {
              { 0.0f, -1.0f },
              { -0.552284749831f, -1.0f },
              { -1.0f, -0.552284749831f },
              { -1.0f, 0.0f }
          }
      };

      auto c3 = CubicBezierCurve{
          {
              { -1.0f, 0.0f },
              { -1.0f, 0.552284749831f },
              { -0.552284749831f, 1.0f },
              { 0.0f, 1.0f }
          }
      };

      auto spline = Spline{
          { c0, c1, c2, c3 }
      };

      auto entity = createEntity();
      entity->add<Transform>();

      auto path = entity->add<Path>();
      path->spline = spline;
      path->scale = 2.5f;
      path->speed = 1.25f;

      auto drawable = entity->add<Drawable>();
      drawable->feature = new ColoredFeature();
      drawable->feature->meshType = Quad;
      drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
      drawable.setRemovalListener([drawable](const Entity e) {
        drawable->renderable->destroy();
      });
    }
};

}  // namespace linguine
