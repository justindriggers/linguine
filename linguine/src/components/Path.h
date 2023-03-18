#pragma once

#include "data/Spline.h"

namespace linguine {

struct Path {
  float speed = 1.0f;
  float scale = 1.0f;
  Spline spline;
  int index = 0;
  float t = 0.0f;
};

}  // namespace linguine
