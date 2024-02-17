#include "TitleUiSystem.h"

#include "components/Button.h"
#include "components/LeaderboardButton.h"
#include "components/OptionsButton.h"

namespace linguine {

void TitleUiSystem::update(float deltaTime) {
  findEntities<LeaderboardButton, Button>()->each([this](const Entity& entity) {
    auto button = entity.get<Button>();
    button->visible = _leaderboardManager.isAvailable();
  });

  findEntities<OptionsButton, Button>()->each([this](const Entity& entity) {
    auto button = entity.get<Button>();

    if (_leaderboardManager.isAvailable()) {
      button->position.y = -112.0f;
    } else {
      button->position.y = -72.0f;
    }
  });
}

}  // namespace linguine
