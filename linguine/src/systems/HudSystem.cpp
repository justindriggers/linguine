#include "HudSystem.h"

#include "components/Ability.h"
#include "components/AbilityButton.h"
#include "components/Friendly.h"
#include "components/HealthBar.h"
#include "components/Hostile.h"
#include "components/Hovered.h"
#include "components/HudDetails.h"
#include "components/Party.h"
#include "components/Player.h"
#include "components/Progressable.h"
#include "components/Selectable.h"
#include "components/Text.h"
#include "components/Tooltip.h"
#include "components/Transform.h"
#include "components/UnitType.h"

namespace linguine {

void HudSystem::update(float deltaTime) {
  findEntities<Player, Party>()->each([this](const Entity& entity) {
    auto party = entity.get<Party>();

    findEntities<Friendly, HudDetails>()->each([this, &party](Entity& hudDetailsEntity) {
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
        healthEntity->add<Friendly>();
        healthEntity->add<HealthBar>();

        auto healthTransform = healthEntity->add<Transform>();
        healthTransform->scale = glm::vec3(40.0f, 40.0f, 0.0f);

        auto healthProgressable = healthEntity->add<Progressable>();
        healthProgressable->feature = new ProgressFeature();
        healthProgressable->feature->meshType = Quad;

        switch (memberEntity->get<UnitType>()->type) {
        case Fire:
          healthProgressable->feature->color = { 1.0f, 0.0f, 0.0f };
          break;
        case Water:
          healthProgressable->feature->color = { 0.0f, 0.0f, 1.0f };
          break;
        case Storm:
          healthProgressable->feature->color = { 0.0f, 1.0f, 1.0f };
          break;
        case Earth:
          healthProgressable->feature->color = { 1.0f, 1.0f, 0.0f };
          break;
        case Growth:
          healthProgressable->feature->color = { 0.0f, 1.0f, 0.0f };
          break;
        case Decay:
          healthProgressable->feature->color = { 1.0f, 0.0f, 1.0f };
          break;
        }

        healthProgressable->feature->backgroundColor = { 0.00972f, 0.04667f, 0.04971f, 1.0f };
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
        transform->scale = glm::vec3(40.0f, 40.0f, 0.0f);

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
        abilityEntity->add<Friendly>();

        auto spellName = createEntity();

        auto spellNameTransform = spellName->add<Transform>();
        spellNameTransform->scale = glm::vec3(5.0f, 5.0f, 0.0f);

        auto text = spellName->add<Text>();
        text->feature = new TextFeature();
        text->feature->color = { 1.0f, 1.0f, 1.0f };
        text->renderable = _renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
        text.setRemovalListener([text](const Entity e) {
          text->renderable->destroy();
        });

        abilityButton->textEntityId = spellName->getId();

        hudDetails->abilityButtonId = abilityEntity->getId();
      }

      auto hudDetails = memberEntity->get<HudDetails>();

      auto healthBarEntity = getEntityById(hudDetails->healthBarId);

      auto healthBar = healthBarEntity->get<HealthBar>();
      healthBar->entityId = memberEntity->getId();

      auto healthTransform = healthBarEntity->get<Transform>();
      healthTransform->position = glm::vec3(172.0f, (-static_cast<float>(count) / 2.0f + static_cast<float>(i) + 0.5f) * 46.0f, 5.0f);

      auto abilityButtonEntity = getEntityById(hudDetails->abilityButtonId);

      auto abilityButton = abilityButtonEntity->get<AbilityButton>();
      abilityButton->abilityEntityId = memberEntity->getId();

      auto abilityTransform = abilityButtonEntity->get<Transform>();
      abilityTransform->position = glm::vec3((-static_cast<float>(count) / 2.0f + static_cast<float>(i) + 0.5f) * 48.0f, -76.0f, 5.0f);

      auto ability = memberEntity->get<Ability>();

      auto abilityProgressable = abilityButtonEntity->get<Progressable>();

      switch (ability->spell.type) {
      case Fire:
        abilityProgressable->feature->color = { 1.0f, 0.0f, 0.0f };
        break;
      case Water:
        abilityProgressable->feature->color = { 0.0f, 0.0f, 1.0f };
        break;
      case Storm:
        abilityProgressable->feature->color = { 0.0f, 1.0f, 1.0f };
        break;
      case Earth:
        abilityProgressable->feature->color = { 1.0f, 1.0f, 0.0f };
        break;
      case Growth:
        abilityProgressable->feature->color = { 0.0f, 1.0f, 0.0f };
        break;
      case Decay:
        abilityProgressable->feature->color = { 1.0f, 0.0f, 1.0f };
        break;
      }

      auto textEntity = getEntityById(abilityButton->textEntityId);

      auto textTransform = textEntity->get<Transform>();
      textTransform->position = glm::vec3((-static_cast<float>(count) / 2.0f + static_cast<float>(i) + 0.5f) * 48.0f - ability->spell.name.length() / 2.0f * 5.0f + 2.5f, -109.5f, 0.1f);

      auto text = textEntity->get<Text>();
      text->feature->text = ability->spell.name;

      switch (i) {
      case 0:
        abilityButton->key = Key::Q;
        break;
      case 1:
        abilityButton->key = Key::E;
        break;
      case 2:
        abilityButton->key = Key::R;
        break;
      case 3:
        abilityButton->key = Key::F;
        break;
      case 4:
        abilityButton->key = Key::C;
        break;
      default:
        throw std::runtime_error("Invalid party member count");
      }
    }
  });

  findEntities<Hostile, Party>()->each([this](const Entity& entity) {
    auto party = entity.get<Party>();

    findEntities<Hostile, HudDetails>()->each([this, &party](Entity& hudDetailsEntity) {
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
        healthEntity->add<Hostile>();
        healthEntity->add<HealthBar>();

        auto healthTransform = healthEntity->add<Transform>();
        healthTransform->scale = glm::vec3(40.0f, 40.0f, 0.0f);

        auto healthProgressable = healthEntity->add<Progressable>();
        healthProgressable->feature = new ProgressFeature();
        healthProgressable->feature->meshType = Quad;

        switch (memberEntity->get<UnitType>()->type) {
        case Fire:
          healthProgressable->feature->color = { 1.0f, 0.0f, 0.0f };
          break;
        case Water:
          healthProgressable->feature->color = { 0.0f, 0.0f, 1.0f };
          break;
        case Storm:
          healthProgressable->feature->color = { 0.0f, 1.0f, 1.0f };
          break;
        case Earth:
          healthProgressable->feature->color = { 1.0f, 1.0f, 0.0f };
          break;
        case Growth:
          healthProgressable->feature->color = { 0.0f, 1.0f, 0.0f };
          break;
        case Decay:
          healthProgressable->feature->color = { 1.0f, 0.0f, 1.0f };
          break;
        }

        healthProgressable->feature->backgroundColor = { 0.04971f, 0.02122f, 0.0319f, 1.0f };
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
      healthTransform->position = glm::vec3(-172.0f, (-static_cast<float>(count) / 2.0f + static_cast<float>(i) + 0.5f) * 46.0f, 5.0f);
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
