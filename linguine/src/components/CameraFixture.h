#pragma once

#include "renderer/Camera.h"

namespace linguine {

struct CameraFixture {
  float height = 15.0f;
  float speed = 1.0f;
  Camera* camera;
};

}  // namespace linguine
