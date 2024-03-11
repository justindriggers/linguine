#pragma once

#include "physics/QuadTree.h"

namespace linguine {

struct WorldNode {
  physics::QuadTree* parent;
};

}  // namespace linguine
