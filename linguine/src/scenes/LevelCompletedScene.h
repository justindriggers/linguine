#pragma once

#include "Scene.h"
#include "data/LevelDatabase.h"

namespace linguine {

class LevelCompletedScene : public Scene {
  public:
    explicit LevelCompletedScene(ServiceLocator& serviceLocator,
                                 uint8_t level, uint8_t stars)
        : Scene(serviceLocator), _level(level), _stars(stars) {}

    void init() override;

  private:
    struct UpgradeState {
      std::vector<std::string_view> upgrades{};
      int index = 0;
    };

    uint8_t _level;
    uint8_t _stars;
    LevelDatabase _levelDatabase;
    UpgradeDatabase _upgradeDatabase;
};

}  // namespace linguine
