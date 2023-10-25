#pragma once

#include "entity/Entity.h"
#include "entity/Result.h"

namespace linguine {

class Action {
  public:
    virtual ~Action() = default;

    virtual void execute(Entity& target) = 0;
};

}  // namespace linguine
