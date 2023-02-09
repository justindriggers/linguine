#include "renderer/features/FeatureRenderer.h"

#include "renderer/Renderable.h"

namespace linguine {

void FeatureRenderer::onFeatureChanged(Renderable& renderable) {
  const auto index = _renderableIndices.find(renderable.getId());

  if (isRelevant(renderable)) {
    if (index == _renderableIndices.end()) {
      _renderableIndices.insert({renderable.getId(), _renderables.size()});
      _renderables.push_back(&renderable);
    }
  } else {
    if (index != _renderableIndices.end()) {
      _renderables.at(index->second) = nullptr;
      _renderableIndices.erase(renderable.getId());
    }
  }
}

}  // namespace linguine
