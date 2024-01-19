#pragma once

#include <glm/vec2.hpp>

namespace linguine {

struct Attachment {
  uint64_t parentId{};
  glm::vec2 offset = { 0.0f, 0.0f };
  bool useFixedUpdate = true;
};

}  // namespace lingiune
