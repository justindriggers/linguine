#pragma once

namespace linguine {

struct Health {
  explicit Health(int32_t max) : max(max), current(max) {}

  int32_t current{};
  int32_t max{};
};

}  // namespace linguine
