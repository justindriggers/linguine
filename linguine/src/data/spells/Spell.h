#pragma once

#include <functional>

#include <string>

#include <glm/vec3.hpp>

#include "actions/Action.h"

namespace linguine {

struct Spell {
  Spell(std::string name, float castTime, float cooldown, glm::vec3 color, std::string tooltip, std::unique_ptr<Action> action)
      : name(name), castTime(castTime), cooldown(cooldown), color(color), tooltip(std::move(tooltip)), action(std::move(action)) {}

  std::string name;
  float castTime;
  float cooldown;
  glm::vec3 color;
  std::string tooltip;
  std::unique_ptr<Action> action;
};

}  // namespace linguine
