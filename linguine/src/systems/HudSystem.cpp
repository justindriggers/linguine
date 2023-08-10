#include "HudSystem.h"

#include "components/Ability.h"
#include "components/AbilityButton.h"
#include "components/HealthBar.h"
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

    auto healthBars = findEntities<HealthBar>()->get();
    auto count = healthBars.size();

    while (count < party->memberIds.size()) {
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

      ++count;
    }

    while (count > party->memberIds.size()) {
      healthBars.begin()->get()->destroy();
      --count;
    }

    auto abilityButtons = findEntities<AbilityButton>()->get();
    count = abilityButtons.size();

    while (count < party->memberIds.size()) {
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

      ++count;
    }

    while (count > party->memberIds.size()) {
      abilityButtons.begin()->get()->destroy();
      --count;
    }

    healthBars = findEntities<HealthBar>()->get();
    abilityButtons = findEntities<AbilityButton>()->get();

    for (auto i = 0; i < party->memberIds.size(); ++i) {
      auto memberEntity = getEntityById(party->memberIds[i]);
      auto healthBarEntity = healthBars[i];

      auto healthBar = healthBarEntity->add<HealthBar>();
      healthBar->entityId = memberEntity->getId();

      auto healthTransform = healthBarEntity->get<Transform>();
      healthTransform->position = glm::vec3((-static_cast<float>(count) / 2.0f + static_cast<float>(i) + 0.5f) * 68.0f, -230.0f, 5.0f);

      auto abilityButtonEntity = abilityButtons[i];

      auto abilityButton = abilityButtonEntity->get<AbilityButton>();
      abilityButton->abilityEntityId = memberEntity->getId();

      auto abilityTransform = abilityButtonEntity->get<Transform>();
      abilityTransform->position = glm::vec3((-static_cast<float>(count) / 2.0f + static_cast<float>(i) + 0.5f) * 64.0f, -320.0f, 5.0f);

      auto ability = memberEntity->get<Ability>();

      auto abilityProgressable = abilityButtonEntity->get<Progressable>();
      abilityProgressable->feature->color = ability->spell.color;

      auto textEntity = getEntityById(abilityButton->textEntityId);

      auto textTransform = textEntity->get<Transform>();
      textTransform->position = glm::vec3((-static_cast<float>(count) / 2.0f + static_cast<float>(i) + 0.5f) * 64.0f, -330.0f, 0.0f);

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
}

}  // namespace linguine
