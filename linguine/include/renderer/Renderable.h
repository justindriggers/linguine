#pragma once

#include <memory>

#include "renderer/features/RenderFeature.h"

namespace linguine {

class Renderer;

class Renderable {
  public:
    Renderable(const uint64_t id, Renderer& renderer, std::unique_ptr<RenderFeature> feature)
        : _id(id), _renderer(renderer), _feature(std::move(feature)), _isEnabled(true) {}

    void destroy();

    [[nodiscard]] uint64_t getId() const {
      return _id;
    }

    [[nodiscard]] bool isEnabled() const {
      return _isEnabled;
    }

    void setEnabled(bool enabled) {
      _isEnabled = enabled;
    }

    template<typename T>
    [[nodiscard]] bool hasFeature() const {
      return dynamic_cast<T*>(_feature.get()) != nullptr;
    }

    template<typename T>
    [[nodiscard]] T& getFeature() const {
      return dynamic_cast<T&>(*_feature);
    }

    void setFeature(std::unique_ptr<RenderFeature> feature);

  private:
    const uint64_t _id;
    Renderer& _renderer;
    std::unique_ptr<RenderFeature> _feature;
    bool _isEnabled;
};

}  // namespace linguine
