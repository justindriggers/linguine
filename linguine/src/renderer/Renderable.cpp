#include "renderer/Renderable.h"

#include "renderer/Renderer.h"

namespace linguine {

void Renderable::setFeature(std::shared_ptr<RenderFeature> feature) {
  _feature = std::move(feature);
  _renderer.onFeatureChanged(*this);
}

}  // namespace linguine
