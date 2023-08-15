#pragma once

#include <unordered_set>

namespace linguine {

struct Hit {
  std::unordered_set<uint64_t> entityIds{};
};

}  // namespace linguine
