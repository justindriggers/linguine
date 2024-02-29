#pragma once

#include <vector>

#include "features/FeatureRenderer.h"

namespace linguine {

class RenderBackend {
  public:
    virtual ~RenderBackend() = default;

    virtual void draw(const std::vector<std::unique_ptr<Camera>>& cameras) = 0;

    virtual void resize(uint16_t width, uint16_t height) = 0;

    [[nodiscard]] virtual const std::vector<std::unique_ptr<FeatureRenderer>>& getFeatures() const = 0;

    virtual void reset() = 0;

    [[nodiscard]] virtual std::optional<uint64_t> getEntityIdAt(float x, float y) const = 0;
};

}  // namespace linguine
