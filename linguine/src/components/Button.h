#pragma once

#include <functional>
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "data/Palette.h"

namespace linguine {

struct Button {
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec2 minSize = glm::vec2(1.0f);
  glm::vec3 color = Palette::Primary;
  glm::vec3 activeColor = Palette::PrimaryAccent;
  bool visible = true;
  bool enabled = true;

  std::string text;
  glm::vec3 textColor = glm::vec3(1.0f);
  float textSize = 1.0f;

  std::function<void()> clickHandler = [](){};
};

}  // namespace linguine
