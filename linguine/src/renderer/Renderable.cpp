#include "renderer/Renderable.h"

#include "renderer/Renderer.h"

namespace linguine {

void Renderable::destroy() {
  _renderer.onDestroy(*this);
}

void Renderable::setFeature(std::unique_ptr<RenderFeature> feature) {
  _feature = std::move(feature);
  _renderer.onFeatureChanged(*this);
}

}  // namespace linguine
