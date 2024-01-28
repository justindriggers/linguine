#pragma once

#include "renderer/Camera.h"

namespace linguine {

struct CameraFixture {
  enum class Measurement {
    Height,
    Width
  };

  float size = 15.0f;
  Measurement type = Measurement::Height;
  float speed = 1.0f;
  bool shake = false;
  Camera* camera;
};

}  // namespace linguine
