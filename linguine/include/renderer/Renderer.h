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
