#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "Camera.h"
#include "RenderBackend.h"
#include "Viewport.h"
#include "features/FeatureRenderer.h"
#include "features/RenderFeature.h"

namespace linguine {

class Renderer {
  public:
    explicit Renderer(std::unique_ptr<RenderBackend> backend)
        : _backend(std::move(backend)) {}

    virtual ~Renderer() = default;

    Renderable* create(std::unique_ptr<RenderFeature> feature, Layer layer = World);

    Camera* createCamera();

    [[nodiscard]] const Viewport& getViewport() const;

    void setBackend(std::unique_ptr<RenderBackend> backend) {
      _backend = std::move(backend);

      for (const auto& feature : _backend->getFeatures()) {
        for (const auto& renderable : _renderables) {
          feature->onFeatureChanged(*renderable.second);
        }
      }
    }

    inline void draw() {
      _backend->draw(_cameras);
    }

    void resize(uint16_t width, uint16_t height) {
      _viewport.setSize(width, height);
      _backend->resize(width, height);
    }

    void setInsets(uint16_t left, uint16_t right, uint16_t top, uint16_t bottom) {
      _viewport.setInsets(left, right, top, bottom);
    }

    inline void reset() {
      _backend->reset();
    }

    /**
     * @param x Where 0.0f is the left, and 1.0f is the right
     * @param y Where 0.0f is the bottom, and 1.0f is the top
     * @return Entity ID, if it exists at that location
     */
    [[nodiscard]] inline std::optional<uint64_t> getEntityIdAt(float x, float y) const {
      return _backend->getEntityIdAt(x, y);
    }

  protected:
    [[nodiscard]] const std::vector<std::unique_ptr<Camera>>& getCameras() const {
      return _cameras;
    }

  private:
    Viewport _viewport;

    std::unique_ptr<RenderBackend> _backend;

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
