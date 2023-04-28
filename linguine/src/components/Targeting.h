#pragma once

#include <optional>

namespace linguine {

struct Targeting {
  enum Strategy {
    Random,
    Nearest,
    Adjacent
  };

  Strategy strategy = Random;
  float range = 10.0f;
  std::optional<uint64_t> current = {};
};

}  // namespace linguine
