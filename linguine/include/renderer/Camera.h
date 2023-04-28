#pragma once

#include <glm/mat4x4.hpp>

#import "Layer.h"

namespace linguine {

class Camera {
  public:
    explicit Camera(uint64_t id) : _id(id) {}

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    glm::mat4 viewProjectionMatrix = glm::mat4(1.0f);
    std::optional<glm::vec3> clearColor = glm::vec3(0.0f);
    Layer layer = World;

    [[nodiscard]] uint64_t getId() const {
      return _id;
    }

  private:
    uint64_t _id;
};

}  // namespace linguine
