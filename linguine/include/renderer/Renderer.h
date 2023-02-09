#pragma once

#include <memory>
#include <vector>

#include "renderer/features/FeatureRenderer.h"
#include "renderer/features/RenderFeature.h"

namespace linguine {

class Renderer {
  public:
    virtual ~Renderer() = default;

    std::shared_ptr<Renderable> create(const std::shared_ptr<RenderFeature>& feature);

    virtual void draw() = 0;

  protected:
    [[nodiscard]] virtual const std::vector<std::unique_ptr<FeatureRenderer>>& getFeatures() const = 0;

  private:
    void onFeatureChanged(Renderable& renderable);

    friend class Renderable;
};

}  // namespace linguine
