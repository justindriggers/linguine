#include "LevelTrackingSystem.h"

#include <glm/gtx/compatibility.hpp>

#include "components/CameraFixture.h"
#include "components/Progressable.h"
#include "components/Shake.h"
#include "components/Text.h"
#include "components/Toast.h"
#include "components/Transform.h"
#include "components/gameover/CurrentLevel.h"
#include "components/gameover/CurrentXp.h"
#include "components/gameover/LevelTracker.h"
#include "components/gameover/RequiredXp.h"
#include "data/upgrades/LevelCurve.h"

namespace linguine {

void LevelTrackingSystem::update(float deltaTime) {
  findEntities<Progressable, LevelTracker>()->each([this, deltaTime](const Entity& entity) {
    auto progressable = entity.get<Progressable>();
    auto levelTracker = entity.get<LevelTracker>();
    levelTracker->elapsed += deltaTime;

    auto lerp = glm::min(levelTracker->elapsed / levelTracker->duration, 1.0f);

    auto currentLevel = LevelCurve::getLevelForXp(levelTracker->startXp);

    if (currentLevel == LevelCurve::MaxLevel) {
      progressable->renderable->setEnabled(false);

      findEntities<CurrentXp, Text>()->each([](const Entity& currentXpEntity) {
        auto text = currentXpEntity.get<Text>();
        text->renderable->setEnabled(false);
      });

      findEntities<RequiredXp, Text, Transform>()->each([](const Entity& requiredXpEntity) {
        auto text = requiredXpEntity.get<Text>();
        text->renderable->setEnabled(false);
      });

      return;
    }

    auto lastLevelXp = LevelCurve::getXpForLevel(currentLevel);
    auto nextLevel = currentLevel + 1;
    auto levelUpXp = LevelCurve::getXpForLevel(nextLevel);
    auto endXp = glm::min(levelTracker->endXp, levelUpXp);

    auto currentXp = glm::lerp(static_cast<float>(levelTracker->startXp), static_cast<float>(endXp), lerp);
    auto currentXpInt = static_cast<int32_t>(glm::floor(currentXp));

    if (levelTracker->elapsed - levelTracker->lastAudible > 0.05f
        && currentXpInt > levelTracker->lastAudibleXp) {
      levelTracker->lastAudible = levelTracker->elapsed;
      levelTracker->lastAudibleXp = currentXpInt;
      _audioManager.play(EffectType::Xp);
    }

    auto current = currentXp - static_cast<float>(lastLevelXp);
    auto levelXp = levelUpXp - lastLevelXp;

    progressable->feature->progress = current / static_cast<float>(levelXp);

    findEntities<CurrentXp, Text>()->each([current](const Entity& currentXpEntity) {
      auto text = currentXpEntity.get<Text>();
      text->feature->text = std::to_string(static_cast<int32_t>(glm::floor(current)));
    });

    findEntities<RequiredXp, Text, Transform>()->each([levelXp](const Entity& requiredXpEntity) {
      auto text = requiredXpEntity.get<Text>();
      text->feature->text = std::to_string(levelXp);

      auto transform = requiredXpEntity.get<Transform>();
      transform->position.x = 88.0f - static_cast<float>(text->feature->text.size() - 1) * 8.0f;
    });

    if (lerp >= 1.0f && levelTracker->endXp >= levelUpXp) {
      _audioManager.play(EffectType::Level);

      currentLevel = nextLevel;
      levelTracker->startXp = levelUpXp;
      levelTracker->elapsed = 0.0f;
      levelTracker->lastAudible = 0.0f;

      findEntities<CurrentLevel, Text, Transform>()->each([currentLevel](const Entity& levelEntity) {
        auto text = levelEntity.get<Text>();
        text->feature->text = std::to_string(currentLevel);

        auto transform = levelEntity.get<Transform>();
        transform->position.x = -static_cast<float>(text->feature->text.size()) * 32.0f / 2.0f + 16.0f;
      });

      findEntities<Toast, Text>()->each([this, currentLevel](const Entity& toastEntity) {
        auto toast = toastEntity.get<Toast>();
        toast->elapsed = 0.0f;

        auto text = toastEntity.get<Text>();
        text->feature->text = _upgradeDatabase.getDescriptionByLevel(currentLevel);
      });

      findEntities<CameraFixture, Shake>()->each([](const Entity& entity) {
        auto shake = entity.get<Shake>();
        shake->magnitude += 8.0f;
        shake->duration += 0.5f;
      });
    }
  });
}

}  // namespace linguine
