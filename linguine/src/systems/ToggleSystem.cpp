#include "ToggleSystem.h"

#include <glm/common.hpp>

#include "components/Button.h"
#include "components/Toggle.h"

namespace linguine {

void ToggleSystem::update(float deltaTime) {
  findEntities<Toggle>()->each([this](Entity& entity) {
    auto toggle = entity.get<Toggle>();
    auto toggleEntityId = entity.getId();

    if (!entity.has<ToggleMetadata>()) {
      auto toggleMetadata = entity.add<ToggleMetadata>();

      auto onButtonEntity = createEntity();
      auto onButtonEntityId = onButtonEntity->getId();
      toggleMetadata->onButtonEntityId = onButtonEntityId;

      auto onButton = onButtonEntity->add<Button>();
      onButton->clickHandler = [this, toggleEntityId]() {
        auto toggleEntity = getEntityById(toggleEntityId);
        auto toggle = toggleEntity->get<Toggle>();
        toggle->isEnabled = !toggle->isEnabled;

        toggle->toggleHandler(toggle->isEnabled);
      };

      auto offButtonEntity = createEntity();
      auto offButtonEntityId = offButtonEntity->getId();
      toggleMetadata->offButtonEntityId = offButtonEntityId;

      auto offButton = offButtonEntity->add<Button>();
      offButton->clickHandler = onButton->clickHandler;
    }

    auto toggleMetadata = entity.get<ToggleMetadata>();

    auto onOffset = glm::max(toggle->minSize.x / 4.0f, toggle->textSize * static_cast<float>(toggle->onText.size()) / 2.0f);

    auto onButtonEntity = getEntityById(toggleMetadata->onButtonEntityId);
    auto onButton = onButtonEntity->get<Button>();
    onButton->text = toggle->onText;
    onButton->textSize = toggle->textSize;
    onButton->textColor = toggle->onTextColor;
    onButton->position = { toggle->position.x + onOffset, toggle->position.y, toggle->position.z };
    onButton->minSize = { toggle->minSize.x / 2.0f, toggle->minSize.y };

    auto offOffset = -glm::max(toggle->minSize.x / 4.0f, toggle->textSize * static_cast<float>(toggle->offText.size()) / 2.0f);

    auto offButtonEntity = getEntityById(toggleMetadata->offButtonEntityId);
    auto offButton = offButtonEntity->get<Button>();
    offButton->text = toggle->offText;
    offButton->textSize = toggle->textSize;
    offButton->textColor = toggle->offTextColor;
    offButton->position = { toggle->position.x + offOffset, toggle->position.y, toggle->position.z };
    offButton->minSize = { toggle->minSize.x / 2.0f, toggle->minSize.y };

    if (toggle->isEnabled) {
      onButton->color = Palette::Primary;
      onButton->activeColor = Palette::PrimaryAccent;

      offButton->color = Palette::Secondary;
      offButton->activeColor = Palette::SecondaryAccent;
    } else {
      onButton->color = Palette::Secondary;
      onButton->activeColor = Palette::SecondaryAccent;

      offButton->color = Palette::Primary;
      offButton->activeColor = Palette::PrimaryAccent;
    }
  });
}

}  // namespace linguine
