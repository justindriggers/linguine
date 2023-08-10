#pragma once

#include <functional>

#include <glm/vec3.hpp>

#include "actions/Action.h"

namespace linguine {

struct Spell {
  Spell(float castTime, float cooldown, glm::vec3 color, std::unique_ptr<Action> action)
      : castTime(castTime), cooldown(cooldown), color(color), action(std::move(action)) {}

  float castTime;
  float cooldown;
  glm::vec3 color;
  std::unique_ptr<Action> action;
};

}  // namespace linguine
