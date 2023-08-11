#include "TooltipSystem.h"

#include "components/Drawable.h"
#include "components/Text.h"
#include "components/Tooltip.h"
#include "components/Transform.h"

namespace linguine {

void TooltipSystem::update(float deltaTime) {
  findEntities<Tooltip>()->each([this](const Entity& entity) {
    auto tooltip = entity.get<Tooltip>();

    if (!tooltip->textEntityId) {
      auto textEntity = createEntity();

      auto transform = textEntity->add<Transform>();
      transform->scale = glm::vec3(10.0f, 10.0f, 0.0f);

      auto text = textEntity->add<Text>();
      text->feature = new TextFeature();
      text->renderable = _renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
      text.setRemovalListener([text](const Entity e) {
        text->renderable->destroy();
      });

      tooltip->textEntityId = textEntity->getId();
    }

    if (!tooltip->backgroundEntityId) {
      auto backgroundEntity = createEntity();
      backgroundEntity->add<Transform>();

      auto drawable = backgroundEntity->add<Drawable>();
      drawable->feature = new ColoredFeature();
      drawable->feature->meshType = Quad;
      drawable->feature->color = { 0.05f, 0.05f, 0.05f };
      drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature), UI);
      drawable.setRemovalListener([drawable](const Entity e) {
        drawable->renderable->destroy();
      });

      tooltip->backgroundEntityId = backgroundEntity->getId();
    }

    auto textEntity = getEntityById(*tooltip->textEntityId);

    auto text = textEntity->get<Text>();
    text->renderable->setEnabled(tooltip->isEnabled);

    auto backgroundEntity = getEntityById(*tooltip->backgroundEntityId);

    auto drawable = backgroundEntity->get<Drawable>();
    drawable->renderable->setEnabled(tooltip->isEnabled);

    if (tooltip->isEnabled) {
      auto textTransform = textEntity->get<Transform>();
      auto backgroundTransform = backgroundEntity->get<Transform>();

      if (tooltip->isDirty) {
        const auto& s = tooltip->getText();

        auto lineLength = 0;
        auto lineCount = 1;
        auto longestLine = 0;

        for (auto& character : s) {
          if (character == '\n') {
            if (lineLength > longestLine) {
              longestLine = lineLength;
            }

            lineLength = 0;
            ++lineCount;
          } else {
            ++lineLength;
          }
        }

        if (lineLength > longestLine) {
          longestLine = lineLength;
        }

        backgroundTransform->scale = {
            10.0f * static_cast<float>(longestLine) + 10.0f,
            10.0f * static_cast<float>(lineCount) + 10.0f,
            0.0f
        };

        text->feature->text = s;
        tooltip->isDirty = false;
      }

      auto width = static_cast<float>(_renderer.getViewport().getWidth());
      auto height = static_cast<float>(_renderer.getViewport().getHeight());
      auto x = _inputManager.getCursorLocation().x * width - width / 2.0f;
      auto y = _inputManager.getCursorLocation().y * height - height / 2.0f;

      textTransform->position = { x + 15.0f, y + backgroundTransform->scale.y, 0.0f };
      backgroundTransform->position = { x + backgroundTransform->scale.x / 2.0f + 5.0f, y + backgroundTransform->scale.y / 2.0f + 10.0f, 0.0001f };

      tooltip->isEnabled = false;
    }
  });
}

}  // namespace linguine
