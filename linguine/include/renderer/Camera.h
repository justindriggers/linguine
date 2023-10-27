#pragma once

#include <optional>

#include <glm/mat4x4.hpp>

#import "Layer.h"

namespace linguine {

class Renderer;

class Camera {
  public:
    Camera(uint64_t id, Renderer& renderer) : _id(id), _renderer(renderer) {}

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    glm::mat4 viewProjectionMatrix = glm::mat4(1.0f);
    std::optional<glm::vec3> clearColor = glm::vec3(0.0f);
    Layer layer = World;

    void destroy();

    [[nodiscard]] uint64_t getId() const {
      return _id;
    }

  private:
    uint64_t _id;
    Renderer& _renderer;
};

}  // namespace linguine
