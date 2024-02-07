#include "UpgradeSystem.h"

#include <glm/common.hpp>

#include "components/Alive.h"
#include "components/Health.h"
#include "components/LevelUp.h"
#include "components/Party.h"
#include "components/Player.h"
#include "components/Score.h"
#include "components/Text.h"
#include "components/Toast.h"
#include "components/UpgradeIndicator.h"
#include "data/upgrades/LevelCurve.h"

namespace linguine {

void UpgradeSystem::update(float deltaTime) {
  findEntities<Score>()->each([this](Entity& entity) {
    auto score = entity.get<Score>();
    auto currentLevel = LevelCurve::getLevelForXp(score->points);

    if (!entity.has<UpgradeMetadata>()) {
      entity.add<UpgradeMetadata>();
    }

    auto metadata = entity.get<UpgradeMetadata>();

    while (metadata->knownLevel < currentLevel) {
      auto level = ++metadata->knownLevel;

      auto startLevel = LevelCurve::getLevelForXp(_saveManager.getPoints());

      if (level > startLevel) {
        _audioManager.play(EffectType::Level);

        findEntities<UpgradeIndicator, Toast, Text>()->each([this, level](const Entity& upgradeEntity) {
          auto toast = upgradeEntity.get<Toast>();
          toast->elapsed = 0.0f;

          auto text = upgradeEntity.get<Text>();
          text->feature->text = _upgradeDatabase.getDescriptionByLevel(level);
        });
      }

      findEntities<LevelUp, Toast, Text>()->each([level](const Entity& levelUpEntity) {
        auto toast = levelUpEntity.get<Toast>();
        toast->elapsed = 0.0f;

        auto text = levelUpEntity.get<Text>();
        text->feature->text = "Level " + std::to_string(level);
      });

      auto upgrades = _upgradeDatabase.getUpgradesByLevel(level);

      for (auto upgradeType : upgrades) {
        switch (upgradeType) {
        case Upgrade::Shield:
          findEntities<Party>()->each([this, level](const Entity& partyEntity) {
            auto shieldEntity = createEntity();

            shieldEntity->add<Health>(1000 + 250 * _upgradeDatabase.getRankByLevel(Upgrade::Type::Durability, level));
            shieldEntity->add<Alive>();

            auto party = partyEntity.get<Party>();
            party->memberIds.push_back(shieldEntity->getId());
          });
          break;
        case Upgrade::Type::Durability:
          findEntities<Health>()->each([this, level](const Entity& healthEntity) {
            auto health = healthEntity.get<Health>();
            health->max = 1000 + 250 * _upgradeDatabase.getRankByLevel(Upgrade::Type::Durability, level);

            if (healthEntity.has<Alive>()) {
              health->current += 250;
            }
          });
          break;
        case Upgrade::Type::Speed:
          findEntities<Player>()->each([this, level](const Entity& playerEntity) {
            auto player = playerEntity.get<Player>();
            player->speed = glm::max(player->speed, 5.0f + 1.0f * static_cast<float>(_upgradeDatabase.getRankByLevel(Upgrade::Type::Speed, level)));
          });
          break;
        case Upgrade::Type::Acceleration:
          findEntities<Player>()->each([this, level](const Entity& playerEntity) {
            auto player = playerEntity.get<Player>();
            player->acceleration = 0.05f + 0.02f * static_cast<float>(_upgradeDatabase.getRankByLevel(Upgrade::Type::Acceleration, level));
          });
          break;
        }
      }
    }
  });
}

}  // namespace linguine
