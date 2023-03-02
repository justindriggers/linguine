#pragma once

#include "renderer/features/ColoredFeature.h"
#include "renderer/Renderable.h"

namespace linguine {

struct Drawable {
  ColoredFeature* feature;
  Renderable* renderable;
};

}  // namespace linguine
