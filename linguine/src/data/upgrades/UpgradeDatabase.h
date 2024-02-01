#pragma once

#include <map>

#include "Upgrade.h"

namespace linguine {

class UpgradeDatabase {
  public:
    UpgradeDatabase()
        : _upgrades {
              { 0, { "Shield Generators", "+1 Shield", { 1, 2, 3, 4, 5 } } },
              { 1, { "Generator Capacity", "+1 Durability", { 3, 7, 10, 13, 16 } } },
              { 2, { "Base Speed", "+1 Speed", { 5, 8, 11, 14, 17 } } },
              { 3, { "Ship Acceleration", "+1 Acceleration", { 6, 9, 12, 15, 18 } } }
          } {}

    [[nodiscard]] const std::map<uint8_t, Upgrade>& getUpgrades() const {
      return _upgrades;
    }

    [[nodiscard]] const Upgrade& getById(uint8_t id) const {
      return _upgrades.at(id);
    }

    [[nodiscard]] uint8_t getRankByLevel(uint8_t id, uint8_t level) const {
      auto& upgrade = getById(id);

      auto rank = 0;

      for (auto rankLevel : upgrade.rankLevels) {
        if (rankLevel > level) {
          break;
        }

        ++rank;
      }

      return rank;
    }

    [[nodiscard]] std::vector<std::string_view> getDescriptionsByLevel(uint8_t level) const {
      auto results = std::vector<std::string_view>();

      for (auto& upgrade : _upgrades) {
        for (auto rankLevel : upgrade.second.rankLevels) {
          if (rankLevel == level) {
            results.push_back(upgrade.second.description);
            break;
          } else if (rankLevel > level) {
            break;
          }
        }
      }

      return results;
    }

  private:
    std::map<uint8_t, Upgrade> _upgrades;
};

}  // namespace linguine
