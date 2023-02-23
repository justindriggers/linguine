#pragma once

#include "renderer/Renderable.h"

namespace linguine {

struct Drawable {
  std::shared_ptr<Renderable> renderable;
};

}  // namespace linguine
