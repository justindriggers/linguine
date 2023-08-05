#pragma once

#include "renderer/features/TextFeature.h"
#include "renderer/Renderable.h"

namespace linguine {

struct Text {
  TextFeature* feature;
  Renderable* renderable;
};

}  // namespace linguine
