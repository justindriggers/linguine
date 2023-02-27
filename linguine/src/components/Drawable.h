#pragma once

#include "renderer/features/ColoredFeature.h"
#include "renderer/Renderable.h"

namespace linguine {

struct Drawable {
  std::shared_ptr<ColoredFeature> feature;
  std::shared_ptr<Renderable> renderable;
};

}  // namespace linguine
