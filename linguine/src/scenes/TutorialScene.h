#pragma once

#include "Scene.h"

#include <array>

#include "components/Ability.h"
#include "components/Alive.h"
#include "components/Attachment.h"
#include "components/BoxCollider.h"
#include "components/CameraFixture.h"
#include "components/Cast.h"
#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/Health.h"
#include "components/Hostile.h"
#include "components/MeleeAttack.h"
#include "components/Party.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/Progressable.h"
#include "components/ProjectileAttack.h"
#include "components/Selectable.h"
#include "components/Static.h"
#include "components/TargetIndicator.h"
#include "components/Text.h"
#include "components/Tooltip.h"
#include "components/Transform.h"
#include "components/Trigger.h"
#include "components/Unit.h"
#include "components/Velocity.h"
#include "components/tutorial/TutorialState.h"
#include "components/tutorial/TutorialText1.h"
#include "components/tutorial/TutorialText2.h"
#include "systems/AttachmentSystem.h"
#include "systems/CameraFollowSystem.h"
#include "systems/CameraSystem.h"
#include "systems/CastSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/CooldownProgressSystem.h"
#include "systems/DirectionalMovementSystem.h"
#include "systems/EffectSystem.h"
#include "systems/EnemyAttackSystem.h"
#include "systems/EnemyTargetingSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/GridPositionSystem.h"
#include "systems/HealthProgressSystem.h"
#include "systems/HudSystem.h"
#include "systems/PhysicsInterpolationSystem.h"
#include "systems/PlayerControllerSystem.h"
#include "systems/ProjectileSystem.h"
#include "systems/TooltipSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/tutorial/WispTriggerSystem.h"

