#pragma once

#include "renderer/Renderable.h"
#include "renderer/features/SelectableFeature.h"

namespace linguine {

struct Selectable {
  std::shared_ptr<Renderable> renderable;
  std::shared_ptr<SelectableFeature> feature;
};

}  // namespace linguine
