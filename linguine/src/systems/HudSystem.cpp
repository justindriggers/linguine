#include "HudSystem.h"

#include "components/Ability.h"
#include "components/Attachment.h"
#include "components/Drawable.h"
#include "components/Enqueued.h"
#include "components/GameOver.h"
#include "components/HealthBar.h"
#include "components/HudDetails.h"
#include "components/Lives.h"
#include "components/Party.h"
#include "components/Player.h"
#include "components/Progressable.h"
#include "components/Selectable.h"
#include "components/ShieldLabel.h"
#include "components/Text.h"
#include "components/Transform.h"

namespace linguine {

void HudSystem::update(float deltaTime) {
  auto partySize = 0;

  findEntities<Player, Party>()->each([this, &partySize](const Entity& entity) {
    auto party = entity.get<Party>();

    findEntities<HudDetails>()->each([this, &party](Entity& hudDetailsEntity) {
      auto hudDetails = hudDetailsEntity.get<HudDetails>();

      auto isPartyMember = std::find(party->memberIds.begin(), party->memberIds.end(), hudDetailsEntity.getId()) != party->memberIds.end();

      if (!isPartyMember) {
        auto healthBarEntity = getEntityById(hudDetails->healthBarId);
        healthBarEntity->destroy();

        hudDetailsEntity.remove<HudDetails>();
      }
    });

    partySize = static_cast<int>(party->memberIds.size());

    for (auto i = 0; i < partySize; ++i) {
      auto memberEntity = getEntityById(party->memberIds[i]);

      if (!memberEntity->has<HudDetails>()) {
        auto hudDetails = memberEntity->add<HudDetails>();

        auto healthEntity = createEntity();
        healthEntity->add<HealthBar>();

        auto healthTransform = healthEntity->add<Transform>();
        healthTransform->scale = glm::vec3(40.0f, 40.0f, 0.0f);

        auto healthProgressable = healthEntity->add<Progressable>();
        healthProgressable->feature = new ProgressFeature();
        healthProgressable->feature->meshType = MeshType::Quad;
        healthProgressable->renderable = _renderer.create(std::unique_ptr<ProgressFeature>(healthProgressable->feature), UI);
        healthProgressable.setRemovalListener([healthProgressable](const Entity e) {
          healthProgressable->renderable->destroy();
        });

        auto healthSelectable = healthEntity->add<Selectable>();
        healthSelectable->feature = new SelectableFeature();
        healthSelectable->feature->meshType = MeshType::Quad;
        healthSelectable->feature->entityId = healthEntity->getId();
        healthSelectable->renderable = _renderer.create(std::unique_ptr<SelectableFeature>(healthSelectable->feature), UI);
        healthSelectable.setRemovalListener([healthSelectable](const Entity e) {
          healthSelectable->renderable->destroy();
        });

        hudDetails->healthBarId = healthEntity->getId();
      }

      auto hudDetails = memberEntity->get<HudDetails>();

      auto healthBarEntity = getEntityById(hudDetails->healthBarId);

      auto healthBar = healthBarEntity->get<HealthBar>();
      healthBar->entityId = memberEntity->getId();

      auto healthTransform = healthBarEntity->get<Transform>();
      healthTransform->position = glm::vec3(0.0f, 0.0f, 5.0f);

      switch (i) {
      case 0:
        healthTransform->position.y = -23.0f;
        break;
      case 1:
        healthTransform->position.y = -69.0f;
        break;
      case 2:
        healthTransform->position.y = 23.0f;
        break;
      case 3:
        healthTransform->position.y = -115.0f;
        break;
      case 4:
        healthTransform->position.y = 69.0f;
        break;
      default:
        healthTransform->position.y = (static_cast<float>(partySize) / 2.0f - static_cast<float>(i) - 0.5f) * 46.0f - 23.0f;
        break;
      }

      switch (_saveManager.getHandedness()) {
      case SaveManager::Handedness::Right:
        healthTransform->position.x = 92.0f;
        break;
      case SaveManager::Handedness::Left:
        healthTransform->position.x = -92.0f;
        break;
      }
    }
  });

  findEntities<Ability, Progressable, Transform>()->each([this, partySize](const Entity& entity) {
    auto transform = entity.get<Transform>();
    transform->scale.x = static_cast<float>(partySize) * 40.0f + static_cast<float>(partySize - 1) * 6.0f;

    if (partySize % 2 == 0) {
      transform->position.y = -46.0f;
    } else {
      transform->position.y = -23.0f;
    }

    auto isGameOver = false;

    findEntities<Player>()->each([&isGameOver](const Entity& playerEntity) {
      isGameOver = playerEntity.has<GameOver>();
    });

    auto progressable = entity.get<Progressable>();

    if (isGameOver) {
      progressable->feature->color = { 1.0f, 0.0f, 0.0f };
    } else {
      auto ability = entity.get<Ability>();

      auto progress = 1.0f - ability->remainingCooldown / ability->spell.cooldown;
      auto red = 1.0f - (progress > 0.5 ? (progress - 0.5f) * 2.0f : 0.0f);
      auto green = 0.0f + (progress < 0.5 ? progress * 2.0f : 1.0f);
      progressable->feature->color = { red, green, 0.0f };
    }
  });

  findEntities<ShieldLabel, Transform>()->each([partySize](const Entity& entity) {
    auto transform = entity.get<Transform>();

    if (partySize % 2 == 0) {
      transform->position.y = (static_cast<float>(partySize + 1) / 2.0f - static_cast<float>(partySize) - 0.5f) * 46.0f - 51.0f;
    } else {
      transform->position.y = (static_cast<float>(partySize) / 2.0f - static_cast<float>(partySize - 1) - 0.5f) * 46.0f - 51.0f;
    }
  });

  findEntities<Lives, Text, Attachment>()->each([this](const Entity& entity) {
    auto lives = entity.get<Lives>();

    auto text = entity.get<Text>();
    text->feature->text = std::to_string(lives->lives);

    auto attachment = entity.get<Attachment>();
    attachment->offset.x = 103.0f - static_cast<float>(text->feature->text.size() - 1) * 10.0f;

    auto iconEntity = getEntityById(lives->iconId);
    auto iconAttachment = iconEntity->get<Attachment>();
    iconAttachment->offset.x = attachment->offset.x - 16.0f;

    auto width = 18.0f + static_cast<float>(text->feature->text.size()) * 10.0f;

    auto backgroundEntity = getEntityById(lives->backgroundId);
    auto backgroundAttachment = backgroundEntity->get<Attachment>();
    backgroundAttachment->offset.x = iconAttachment->offset.x + width / 2.0f - 7.0f;

    auto backgroundTransform = backgroundEntity->get<Transform>();
    backgroundTransform->scale.x = width + 8.0f;
  });

  findEntities<Enqueued, Drawable, Transform>()->each([this](const Entity& entity) {
    auto enqueued = entity.get<Enqueued>();
    auto drawable = entity.get<Drawable>();

    if (enqueued->healthBarId) {
      auto healthBarEntity = getEntityById(*enqueued->healthBarId);
      auto healthBarTransform = healthBarEntity->get<Transform>();

      auto transform = entity.get<Transform>();
      transform->position.x = healthBarTransform->position.x;
      transform->position.y = healthBarTransform->position.y;

      drawable->renderable->setEnabled(true);
    } else {
      drawable->renderable->setEnabled(false);
    }
  });
}

}  // namespace linguine
