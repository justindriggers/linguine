#include "renderer/Renderer.h"

namespace linguine {

Renderable* Renderer::create(std::unique_ptr<RenderFeature> feature, Layer layer) {
  const auto id = _nextRenderableId++;
  _renderables.insert({id, std::make_unique<Renderable>(id, *this, std::move(feature), layer)});

  auto& renderable = _renderables[id];
  onFeatureChanged(*renderable);

  return renderable.get();
}

Camera* Renderer::createCamera() {
  const auto id = _nextCameraId++;
  _cameras.push_back(std::make_unique<Camera>(id, *this));

  return std::prev(_cameras.end())->get();
}

const Viewport& Renderer::getViewport() const {
  return _viewport;
}

void Renderer::onFeatureChanged(Renderable& renderable) {
  for (const auto& feature : _backend->getFeatures()) {
    feature->onFeatureChanged(renderable);
  }
}

void Renderer::onDestroy(Renderable& renderable) {
  for (const auto& feature : _backend->getFeatures()) {
    feature->onDestroy(renderable);
  }

  _renderables.erase(renderable.getId());
}

void Renderer::onDestroy(Camera& camera) {
  _cameras.erase(std::remove_if(_cameras.begin(), _cameras.end(), [camera](const std::unique_ptr<Camera>& c) {
    return c->getId() == camera.getId();
  }), _cameras.end());
}

}  // namespace linguine
