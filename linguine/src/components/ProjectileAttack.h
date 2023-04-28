#pragma once

namespace linguine {

struct ProjectileAttack {
  float speed = 1.0f;
  float range = 10.0f;
  int32_t power = 100;
  float frequency = 1.0f;
  float elapsed = 0.0f;
};

}  // namespace linguine
