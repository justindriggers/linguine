#pragma once

#include <optional>

namespace linguine {

struct Targeting {
  enum Strategy {
    Random
  };

  Strategy strategy = Random;
  std::optional<uint64_t> current = {};
};

}  // namespace linguine
