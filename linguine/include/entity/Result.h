#pragma once

#include <functional>
#include <vector>

#include "Entity.h"

namespace linguine {

class Result {
  public:
    virtual ~Result() = default;

    virtual void each(std::function<void(Entity&)> function) = 0;

    virtual std::vector<std::shared_ptr<Entity>> get() = 0;
};

}  // namespace linguine