#pragma once

#include "entity/Entity.h"

namespace linguine {

class Action {
  public:
    virtual ~Action() = default;

    virtual void execute(Entity& target) = 0;
};

}  // namespace linguine
