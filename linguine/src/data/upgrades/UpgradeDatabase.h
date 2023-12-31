#pragma once

#include <map>

#include "Upgrade.h"

namespace linguine {

class UpgradeDatabase {
  public:
    UpgradeDatabase()
        : _upgrades {
              { 0, { "Shield Generators", "+1 Shield", { 1, 2, 4, 8, 12 } } },
              { 1, { "Generator Capacity", "+1 Durability", { 3, 7, 11, 15, 18 } } },
              { 2, { "Base Speed", "+1 Speed", { 5, 9, 13, 16, 19 } } },
              { 3, { "Ship Acceleration", "+1 Acceleration", { 6, 10, 14, 17, 20 } } }
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

    [[nodiscard]] const std::string& getDescriptionByLevel(uint8_t level) const {
      for (auto& upgrade : _upgrades) {
        for (auto rankLevel : upgrade.second.rankLevels) {
          if (rankLevel == level) {
            return upgrade.second.description;
          }
        }
      }

      throw std::runtime_error("No description found");
    }

  private:
    std::map<uint8_t, Upgrade> _upgrades;
};

}  // namespace linguine
