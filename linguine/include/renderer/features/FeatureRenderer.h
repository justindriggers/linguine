#pragma once

#include <unordered_map>

#include "renderer/Camera.h"
#include "renderer/Renderable.h"

namespace linguine {

class FeatureRenderer {
  public:
    virtual ~FeatureRenderer() = default;

    void onFeatureChanged(Renderable& renderable);

    void onDestroy(Renderable& renderable);

    virtual bool isRelevant(Renderable& renderable) = 0;

    virtual void onFrameBegin() = 0;

    virtual void draw(Camera& camera) = 0;

    virtual void resize(uint16_t width, uint16_t height) = 0;

  protected:
    [[nodiscard]] const std::unordered_map<uint64_t, Renderable*>& getRenderables() const {
      return _renderables;
    }

  private:
    std::unordered_map<uint64_t, Renderable*> _renderables;
};

}  // namespace linguine