namespace linguine {

class TutorialScene : public Scene {
  public:
    explicit TutorialScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()),
          _spellDatabase(std::make_unique<SpellDatabase>(serviceLocator, getEntityManager())) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<PlayerControllerSystem>(getEntityManager(), *_spellDatabase, serviceLocator.get<InputManager>()));
      registerSystem(std::make_unique<AttachmentSystem>(getEntityManager()));
      registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<DirectionalMovementSystem>(getEntityManager(), serviceLocator.get<InputManager>()));
      registerSystem(std::make_unique<CollisionSystem>(getEntityManager()));
      registerSystem(std::make_unique<GridPositionSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<EffectSystem>(getEntityManager(), *_spellDatabase));
      registerSystem(std::make_unique<ProjectileSystem>(getEntityManager()));
      registerSystem(std::make_unique<EnemyTargetingSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<EnemyAttackSystem>(getEntityManager(), serviceLocator.get<Renderer>()));
      registerSystem(std::make_unique<HudSystem>(getEntityManager(), serviceLocator.get<Renderer>()));
      registerSystem(std::make_unique<HealthProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<CooldownProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<CastSystem>(getEntityManager()));
      registerSystem(std::make_unique<TooltipSystem>(getEntityManager(), serviceLocator.get<Renderer>(), serviceLocator.get<InputManager>()));

      // Scene-specific
      registerSystem(std::make_unique<WispTriggerSystem>(getEntityManager()));

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
        fixture->camera->clearColor = glm::vec3(0.04f, 0.05f, 0.37f);
      }

      {
        auto uiCameraEntity = createEntity();

        auto transform = uiCameraEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

        auto fixture = uiCameraEntity->add<CameraFixture>();
        fixture->height = 768.0f;
        fixture->camera = renderer.createCamera();
        fixture->camera->clearColor = {};
        fixture->camera->layer = UI;
      }

      {
        auto tooltipEntity = createEntity();
        tooltipEntity->add<Tooltip>();
      }

      {
        auto uiBackgroundEntity = createEntity();

        auto transform = uiBackgroundEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, -269.0f, 10.0f);
        transform->scale = glm::vec3(768.0f, 230.0f, 0.0f);

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

      {
        auto playerEntity = createEntity();
        playerEntity->add<Player>();
        playerEntity->add<Friendly>();
        playerEntity->add<Unit>();
        playerEntity->add<Alive>();

        auto transform = playerEntity->add<Transform>();
        transform->scale = glm::vec3(0.75f);
        transform->position = { 0.0f, 0.0f, 1.0f };

        auto physicalState = playerEntity->add<PhysicalState>();
        physicalState->previousPosition = glm::vec2(transform->position);
        physicalState->currentPosition = physicalState->previousPosition;

        playerEntity->add<Velocity>();

        auto circleCollider = playerEntity->add<CircleCollider>();
        circleCollider->radius = 0.25;

        auto drawable = playerEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Triangle;
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });

        playerEntity->add<Party>();
      }

      {
        auto castEntity = createEntity();

        auto transform = castEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, -176.0f, 0.0f);
        transform->scale = glm::vec3(192.0f, 20.0f, 1.0f);

        auto progressable = castEntity->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->meshType = Quad;
        progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature), UI);
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });
        progressable->renderable->setEnabled(false);

        castEntity->add<Cast>();
      }

      {
        auto targetIndicatorEntity = createEntity();
        targetIndicatorEntity->add<TargetIndicator>();

        auto transform = targetIndicatorEntity->add<Transform>();
        transform->scale = glm::vec3(68.0f, 68.0f, 0.0f);

        auto drawable = targetIndicatorEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Quad;
        drawable->feature->color = { 0.35f, 0.35f, 0.35f };
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature), UI);
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });
        drawable->renderable->setEnabled(false);
      }

      {
        auto gcdEntity = createEntity();

        auto globalCooldown = gcdEntity->add<GlobalCooldown>();
        globalCooldown->elapsed = 1.5f;
        globalCooldown->total = 1.5f;
      }

      {
        const auto Y = true;
        const auto N = false;

        std::array<std::array<bool, 12>, 12> tiles = {
            std::array<bool, 12> { N, N, N, N, N, N, N, N, N, N, N, Y },
            std::array<bool, 12> { N, N, N, N, N, N, N, N, N, N, N, Y },
            std::array<bool, 12> { N, N, N, N, N, N, N, N, N, N, N, Y },
            std::array<bool, 12> { N, N, N, N, N, N, N, N, N, N, N, Y },
            std::array<bool, 12> { N, N, N, N, N, N, N, N, N, N, N, Y },
            std::array<bool, 12> { N, N, N, N, N, N, N, N, N, N, N, Y },
            std::array<bool, 12> { Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y },
            std::array<bool, 12> { N, N, N, N, N, N, N, N, N, N, N, N },
            std::array<bool, 12> { N, N, N, N, N, N, N, N, N, N, N, N },
            std::array<bool, 12> { N, N, N, N, N, N, N, N, N, N, N, N },
            std::array<bool, 12> { N, N, N, N, N, N, N, N, N, N, N, N },
            std::array<bool, 12> { N, N, N, N, N, N, N, N, N, N, N, N }
        };

        std::array<glm::vec2, 4> mods = {
            glm::vec2 { -1.0f,  1.0f },
            glm::vec2 {  1.0f,  1.0f },
            glm::vec2 { -1.0f, -1.0f },
            glm::vec2 {  1.0f, -1.0f }
        };

        for (auto i = 0; i < tiles.size(); ++i) {
          for (auto j = 0; j < tiles[i].size(); ++j) {
            if (tiles[i][j]) {
              for (auto& mod : mods) {
                auto immovableEntity = createEntity();

                auto transform = immovableEntity->add<Transform>();
                transform->position = {
                    mod.x * (static_cast<float>(j) + 0.5f),
                    mod.y * (static_cast<float>(i) + 0.5f),
                    5.0f
                };

                auto physicalState = immovableEntity->add<PhysicalState>();
                physicalState->previousPosition = glm::vec2(transform->position);
                physicalState->currentPosition = physicalState->previousPosition;

                immovableEntity->add<BoxCollider>();
                immovableEntity->add<Static>();

                _grid->addObstruction(_grid->getGridPosition(physicalState->currentPosition), { 1, 1 });

                auto drawable = immovableEntity->add<Drawable>();
                drawable->feature = new ColoredFeature();
                drawable->feature->meshType = Quad;
                drawable->feature->color = {0.76f, 0.76f, 0.76f};
                drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
                drawable.setRemovalListener([drawable](const Entity e) {
                  drawable->renderable->destroy();
                });
              }
            }
          }
        }
      }

      // Instant-cast Wisp
      {
        auto wispEntity = createEntity();

        auto transform = wispEntity->add<Transform>();
        transform->scale = glm::vec3(0.25f);
        transform->position = { -5.0f, 3.0f, 1.0f };

        auto physicalState = wispEntity->add<PhysicalState>();
        physicalState->previousPosition = glm::vec2(transform->position);
        physicalState->currentPosition = physicalState->previousPosition;

        wispEntity->add<Static>();

        auto collider = wispEntity->add<BoxCollider>();
        collider->size = transform->scale;

        auto drawable = wispEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Quad;
        drawable->feature->color = { 1.0f, 1.0f, 0.0f };
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });

        {
          auto triggerEntity = createEntity();
          triggerEntity->add<Trigger>();

          triggerEntity->add<Ability>(_spellDatabase->getSpellById(0));

          auto health = triggerEntity->add<Health>(500);
          health->current = 150;

          auto triggerPhysicalState = triggerEntity->add<PhysicalState>();
          triggerPhysicalState->previousPosition = physicalState->currentPosition;
          triggerPhysicalState->currentPosition = triggerPhysicalState->previousPosition;

          auto circleCollider = triggerEntity->add<CircleCollider>();
          circleCollider->radius = 2.0f;
        }
      }

      // Hard-cast Wisp
      {
        auto wispEntity = createEntity();

        auto transform = wispEntity->add<Transform>();
        transform->scale = glm::vec3(0.25f);
        transform->position = { 0.0f, 3.0f, 1.0f };

        auto physicalState = wispEntity->add<PhysicalState>();
        physicalState->previousPosition = glm::vec2(transform->position);
        physicalState->currentPosition = physicalState->previousPosition;

        wispEntity->add<Static>();

        auto collider = wispEntity->add<BoxCollider>();
        collider->size = transform->scale;

        auto drawable = wispEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Quad;
        drawable->feature->color = { 0.0f, 1.0f, 0.0f };
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });

        {
          auto triggerEntity = createEntity();
          triggerEntity->add<Trigger>();

          triggerEntity->add<Ability>(_spellDatabase->getSpellById(1));

          auto health = triggerEntity->add<Health>(400);
          health->current = 150;

          auto triggerPhysicalState = triggerEntity->add<PhysicalState>();
          triggerPhysicalState->previousPosition = physicalState->currentPosition;
          triggerPhysicalState->currentPosition = triggerPhysicalState->previousPosition;

          auto circleCollider = triggerEntity->add<CircleCollider>();
          circleCollider->radius = 2.0f;
        }
      }

      // HoT Wisp
      {
        auto wispEntity = createEntity();

        auto transform = wispEntity->add<Transform>();
        transform->scale = glm::vec3(0.25f);
        transform->position = { 5.0f, 3.0f, 1.0f };

        auto physicalState = wispEntity->add<PhysicalState>();
        physicalState->previousPosition = glm::vec2(transform->position);
        physicalState->currentPosition = physicalState->previousPosition;

        wispEntity->add<Static>();

        auto collider = wispEntity->add<BoxCollider>();
        collider->size = transform->scale;

        auto drawable = wispEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Quad;
        drawable->feature->color = { 1.0f, 0.0f, 0.0f };
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });

        {
          auto triggerEntity = createEntity();
          triggerEntity->add<Trigger>();

          triggerEntity->add<Ability>(_spellDatabase->getSpellById(2));

          auto health = triggerEntity->add<Health>(350);
          health->current = 150;

          auto triggerPhysicalState = triggerEntity->add<PhysicalState>();
          triggerPhysicalState->previousPosition = physicalState->currentPosition;
          triggerPhysicalState->currentPosition = triggerPhysicalState->previousPosition;

          auto circleCollider = triggerEntity->add<CircleCollider>();
          circleCollider->radius = 2.0f;
        }
      }

      auto tutorialStateEntity = createEntity();
      tutorialStateEntity->add<TutorialState>();

      {
        auto tutorialTextEntity1 = createEntity();
        tutorialTextEntity1->add<TutorialText1>();

        auto transform = tutorialTextEntity1->add<Transform>();
        transform->scale = glm::vec3(20.0f);
        transform->position = { -340.0f, -210.0f, 1.0f };

        auto text = tutorialTextEntity1->add<Text>();
        text->feature = new TextFeature();
        text->feature->text = " Hover over a\n health bar to\nchoose a target";
        text->feature->color = { 0.0f, 0.0f, 0.0f };
        text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
        text.setRemovalListener([text](const Entity e) {
          text->renderable->destroy();
        });
        text->renderable->setEnabled(false);
      }

      {
        auto tutorialTextEntity2 = createEntity();
        tutorialTextEntity2->add<TutorialText2>();

        auto transform = tutorialTextEntity2->add<Transform>();
        transform->scale = glm::vec3(20.0f);
        transform->position = { 40.0f, -290.0f, 1.0f };

        auto text = tutorialTextEntity2->add<Text>();
        text->feature = new TextFeature();
        text->feature->text = " While hovering\n over a target,\n press a hotkey\nto use an ability";
        text->feature->color = { 0.0f, 0.0f, 0.0f };
        text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
        text.setRemovalListener([text](const Entity e) {
          text->renderable->destroy();
        });
        text->renderable->setEnabled(false);
      }
    }

  private:
    std::unique_ptr<Grid> _grid = std::make_unique<Grid>(1.0f);
    std::unique_ptr<SpellDatabase> _spellDatabase;
};

}  // namespace linguine
