#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "renderer/Camera.h"
#include "renderer/features/FeatureRenderer.h"
#include "renderer/features/RenderFeature.h"
#include "renderer/Viewport.h"

namespace linguine {

class Renderer {
  public:
    virtual ~Renderer() = default;

    Renderable* create(std::unique_ptr<RenderFeature> feature, Layer layer = World);

    Camera* createCamera();

    [[nodiscard]] const Viewport& getViewport() const;

    virtual void draw() = 0;

    virtual void resize(uint16_t width, uint16_t height) {
      _viewport.setSize(width, height);

      for (auto& feature : getFeatures()) {
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
    [[nodiscard]] const std::vector<std::unique_ptr<Camera>>& getCameras() const {
      return _cameras;
    }

    [[nodiscard]] virtual const std::vector<std::unique_ptr<FeatureRenderer>>& getFeatures() const = 0;

  private:
    Viewport _viewport;

    uint64_t _nextCameraId = 0;
    uint64_t _nextRenderableId = 0;

    std::vector<std::unique_ptr<Camera>> _cameras;
    std::unordered_map<uint64_t, std::unique_ptr<Renderable>> _renderables;

    void onFeatureChanged(Renderable& renderable);

    void onDestroy(Renderable& renderable);

    void onDestroy(Camera& camera);

    friend class Camera;
    friend class Renderable;
};

}  // namespace linguine
