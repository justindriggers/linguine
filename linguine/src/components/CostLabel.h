#pragma once

namespace linguine {

struct CostLabel {
  CostLabel(uint8_t upgradeId) : upgradeId(upgradeId) {}

  uint8_t upgradeId;
};

}  // namespace linguine
