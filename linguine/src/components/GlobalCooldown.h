#pragma once

namespace linguine {

struct GlobalCooldown {
  float remaining = 0.0f;
  float total = 1.5f;
};

}  // namespace linguine
