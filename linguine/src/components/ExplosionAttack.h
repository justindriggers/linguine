#pragma once

namespace linguine {

struct ExplosionAttack {
  int count = 36;
  int32_t power = 25;
  float speed = 1.0f;
  float frequency = 10.0f;
  float elapsed = 0.0f;
};

}  // namespace linguine
