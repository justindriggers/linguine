#include "HudSystem.h"

#include "components/Ability.h"
#include "components/AbilityButton.h"
#include "components/HealthBar.h"
#include "components/Hovered.h"
#include "components/HudDetails.h"
#include "components/Party.h"
#include "components/Player.h"
#include "components/Progressable.h"
#include "components/Selectable.h"
#include "components/Text.h"
#include "components/Tooltip.h"
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

        auto abilityButtonEntity = getEntityById(hudDetails->abilityButtonId);
        abilityButtonEntity->destroy();

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
        healthTransform->scale = glm::vec3(64.0f, 64.0f, 0.0f);

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

        auto abilityEntity = createEntity();

        auto transform = abilityEntity->add<Transform>();
        transform->scale = glm::vec3(48.0f, 48.0f, 0.0f);

        auto progressable = abilityEntity->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->color = { 1.0f, 1.0f, 0.0f };
        progressable->feature->meshType = Quad;
        progressable->renderable = _renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature), UI);
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });

        auto selectable = abilityEntity->add<Selectable>();
        selectable->feature = new SelectableFeature();
        selectable->feature->meshType = Quad;
        selectable->feature->entityId = abilityEntity->getId();
        selectable->renderable = _renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature), UI);
        selectable.setRemovalListener([selectable](const Entity e) {
          selectable->renderable->destroy();
        });

        auto abilityButton = abilityEntity->add<AbilityButton>();

        auto keybind = createEntity();

        auto keybindTransform = keybind->add<Transform>();
        keybindTransform->scale = glm::vec3(30.0f, 30.0f, 0.0f);

        auto text = keybind->add<Text>();
        text->feature = new TextFeature();
        text->feature->color = { 0.15f, 0.15f, 0.15f };
        text->renderable = _renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
        text.setRemovalListener([text](const Entity e) {
          text->renderable->destroy();
        });

        abilityButton->textEntityId = keybind->getId();

        hudDetails->abilityButtonId = abilityEntity->getId();
      }

      auto hudDetails = memberEntity->get<HudDetails>();

      auto healthBarEntity = getEntityById(hudDetails->healthBarId);

      auto healthBar = healthBarEntity->get<HealthBar>();
      healthBar->entityId = memberEntity->getId();

      auto healthTransform = healthBarEntity->get<Transform>();
      healthTransform->position = glm::vec3((-static_cast<float>(count) / 2.0f + static_cast<float>(i) + 0.5f) * 68.0f, -230.0f, 5.0f);

      auto abilityButtonEntity = getEntityById(hudDetails->abilityButtonId);

      auto abilityButton = abilityButtonEntity->get<AbilityButton>();
      abilityButton->abilityEntityId = memberEntity->getId();

      auto abilityTransform = abilityButtonEntity->get<Transform>();
      abilityTransform->position = glm::vec3((-static_cast<float>(count) / 2.0f + static_cast<float>(i) + 0.5f) * 64.0f, -320.0f, 5.0f);

      auto ability = memberEntity->get<Ability>();

      auto abilityProgressable = abilityButtonEntity->get<Progressable>();
      abilityProgressable->feature->color = ability->spell.color;

      auto textEntity = getEntityById(abilityButton->textEntityId);

      auto textTransform = textEntity->get<Transform>();
      textTransform->position = glm::vec3((-static_cast<float>(count) / 2.0f + static_cast<float>(i) + 0.5f) * 64.0f, -330.0f, 0.1f);

      auto text = textEntity->get<Text>();

      switch (i) {
      case 0:
        abilityButton->key = Key::Q;
        text->feature->text = "Q";
        break;
      case 1:
        abilityButton->key = Key::E;
        text->feature->text = "E";
        break;
      case 2:
        abilityButton->key = Key::R;
        text->feature->text = "R";
        break;
      case 3:
        abilityButton->key = Key::F;
        text->feature->text = "F";
        break;
      case 4:
        abilityButton->key = Key::C;
        text->feature->text = "C";
        break;
      default:
        throw std::runtime_error("Invalid party member count");
      }
    }
  });

  findEntities<Tooltip>()->each([this](const Entity& tooltipEntity) {
    auto tooltip = tooltipEntity.get<Tooltip>();

    findEntities<AbilityButton, Hovered>()->each([this, &tooltip](const Entity& abilityButtonEntity) {
      auto abilityButton = abilityButtonEntity.get<AbilityButton>();
      auto abilityEntity = getEntityById(abilityButton->abilityEntityId);
      auto ability = abilityEntity->get<Ability>();

      tooltip->setText(ability->spell.tooltip);
    });
  });

}

}  // namespace linguine
