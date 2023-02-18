#pragma once

#include <memory>
#include <set>
#include <typeindex>

#include "Entity.h"
#include "Result.h"

namespace linguine {

class EntityManager {
  public:
    virtual ~EntityManager() = default;

    virtual std::shared_ptr<Entity> create() = 0;

    template<typename... T>
    inline std::shared_ptr<Result> find() {
      return find({ typeid(T)... });
    }

  private:
    virtual std::shared_ptr<Result> find(std::set<std::type_index> types) = 0;
};

}  // namespace linguine