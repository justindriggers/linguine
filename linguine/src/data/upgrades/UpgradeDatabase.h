#pragma once

#include <map>

#include "Upgrade.h"

namespace linguine {

class UpgradeDatabase {
  public:
    UpgradeDatabase()
        : _upgrades {
              { 0, { "Shield Generators", "Add additional defenses to your ship", { 25, 50, 100, 250, 500 } } },
              { 1, { "Generator Capacity", "Increase the strength of each shield", { 50, 100, 250, 500, 1000 } } },
              { 2, { "Base Speed", "Start each run with more speed", { 75, 150, 300, 750, 1500 } } },
              { 3, { "Ship Acceleration", "Pick up more speed over time", { 100, 250, 500, 1000, 2500 } } }
          } {}

    const std::map<uint8_t, Upgrade>& getUpgrades() const {
      return _upgrades;
    }

    const Upgrade& getById(uint8_t id) const {
      return _upgrades.at(id);
    }

  private:
    std::map<uint8_t, Upgrade> _upgrades;
};

}  // namespace linguine
