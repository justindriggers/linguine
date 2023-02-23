#pragma once

#include <memory>

#include "renderer/features/QuadFeature.h"

namespace linguine {

struct Quad {
  std::shared_ptr<QuadFeature> feature;
};

}  // namespace linguine
