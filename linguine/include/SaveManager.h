#pragma once

#include <cstdint>
#include <unordered_map>

namespace linguine {

class SaveManager {
  public:
    void addPoints(int32_t points) {
      _points += points;
    }

    void removePoints(int32_t points) {
      _points -= points;
    }

    uint8_t getRank(uint8_t upgradeId) const {
      if (_upgradeRanks.find(upgradeId) != _upgradeRanks.end()) {
        return _upgradeRanks.at(upgradeId);
      } else {
        return 0;
      }
    }

    void increaseRank(uint8_t upgradeId) {
      if (_upgradeRanks.find(upgradeId) != _upgradeRanks.end()) {
        _upgradeRanks[upgradeId]++;
      } else {
        _upgradeRanks[upgradeId] = 1;
      }
    }

    int32_t getPoints() const {
      return _points;
    }

  private:
    int32_t _points{};
    std::unordered_map<uint8_t, uint8_t> _upgradeRanks;
};

}  // namespace linguine
