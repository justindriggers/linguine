#include "TutorialSystem.h"

#include <glm/gtx/compatibility.hpp>

#include "components/Drawable.h"
#include "components/GlobalCooldown.h"
#include "components/Score.h"
#include "components/Transform.h"
#include "components/TutorialState.h"
#include "data/Palette.h"

namespace linguine {

void TutorialSystem::update(float deltaTime) {
  findEntities<TutorialState>()->each([this](Entity& tutorialStateEntity) {
    auto tutorialState = tutorialStateEntity.get<TutorialState>();

    switch (tutorialState->currentState) {
    case TutorialState::State::WaitingForScore:
      findEntities<Score>()->each([&tutorialState](const Entity& scoreEntity) {
        auto score = scoreEntity.get<Score>();

        if (score->points > 0) {
          tutorialState->hasScored = true;
        }
      });
      break;
    case TutorialState::State::WaitingForHeal: {
      findEntities<GlobalCooldown>()->each([&tutorialState](const Entity& globalCooldownEntity) {
        auto globalCooldown = globalCooldownEntity.get<GlobalCooldown>();

        if (globalCooldown->remaining > 0.0f) {
          tutorialState->hasHealed = true;
        }
      });

      if (!tutorialStateEntity.has<Drawable>()) {
        auto transform = tutorialStateEntity.add<Transform>();
        transform->position = { 0.0f, -23.0f, 10.0f };
        transform->scale = glm::vec3(64.0f);

        switch (_saveManager.getHandedness()) {
        case SaveManager::Handedness::Right:
          transform->position.x = 92.0f;
          break;
        case SaveManager::Handedness::Left:
          transform->position.x = -92.0f;
          break;
        }

        auto drawable = tutorialStateEntity.add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->color = Palette::Primary;
        drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature), UI);
        drawable.setRemovalListener([drawable](const Entity& entity) {
          drawable->renderable->destroy();
        });
      } else {
        auto drawable = tutorialStateEntity.get<Drawable>();
        drawable->feature->color.a = glm::cos(tutorialState->elapsed * 4.0f) / 2.0f + 0.5f;

        auto transform = tutorialStateEntity.get<Transform>();
        transform->scale = glm::vec3(glm::lerp(96.0f, 44.0f, glm::clamp(tutorialState->elapsed / 0.2f, 0.0f, 1.0f)));
      }
      break;
    }
    case TutorialState::State::Finished:
      _saveManager.completeNewPlayerExperience();
      tutorialState->hasFinished = true;
      break;
    }
  });
}

}  // namespace linguine
