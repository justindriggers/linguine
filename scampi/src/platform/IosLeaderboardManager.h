#pragma once

#include <LeaderboardManager.h>

namespace linguine::scampi {

class IosLeaderboardManager : public LeaderboardManager {
  public:
    ~IosLeaderboardManager() override = default;

    bool isAvailable() override;

    void getHighScores(std::function<void(std::vector<Record> results)> onSuccess,
                       std::function<void(std::string reason)> onError) override;

    void submitScore(int32_t score,
                     std::function<void()> onSuccess,
                     std::function<void(std::string reason)> onError) override;
};

}  // namespace linguine::scampi
