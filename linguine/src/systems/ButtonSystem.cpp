#include "ButtonSystem.h"

#include "components/Button.h"
#include "components/Drawable.h"
#include "components/Pressed.h"
#include "components/Selectable.h"
#include "components/Tapped.h"
#include "components/Text.h"
#include "components/Transform.h"
#include "renderer/features/TextFeature.h"

namespace linguine {

void ButtonSystem::update(float deltaTime) {
  findEntities<Button>()->each([this](Entity& entity) {
    auto button = entity.get<Button>();

    if (!entity.has<ButtonMetadata>()) {
      auto buttonMetadata = entity.add<ButtonMetadata>();
      entity.add<Transform>();

      auto drawable = entity.add<Drawable>();
      drawable->feature = new ColoredFeature();
      drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature), UI);
      drawable.setRemovalListener([drawable](const Entity& e) {
        drawable->renderable->destroy();
      });

      auto selectable = entity.add<Selectable>();
      selectable->feature = new SelectableFeature();
      selectable->feature->entityId = entity.getId();
      selectable->renderable = _renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature), UI);
      selectable.setRemovalListener([selectable](const Entity& e) {
        selectable->renderable->destroy();
      });

      {
        auto textEntity = createEntity();
        textEntity->add<Transform>();

        auto text = textEntity->add<Text>();
        text->feature = new TextFeature();
        text->renderable = _renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
        text.setRemovalListener([text](const Entity& e) {
          text->renderable->destroy();
        });

        buttonMetadata->textEntityId = textEntity->getId();
      }
    }

    auto transform = entity.get<Transform>();
    transform->position = button->position;

    auto width = glm::max(button->textSize * static_cast<float>(button->text.size()), button->minSize.x);
    auto height = glm::max(button->textSize, button->minSize.y);
    transform->scale = { width, height, 1.0f };

    auto drawable = entity.get<Drawable>();

    if (entity.has<Pressed>()) {
      auto pressed = entity.get<Pressed>();

      if (pressed->isFirstFrame) {
        _audioManager.play(EffectType::ButtonDown);
      }

      drawable->feature->color = button->activeColor;
    } else {
      drawable->feature->color = button->color;
    }

    auto buttonMetadata = entity.get<ButtonMetadata>();
    auto textEntity = getEntityById(buttonMetadata->textEntityId);

    auto textTransform = textEntity->get<Transform>();
    textTransform->position = button->position;
    textTransform->position.x -= button->textSize * static_cast<float>(button->text.size()) / 2.0f - button->textSize / 2.0f;
    textTransform->position.z -= 0.0001f;
    textTransform->scale = glm::vec3(button->textSize);

    auto text = textEntity->get<Text>();
    text->feature->text = button->text;
    text->feature->color = button->textColor;
    text->renderable->setEnabled(drawable->renderable->isEnabled());

    if (entity.has<Tapped>()) {
      _audioManager.play(EffectType::ButtonUp);
      button->clickHandler();
    }
  });
}

}  // namespace linguine
