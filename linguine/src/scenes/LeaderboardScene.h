#pragma once

#include "Scene.h"

namespace linguine {

class LeaderboardScene : public Scene {
  public:
    explicit LeaderboardScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator) {}

    void init() override;
};

}  // namespace linguine
