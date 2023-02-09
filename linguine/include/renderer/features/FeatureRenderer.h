#pragma once

#include <unordered_map>
#include <vector>

#include "renderer/Renderable.h"

namespace linguine {

class FeatureRenderer {
  public:
    virtual ~FeatureRenderer() = default;

    void onFeatureChanged(Renderable& renderable);

    virtual bool isRelevant(Renderable& renderable) = 0;

    virtual void draw() = 0;

  protected:
    [[nodiscard]] const std::vector<Renderable*>& getRenderables() const {
      return _renderables;
    }

  private:
    std::vector<Renderable*> _renderables;
    std::unordered_map<uint64_t, size_t> _renderableIndices;
};

}  // namespace linguine
