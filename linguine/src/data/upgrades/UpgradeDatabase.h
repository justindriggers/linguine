#pragma once

#include <vector>

#include "Upgrade.h"

namespace linguine {

class UpgradeDatabase {
  public:
    UpgradeDatabase()
        : _upgrades {
              { "Shield Generators", "Add additional defenses to your ship", 1000, 5 },
              { "Generator Capacity", "Increase the strength of each shield", 100, 10 },
              { "Base Speed", "Start each run with more speed", 100, 10 },
              { "Ship Acceleration", "Pick up more speed over time", 100, 5 }
          } {}

    const std::vector<Upgrade>& getUpgrades() const {
      return _upgrades;
    }

  private:
    std::vector<Upgrade> _upgrades;
};

}  // namespace linguine
