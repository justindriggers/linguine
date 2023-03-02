#include "renderer/Renderer.h"

#include <memory>

namespace linguine {

Renderable* Renderer::create(std::unique_ptr<RenderFeature> feature) {
  const auto id = _nextIndex++;
  _renderables.insert({id, std::make_unique<Renderable>(id, *this, std::move(feature))});

  auto& renderable = _renderables[id];
  onFeatureChanged(*renderable);

  return renderable.get();
}

Camera& Renderer::getCamera() {
  return _camera;
}

const Viewport& Renderer::getViewport() const {
  return _viewport;
}

void Renderer::onFeatureChanged(Renderable& renderable) {
  for (const auto& feature : getFeatures()) {
    feature->onFeatureChanged(renderable);
  }
}

void Renderer::onDestroy(Renderable& renderable) {
  for (const auto& feature : getFeatures()) {
    feature->onDestroy(renderable);
  }

  _renderables.erase(renderable.getId());
}

}  // namespace linguine
