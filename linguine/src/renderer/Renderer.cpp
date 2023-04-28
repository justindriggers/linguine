#include "renderer/Renderer.h"

#include <memory>

namespace linguine {

Renderable* Renderer::create(std::unique_ptr<RenderFeature> feature, Layer layer) {
  const auto id = _nextIndex++;
  _renderables.insert({id, std::make_unique<Renderable>(id, *this, std::move(feature), layer)});

  auto& renderable = _renderables[id];
  onFeatureChanged(*renderable);

  return renderable.get();
}

Camera* Renderer::createCamera() {
  const auto id = _cameras.size();
  _cameras.push_back(std::make_unique<Camera>(id));

  return _cameras[id].get();
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
