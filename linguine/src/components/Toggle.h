#pragma once

#include <functional>
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "data/Palette.h"

namespace linguine {

struct Toggle {
  bool isEnabled = false;

  glm::vec3 position = glm::vec3(0.0f);
  glm::vec2 minSize = glm::vec2(1.0f);
  float textSize = 1.0f;

  glm::vec3 onColor = Palette::Primary;
  glm::vec3 onActiveColor = Palette::PrimaryAccent;
  std::string onText = "On";
  glm::vec3 onTextColor = glm::vec3(1.0f);

  glm::vec3 offColor = Palette::Secondary;
  glm::vec3 offActiveColor = Palette::SecondaryAccent;
  std::string offText = "Off";
  glm::vec3 offTextColor = glm::vec3(1.0f);

  std::function<void(bool)> toggleHandler = [](bool){};
};

}  // namespace linguine