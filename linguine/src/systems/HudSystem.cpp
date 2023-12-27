#include "HudSystem.h"

#include "components/Ability.h"
#include "components/AbilityLabel.h"
#include "components/GameOver.h"
#include "components/HealthBar.h"
#include "components/HudDetails.h"
#include "components/Party.h"
#include "components/Player.h"
#include "components/Progressable.h"
#include "components/Selectable.h"
#include "components/Text.h"
#include "components/Transform.h"

namespace linguine {

void HudSystem::update(float deltaTime) {
  findEntities<Player, Party>()->each([this](const Entity& entity) {
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

    auto count = party->memberIds.size();

    for (auto i = 0; i < count; ++i) {
      auto memberEntity = getEntityById(party->memberIds[i]);

      if (!memberEntity->has<HudDetails>()) {
        auto hudDetails = memberEntity->add<HudDetails>();

        auto healthEntity = createEntity();
        healthEntity->add<HealthBar>();

        auto healthTransform = healthEntity->add<Transform>();
        healthTransform->scale = glm::vec3(40.0f, 40.0f, 0.0f);
        healthTransform->rotation = glm::angleAxis(glm::pi<float>() / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));

        auto healthProgressable = healthEntity->add<Progressable>();
        healthProgressable->feature = new ProgressFeature();
        healthProgressable->feature->meshType = Quad;
        healthProgressable->renderable = _renderer.create(std::unique_ptr<ProgressFeature>(healthProgressable->feature), UI);
        healthProgressable.setRemovalListener([healthProgressable](const Entity e) {
          healthProgressable->renderable->destroy();
        });

        auto healthSelectable = healthEntity->add<Selectable>();
        healthSelectable->feature = new SelectableFeature();
        healthSelectable->feature->meshType = Quad;
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
      healthTransform->position = glm::vec3((-static_cast<float>(count) / 2.0f + static_cast<float>(i) + 0.5f) * 46.0f, -128.0f, 5.0f);
    }
  });

  findEntities<Text, AbilityLabel>()->each([this](const Entity& entity) {
    auto text = entity.get<Text>();

    auto isGameOver = false;

    findEntities<Player>()->each([&isGameOver](const Entity& playerEntity) {
      isGameOver = playerEntity.has<GameOver>();
    });

    auto abilityLabel = entity.get<AbilityLabel>();
    auto abilityEntity = getEntityById(abilityLabel->abilityId);
    auto progressable = abilityEntity->get<Progressable>();

    if (isGameOver) {
      text->feature->text = "Signal Lost...";
      progressable->feature->color = { 1.0f, 0.0f, 0.0f };
    } else {
      auto ability = abilityEntity->get<Ability>();

      if (ability->remainingCooldown > 0.0f) {
        text->feature->text = "Recharging";
      } else {
        text->feature->text = "Ready";
      }

      auto progress = 1.0f - ability->remainingCooldown / ability->spell.cooldown;
      auto red = 1.0f - (progress > 0.5 ? (progress - 0.5f) * 2.0f : 0.0f);
      auto green = 0.0f + (progress < 0.5 ? progress * 2.0f : 1.0f);
      progressable->feature->color = { red, green, 0.0f };
    }
  });
}

}  // namespace linguine
