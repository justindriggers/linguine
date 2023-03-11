#pragma once

#include "renderer/features/ProgressFeature.h"
#include "renderer/Renderable.h"

namespace linguine {

struct Progressable {
  ProgressFeature* feature;
  Renderable* renderable;
};

}  // namespace linguine
