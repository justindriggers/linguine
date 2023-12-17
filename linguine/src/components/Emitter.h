#pragma once

#include <functional>

namespace linguine {

struct Emitter {
  explicit Emitter(std::function<void()> particleFactory)
      : particleFactory(std::move(particleFactory)) {}

  std::function<void()> particleFactory;
  float frequency = 1.0f;
  float elapsed = 0.0f;
};

}  // namespace linguine
