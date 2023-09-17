#pragma once

#include "Scene.h"

#include "components/Ability.h"
#include "components/Alive.h"
#include "components/Attachment.h"
#include "components/CameraFixture.h"
#include "components/Cast.h"
#include "components/Circle.h"
#include "components/Drawable.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/Health.h"
#include "components/Hostile.h"
#include "components/Party.h"
#include "components/Player.h"
#include "components/Progressable.h"
#include "components/Selectable.h"
#include "components/TargetIndicator.h"
#include "components/Text.h"
#include "components/Tooltip.h"
#include "components/Transform.h"
#include "components/Trigger.h"
#include "components/Unit.h"
#include "systems/CameraSystem.h"
#include "systems/CastSystem.h"
#include "systems/CooldownProgressSystem.h"
#include "systems/EffectSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/HealthProgressSystem.h"
#include "systems/HudSystem.h"
#include "systems/PlayerControllerSystem.h"
#include "systems/TooltipSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class BattleScene : public Scene {
  public:
    explicit BattleScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()),
          _spellDatabase(std::make_unique<SpellDatabase>(serviceLocator, getEntityManager())) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<PlayerControllerSystem>(getEntityManager(), *_spellDatabase, serviceLocator.get<InputManager>()));
      registerSystem(std::make_unique<EffectSystem>(getEntityManager(), *_spellDatabase));
      registerSystem(std::make_unique<HudSystem>(getEntityManager(), serviceLocator.get<Renderer>()));
      registerSystem(std::make_unique<HealthProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<CooldownProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<CastSystem>(getEntityManager()));
      registerSystem(std::make_unique<TooltipSystem>(getEntityManager(), serviceLocator.get<Renderer>(), serviceLocator.get<InputManager>()));

      // Scene-specific

      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();

      {
        auto cameraEntity = createEntity();

        auto transform = cameraEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

        auto fixture = cameraEntity->add<CameraFixture>();
        fixture->height = 20.0f;
        fixture->camera = renderer.createCamera();
        fixture->camera->clearColor = glm::vec3(0.00972f, 0.04667f, 0.04971f);
      }

      {
        auto uiCameraEntity = createEntity();

        auto transform = uiCameraEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

        auto fixture = uiCameraEntity->add<CameraFixture>();
        fixture->height = 240.0f;
        fixture->camera = renderer.createCamera();
        fixture->camera->clearColor = {};
        fixture->camera->layer = UI;
      }

      {
        auto tooltipEntity = createEntity();
        tooltipEntity->add<Tooltip>();
      }

      {
        auto playerEntity = createEntity();
        playerEntity->add<Player>();
        playerEntity->add<Friendly>();
        playerEntity->add<Unit>();
        playerEntity->add<Alive>();

        auto party = playerEntity->add<Party>();

        // Instant-cast Wisp
        {
          auto wispEntity = createEntity();
          wispEntity->add<Friendly>();

          wispEntity->add<Ability>(_spellDatabase->getSpellById(0));

          auto health = wispEntity->add<Health>(500);
          health->current = 150;

          party->memberIds.push_back(wispEntity->getId());
        }

        // Hard-cast Wisp
        {
          auto wispEntity = createEntity();
          wispEntity->add<Friendly>();

          wispEntity->add<Ability>(_spellDatabase->getSpellById(1));

          auto health = wispEntity->add<Health>(400);
          health->current = 150;

          party->memberIds.push_back(wispEntity->getId());
        }

        // HoT Wisp
        {
          auto wispEntity = createEntity();
          wispEntity->add<Friendly>();

          wispEntity->add<Ability>(_spellDatabase->getSpellById(2));

          auto health = wispEntity->add<Health>(350);
          health->current = 150;

          party->memberIds.push_back(wispEntity->getId());
        }

        // DoT Wisp
        {
          auto wispEntity = createEntity();
          wispEntity->add<Friendly>();

          wispEntity->add<Ability>(_spellDatabase->getSpellById(3));

          auto health = wispEntity->add<Health>(350);
          health->current = 150;

          party->memberIds.push_back(wispEntity->getId());
        }
      }

      {
        auto enemyEntity = createEntity();
        enemyEntity->add<Hostile>();
        enemyEntity->add<Unit>();
        enemyEntity->add<Alive>();

        auto party = enemyEntity->add<Party>();

        // Hard-cast Wisp
        {
          auto wispEntity = createEntity();
          wispEntity->add<Hostile>();

          wispEntity->add<Ability>(_spellDatabase->getSpellById(1));

          auto health = wispEntity->add<Health>(400);
          health->current = 150;

          party->memberIds.push_back(wispEntity->getId());
        }

        // Instant-cast Wisp
        {
          auto wispEntity = createEntity();
          wispEntity->add<Hostile>();

          wispEntity->add<Ability>(_spellDatabase->getSpellById(0));

          auto health = wispEntity->add<Health>(500);
          health->current = 150;

          party->memberIds.push_back(wispEntity->getId());
        }

        // HoT Wisp
        {
          auto wispEntity = createEntity();
          wispEntity->add<Hostile>();

          wispEntity->add<Ability>(_spellDatabase->getSpellById(2));

          auto health = wispEntity->add<Health>(350);
          health->current = 150;

          party->memberIds.push_back(wispEntity->getId());
        }
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
        transform->scale = glm::vec3(44.0f, 44.0f, 0.0f);

        auto drawable = targetIndicatorEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Quad;
        drawable->feature->color = { 0.42869f, 0.521f, 0.64448f };
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
        auto playerBackgroundEntity = createEntity();

        auto transform = playerBackgroundEntity->add<Transform>();
        transform->position = { 172.0f, 0.0f, 10.0f };
        transform->scale = { 56.0f, 240.0f, 0.0f };

        auto drawable = playerBackgroundEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Quad;
        drawable->feature->color = { 0.00121f, 0.23074f, 0.6376f };
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature), UI);
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });
      }

      {
        auto opponentBackgroundEntity = createEntity();

        auto transform = opponentBackgroundEntity->add<Transform>();
        transform->position = { -172.0f, 0.0f, 10.0f };
        transform->scale = { 56.0f, 240.0f, 0.0f };

        auto drawable = opponentBackgroundEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Quad;
        drawable->feature->color = { 0.96469f, 0.28744f, 0.02416f };
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature), UI);
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });
      }

      {
        auto playerAbilityBackgroundEntity = createEntity();

        auto transform = playerAbilityBackgroundEntity->add<Transform>();
        transform->position = { 0.0f, -84.0f, 10.0f };
        transform->scale = { 288.0f, 72.0f, 0.0f };

        auto drawable = playerAbilityBackgroundEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Quad;
        drawable->feature->color = { 0.00121f, 0.23074f, 0.6376f };
//        drawable->feature->color = { 0.96469f, 0.28744f, 0.02416f };
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature), UI);
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });
      }

      {
        auto circleEntity = createEntity();

        auto transform = circleEntity->add<Transform>();
        transform->scale = { 40.0f, 40.0f, 0.0f };

        auto circle = circleEntity->add<Circle>();
        circle->feature = new CircleFeature();
        circle->renderable = renderer.create(std::unique_ptr<CircleFeature>(circle->feature), UI);
        circle.setRemovalListener([circle](const Entity e) {
          circle->renderable->destroy();
        });
      }
    }

  private:
    std::unique_ptr<SpellDatabase> _spellDatabase;
};

}  // namespace linguine
