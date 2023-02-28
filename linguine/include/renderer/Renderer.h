#pragma once

#include <memory>
#include <vector>

#include "renderer/Camera.h"
#include "renderer/features/FeatureRenderer.h"
#include "renderer/features/RenderFeature.h"
#include "renderer/Viewport.h"

namespace linguine {

class Renderer {
  public:
    virtual ~Renderer() = default;

    std::shared_ptr<Renderable> create(const std::shared_ptr<RenderFeature>& feature);

    [[nodiscard]] const std::shared_ptr<Camera>& getCamera() const;

    [[nodiscard]] const std::shared_ptr<Viewport>& getViewport() const;

    virtual void draw() = 0;

    void resize(uint16_t width, uint16_t height) {
      getViewport()->setSize(width, height);

      for (const auto& feature : getFeatures()) {
        feature->resize(width, height);
      }
    }

    /**
     * @param x Where 0.0f is the left, and 1.0f is the right
     * @param y Where 0.0f is the bottom, and 1.0f is the top
     * @return Entity ID, if it exists at that location
     */
    [[nodiscard]] virtual std::optional<uint64_t> getEntityIdAt(float x, float y) const = 0;

  protected:
    [[nodiscard]] virtual const std::vector<std::unique_ptr<FeatureRenderer>>& getFeatures() const = 0;

  private:
    std::shared_ptr<Camera> _camera = std::make_shared<Camera>();
    std::shared_ptr<Viewport> _viewport = std::make_shared<Viewport>();

    uint64_t _nextIndex = 0;

    void onFeatureChanged(Renderable& renderable);

    friend class Renderable;
};

}  // namespace linguine
