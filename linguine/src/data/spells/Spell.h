#pragma once

#include <functional>

#include "actions/Action.h"

namespace linguine {

struct Spell {
  Spell(float castTime, float cooldown, std::unique_ptr<Action> action)
      : castTime(castTime), cooldown(cooldown), action(std::move(action)) {}

  float castTime;
  float cooldown;
  std::unique_ptr<Action> action;
};

}  // namespace linguine
