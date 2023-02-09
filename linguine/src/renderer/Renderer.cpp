#include "renderer/Renderer.h"

#include <memory>

namespace linguine {

std::shared_ptr<Renderable> Renderer::create(const std::shared_ptr<RenderFeature>& feature) {
  auto renderable = std::make_shared<Renderable>(0, *this, feature);
  onFeatureChanged(*renderable);
  return renderable;
}

void Renderer::onFeatureChanged(Renderable& renderable) {
  for (const auto& feature : getFeatures()) {
    feature->onFeatureChanged(renderable);
  }
}

}  // namespace linguine
