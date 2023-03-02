#pragma once

#include "renderer/Renderable.h"
#include "renderer/features/SelectableFeature.h"

namespace linguine {

struct Selectable {
  Renderable* renderable;
  SelectableFeature* feature;
};

}  // namespace linguine
