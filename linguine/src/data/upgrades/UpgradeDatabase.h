#pragma once

#include <map>

#include "Upgrade.h"

namespace linguine {

class UpgradeDatabase {
  public:
    UpgradeDatabase()
        : _upgrades {
              { Upgrade::Type::Shield, { "Shield Generators", "+1 Shield", { 1, 2, 4, 8, 12 } } },
              { Upgrade::Type::Durability, { "Generator Capacity", "+1 Durability", { 3, 7, 11, 15, 18 } } },
              { Upgrade::Type::Speed, { "Max Speed", "+1 Max Speed", { 5, 9, 13, 16, 19 } } },
              { Upgrade::Type::Acceleration, { "Ship Acceleration", "+1 Acceleration", { 6, 10, 14, 17, 20 } } }
          } {}

    [[nodiscard]] const Upgrade& getByType(Upgrade::Type type) const {
      return _upgrades.at(type);
    }

    [[nodiscard]] uint8_t getRankByLevel(Upgrade::Type type, uint8_t level) const {
      auto& upgrade = getByType(type);

      auto rank = 0;

      for (auto rankLevel : upgrade.rankLevels) {
        if (rankLevel > level) {
          break;
        }

        ++rank;
      }

      return rank;
    }

    [[nodiscard]] std::vector<Upgrade::Type> getUpgradesByLevel(uint8_t level) const {
      auto results = std::vector<Upgrade::Type>();

      for (auto& upgrade : _upgrades) {
        for (auto rankLevel : upgrade.second.rankLevels) {
          if (rankLevel == level) {
            results.push_back(upgrade.first);
            break;
          } else if (rankLevel > level) {
            break;
          }
        }
      }

      return results;
    }

    [[nodiscard]] const std::string& getDescriptionByLevel(uint8_t level) const {
      for (auto& upgrade : _upgrades) {
        for (auto rankLevel : upgrade.second.rankLevels) {
          if (rankLevel == level) {
            return upgrade.second.description;
          } else if (rankLevel > level) {
            break;
          }
        }
      }

      throw std::runtime_error("No description found");
    }

  private:
    std::map<Upgrade::Type, Upgrade> _upgrades;
};

}  // namespace linguine
