#pragma once

#include <renderer/Renderer.h>

namespace linguine::pesto {

class WebRenderer : public Renderer {
  public:
    ~WebRenderer() override = default;

    void draw() override {}

    [[nodiscard]] std::optional<uint64_t> getEntityIdAt(float x, float y) const override {
      return std::optional<uint64_t>();
    }

  protected:
    [[nodiscard]] const std::vector<std::unique_ptr<FeatureRenderer>>& getFeatures() const override {
      return _features;
    }

  private:
    std::vector<std::unique_ptr<FeatureRenderer>> _features = {};
};

}  // namespace linguine::pesto
