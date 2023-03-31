#pragma once

#include "data/UnitType.h"

namespace linguine {

struct UnitSelector {
  UnitType type = Default;
  bool isSelected = false;
};

}  // namespace linguine
