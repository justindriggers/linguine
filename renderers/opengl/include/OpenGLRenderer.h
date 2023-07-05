#pragma once

#include "renderer/Renderer.h"
#include "renderer/features/FeatureRenderer.h"

namespace linguine::render {

class OpenGLRenderer : public Renderer {
  public:
    ~OpenGLRenderer() override = default;

    void draw() override;

    [[nodiscard]] std::optional<uint64_t> getEntityIdAt(float x, float y) const override {
      return {};
    }

  protected:
    [[nodiscard]] const std::vector<std::unique_ptr<FeatureRenderer>>& getFeatures() const override {
      return _features;
    }

  private:
    std::vector<std::unique_ptr<FeatureRenderer>> _features;
};

}  // namespace linguine::render
