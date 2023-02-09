#pragma once

#include <memory>

#include "renderer/features/RenderFeature.h"

namespace linguine {

class Renderer;

class Renderable {
  public:
    Renderable(const uint64_t id, Renderer& renderer, std::shared_ptr<RenderFeature> feature)
        : _id(id), _renderer(renderer), _feature(std::move(feature)), _isEnabled(true) {}

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
      return std::dynamic_pointer_cast<T>(_feature) != nullptr;
    }

    template<typename T>
    [[nodiscard]] std::shared_ptr<T> getFeature() const {
      return std::dynamic_pointer_cast<T>(_feature);
    }

    void setFeature(std::shared_ptr<RenderFeature> feature);

  private:
    const uint64_t _id;
    Renderer& _renderer;
    std::shared_ptr<RenderFeature> _feature;
    bool _isEnabled;
};

}  // namespace linguine
