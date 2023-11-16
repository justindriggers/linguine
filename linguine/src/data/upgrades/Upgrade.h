#pragma once

#include <string>

namespace linguine {

struct Upgrade {
  std::string name;
  std::string description;
  std::vector<int16_t> rankCosts;
};

}  // namespace linguine
