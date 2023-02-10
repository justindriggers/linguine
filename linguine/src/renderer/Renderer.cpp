#include "renderer/Renderer.h"

#include <memory>

namespace linguine {

std::shared_ptr<Renderable> Renderer::create(const std::shared_ptr<RenderFeature>& feature) {
  auto renderable = std::make_shared<Renderable>(_nextIndex++, *this, feature);
  onFeatureChanged(*renderable);
  return renderable;
}

const std::shared_ptr<Camera>& Renderer::getCamera() const {
  return _camera;
}

const std::shared_ptr<Viewport>& Renderer::getViewport() const {
  return _viewport;
}

void Renderer::onFeatureChanged(Renderable& renderable) {
  for (const auto& feature : getFeatures()) {
    feature->onFeatureChanged(renderable);
  }
}

}  // namespace linguine
