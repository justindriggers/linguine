#pragma once

#include "System.h"

#include "LeaderboardManager.h"

namespace linguine {

class TitleUiSystem : public System {
  public:
    TitleUiSystem(EntityManager& entityManager,
                  LeaderboardManager& leaderboardManager)
        : System(entityManager), _leaderboardManager(leaderboardManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    LeaderboardManager & _leaderboardManager;
};

}  // namespace linguine
