#include "MacLeaderboardManager.h"

namespace linguine::alfredo {

void MacLeaderboardManager::getHighScores(std::function<void(std::vector<Record> results)> onSuccess,
                                          std::function<void(std::string reason)> onError) {
  auto highScore = 1'000'000;
  auto alphabet = std::string("abcdefghijklmnopqrstuvwxyz");
  auto randomLength = std::uniform_int_distribution(3, 15);

  auto results = std::vector<Record>();

  for (auto i = 0; i < 10; ++i) {
    auto name = alphabet.substr(0, randomLength(_random));

    auto randomScore = std::uniform_int_distribution(4, highScore - 1);
    highScore = randomScore(_random);

    results.push_back({ name, highScore });
  }

  results[4].isPlayer = true;

  onSuccess(results);
}

void MacLeaderboardManager::submitScore(int32_t score,
                                        std::function<void()> onSuccess,
                                        std::function<void(std::string)> onError) {
  onSuccess();
}

}  // namespace linguine::alfredo
