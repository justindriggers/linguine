#pragma once

#include <string>

namespace linguine {

struct Upgrade {
  std::string name;
  std::string description;
  int16_t cost;
  int8_t ranks;
};

}  // namespace linguine
