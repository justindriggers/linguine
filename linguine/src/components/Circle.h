#pragma once

#include "renderer/features/CircleFeature.h"
#include "renderer/Renderable.h"

namespace linguine {

struct Circle {
  CircleFeature* feature;
  Renderable* renderable;
};

}  // namespace linguine
