#include "renderer/features/FeatureRenderer.h"

#include "renderer/Renderable.h"

namespace linguine {

void FeatureRenderer::onFeatureChanged(Renderable& renderable) {
  if (isRelevant(renderable)) {
    _renderables.insert({renderable.getId(), &renderable});
  } else {
    _renderables.erase(renderable.getId());
  }
}

void FeatureRenderer::onDestroy(Renderable& renderable) {
  _renderables.erase(renderable.getId());
}

}  // namespace linguine
