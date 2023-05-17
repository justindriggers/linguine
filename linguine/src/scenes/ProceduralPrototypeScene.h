#pragma once

#include "Scene.h"

#include <random>
#include <utility>

#include "components/BigHeal.h"
#include "components/CameraFixture.h"
#include "components/CircleCollider.h"
#include "components/Cooldown.h"
#include "components/Drawable.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/GridPosition.h"
#include "components/Health.h"
#include "components/HealthBar.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/PlayerTarget.h"
#include "components/Progressable.h"
#include "components/Rotating.h"
#include "components/Transform.h"
#include "data/Grid.h"
#include "data/rooms/RoomA.h"
#include "data/rooms/RoomB.h"
#include "data/rooms/RoomC.h"
#include "data/rooms/RoomLayout.h"
#include "data/rooms/SpawnRoom.h"
#include "systems/CameraFollowSystem.h"
#include "systems/CameraSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/CooldownProgressSystem.h"
#include "systems/EnemyAttackSystem.h"
#include "systems/EnemyTargetingSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/GridPositionSystem.h"
#include "systems/HealthProgressSystem.h"
#include "systems/LivenessSystem.h"
#include "systems/PhysicsInterpolationSystem.h"
#include "systems/PlayerAttackSystem.h"
#include "systems/PlayerControllerSystem.h"
#include "systems/PlayerTargetingSystem.h"
#include "systems/PointAndClickMovementSystem.h"
#include "systems/ProjectileSystem.h"
#include "systems/RotatorSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class ProceduralPrototypeScene : public Scene {
  public:
    explicit ProceduralPrototypeScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<PlayerControllerSystem>(getEntityManager()));
      registerSystem(std::make_unique<PlayerTargetingSystem>(getEntityManager()));
      registerSystem(std::make_unique<PointAndClickMovementSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<CollisionSystem>(getEntityManager()));
      registerSystem(std::make_unique<RotatorSystem>(getEntityManager()));
      registerSystem(std::make_unique<ProjectileSystem>(getEntityManager()));
      registerSystem(std::make_unique<GridPositionSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<EnemyTargetingSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<EnemyAttackSystem>(getEntityManager(), serviceLocator.get<Renderer>(), *_grid));
      registerSystem(std::make_unique<PlayerAttackSystem>(getEntityManager(), serviceLocator.get<Renderer>()));
      registerSystem(std::make_unique<LivenessSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<HealthProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<CooldownProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraFollowSystem>(getEntityManager()));
      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();

      {
        auto cameraEntity = createEntity();

        auto transform = cameraEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

        auto physicalState = cameraEntity->add<PhysicalState>();
        physicalState->currentPosition = transform->position;

        auto fixture = cameraEntity->add<CameraFixture>();
        fixture->height = 20.0f;
        fixture->camera = renderer.createCamera();
        fixture->camera->clearColor = glm::vec3(0.64f, 0.65f, 0.11f);
      }

      {
        auto uiCameraEntity = createEntity();

        auto transform = uiCameraEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

        auto fixture = uiCameraEntity->add<CameraFixture>();
        fixture->height = 1.0f;
        fixture->camera = renderer.createCamera();
        fixture->camera->clearColor = {};
        fixture->camera->layer = UI;
      }

      {
        auto uiBackgroundEntity = createEntity();

        auto transform = uiBackgroundEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, -0.35f, 10.0f);
        transform->scale = glm::vec3(1.0f, 0.3f, 0.0f);

        auto drawable = uiBackgroundEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Quad;
        drawable->feature->color = glm::vec3(0.15f);
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature), UI);
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });

        auto selectable = uiBackgroundEntity->add<Selectable>();
        selectable->feature = new SelectableFeature();
        selectable->feature->meshType = Quad;
        selectable->feature->entityId = uiBackgroundEntity->getId();
        selectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature), UI);
        selectable.setRemovalListener([selectable](const Entity e) {
          selectable->renderable->destroy();
        });
      }

      auto roomLayout = RoomLayout();

      std::unique_ptr<Room> previousRoom = std::make_unique<SpawnRoom>();

      auto origin = glm::ivec2(250, 250);
      previousRoom->layout(getEntityManager(), serviceLocator, *_grid, roomLayout, origin);

      auto playerStartPosition = origin + glm::ivec2(previousRoom->getWidth(), previousRoom->getHeight()) / 2;

      auto rand = std::random_device();
      auto randomRoom = std::uniform_int_distribution(0, 2);
      auto randomDirection = std::uniform_int_distribution(0, 3);

      auto roomCount = 0;

      do {
          std::unique_ptr<Room> currentRoom;

          switch (randomRoom(rand)) {
            case 0:
              currentRoom = std::make_unique<RoomA>();
              break;
            case 1:
              currentRoom = std::make_unique<RoomB>();
              break;
            case 2:
              currentRoom = std::make_unique<RoomC>();
              break;
            default:
              throw std::runtime_error("Unexpected RNG value");
          }

          auto newOrigin = origin;

          switch (randomDirection(rand)) {
            case 0:
              newOrigin += previousRoom->getNorthDoor();
              newOrigin -= currentRoom->getSouthDoor();
              newOrigin += glm::ivec2(0, 1);
              break;
            case 1:
              newOrigin += previousRoom->getSouthDoor();
              newOrigin -= currentRoom->getNorthDoor();
              newOrigin -= glm::ivec2(0, 1);
              break;
            case 2:
              newOrigin += previousRoom->getEastDoor();
              newOrigin -= currentRoom->getWestDoor();
              newOrigin += glm::ivec2(1, 0);
              break;
            case 3:
              newOrigin += previousRoom->getWestDoor();
              newOrigin -= currentRoom->getEastDoor();
              newOrigin -= glm::ivec2(1, 0);
              break;
            default:
              throw std::runtime_error("Unexpected RNG value");
          }

          if (currentRoom->layout(getEntityManager(), serviceLocator, *_grid, roomLayout, newOrigin)) {
            previousRoom->enclose(getEntityManager(), serviceLocator, *_grid, roomLayout, origin);
            previousRoom = std::move(currentRoom);
            origin = newOrigin;
            ++roomCount;
          }
      } while (roomCount < 16);

      previousRoom->enclose(getEntityManager(), serviceLocator, *_grid, roomLayout, origin);

      {
        auto playerEntity = createEntity();
        playerEntity->add<Player>();
        playerEntity->add<Friendly>();
        playerEntity->add<Unit>();
        playerEntity->add<Alive>();

        auto transform = playerEntity->add<Transform>();
        transform->scale = glm::vec3(0.75f);
        transform->position = glm::vec3(_grid->getWorldPosition(playerStartPosition), 1.0f);

        auto physicalState = playerEntity->add<PhysicalState>();
        physicalState->previousPosition = glm::vec2(transform->position);
        physicalState->currentPosition = physicalState->previousPosition;

        auto circleCollider = playerEntity->add<CircleCollider>();
        circleCollider->radius = 0.375f;

        auto gridPositionComponent = playerEntity->add<GridPosition>();
        gridPositionComponent->position = playerStartPosition;
        gridPositionComponent->speed = 2.0f;

        auto drawable = playerEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Triangle;
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });

        auto projectileAttack = playerEntity->add<ProjectileAttack>();
        projectileAttack->speed = 5.0f;
        projectileAttack->power = 100;
        projectileAttack->frequency = 1.5f;

        const auto count = 5;

        for (int i = 0; i < count; ++i) {
          auto orbiterEntity = createEntity();
          orbiterEntity->add<Friendly>();

          orbiterEntity->add<Health>(500);
          orbiterEntity->add<Alive>();

          {
            auto healthEntity = createEntity();

            auto healthBar = healthEntity->add<HealthBar>();
            healthBar->entityId = orbiterEntity->getId();

            auto healthTransform = healthEntity->add<Transform>();
            healthTransform->position = glm::vec3((-static_cast<float>(count) / 2.0f + static_cast<float>(i) + 0.5f) * 0.08f, -0.3f, 0.0f);
            healthTransform->scale = glm::vec3(0.075f, 0.075f, 0.0f);

            auto healthProgressable = healthEntity->add<Progressable>();
            healthProgressable->feature = new ProgressFeature();
            healthProgressable->feature->meshType = Quad;
            healthProgressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(healthProgressable->feature), UI);
            healthProgressable.setRemovalListener([healthProgressable](const Entity e) {
              healthProgressable->renderable->destroy();
            });

            auto healthSelectable = healthEntity->add<Selectable>();
            healthSelectable->feature = new SelectableFeature();
            healthSelectable->feature->meshType = Quad;
            healthSelectable->feature->entityId = healthEntity->getId();
            healthSelectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(healthSelectable->feature), UI);
            healthSelectable.setRemovalListener([healthSelectable](const Entity e) {
              healthSelectable->renderable->destroy();
            });
          }
        }

        {
          auto playerTargetEntity = createEntity();
          playerTargetEntity->add<PlayerTarget>();

          auto playerTargetRotating = playerTargetEntity->add<Rotating>();
          playerTargetRotating->speed = 0.5f;

          auto playerTargetTransform = playerTargetEntity->add<Transform>();
          playerTargetTransform->position = glm::vec3(0.0f, 0.0f, 0.0f);
          playerTargetTransform->scale = glm::vec3(0.25f);

          auto playerTargetDrawable = playerTargetEntity->add<Drawable>();
          playerTargetDrawable->feature = new ColoredFeature();
          playerTargetDrawable->feature->meshType = Quad;
          playerTargetDrawable->feature->color = glm::vec3(1.0f);
          playerTargetDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(playerTargetDrawable->feature));
          playerTargetDrawable.setRemovalListener([playerTargetDrawable](const Entity e) {
            playerTargetDrawable->renderable->destroy();
          });
          playerTargetDrawable->renderable->setEnabled(false);
        }
      }

      {
        auto gcdEntity = createEntity();

        auto transform = gcdEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, -0.225f, 0.0f);
        transform->scale = glm::vec3(0.25f, 0.025f, 1.0f);

        auto progressable = gcdEntity->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->meshType = Quad;
        progressable->feature->color = glm::vec3(0.88f, 0.34f, 0.1f);
        progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature), UI);
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });
        progressable->renderable->setEnabled(false);

        auto globalCooldown = gcdEntity->add<GlobalCooldown>();
        globalCooldown->elapsed = 1.5f;
        globalCooldown->total = 1.5f;
      }

      {
        auto bigHealEntity = createEntity();

        auto transform = bigHealEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, -0.425f, 0.0f);
        transform->scale = glm::vec3(0.09f);

        auto progressable = bigHealEntity->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->meshType = Quad;
        progressable->feature->color = glm::vec3(0.0f, 1.0f, 0.0f);
        progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature), UI);
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });

        auto selectable = bigHealEntity->add<Selectable>();
        selectable->feature = new SelectableFeature();
        selectable->feature->entityId = bigHealEntity->getId();
        selectable->feature->meshType = Quad;
        selectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature), UI);
        selectable.setRemovalListener([selectable](const Entity e) {
          selectable->renderable->destroy();
        });

        bigHealEntity->add<BigHeal>();

        auto cooldown = bigHealEntity->add<Cooldown>();
        cooldown->elapsed = 10.0f;
        cooldown->total = 10.0f;
      }
    }

  private:
    std::unique_ptr<Grid> _grid = std::make_unique<Grid>(500, 500, 1.0f);
};

}  // namespace linguine
