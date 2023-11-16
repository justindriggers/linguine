#pragma once

namespace linguine {

struct RankIndicator {
  RankIndicator(uint8_t upgradeId, uint8_t rank)
      : upgradeId(upgradeId), rank(rank) {}

  uint8_t upgradeId;
  uint8_t rank;
};

}  // namespace linguine
