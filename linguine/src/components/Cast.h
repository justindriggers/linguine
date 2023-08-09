#pragma once

#include "data/spells/Spell.h"

namespace linguine {

struct Cast {
  std::optional<uint64_t> abilityEntityId{};
  std::optional<uint64_t> targetEntityId{};
  float elapsed = 0.0f;
};

}  // namespace linguine
