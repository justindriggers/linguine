#pragma once

#include <functional>
#include <vector>

#include "Entity.h"

namespace linguine {

class Result {
  public:
    virtual ~Result() = default;

    virtual void each(const std::function<void(Entity&)>& function) const = 0;

    [[nodiscard]] virtual std::vector<std::shared_ptr<Entity>> get() const = 0;
};

}  // namespace linguine