#pragma once

#include "input/Key.h"

namespace linguine {

struct AbilityButton {
  uint64_t abilityEntityId{};
  uint64_t textEntityId{};
  Key key{};
};

}  // namespace linguine
