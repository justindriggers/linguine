#pragma once

#include <memory>

#include "renderer/Camera.h"

namespace linguine {

struct HasCamera {
  std::shared_ptr<Camera> camera;
};

}  // namespace linguine
