#pragma once

#include <typeinfo>

#include "Component.h"

namespace linguine {

class Entity {
  public:
    Entity(EntityManager& entityManager, uint64_t id)
        : _entityManager(entityManager), _id(id) {}

    [[nodiscard]] uint64_t getId() const {
      return _id;
    }

    template<typename T>
    [[nodiscard]] inline bool has() const {
      return _entityManager.has(_id, typeid(T));
    }

    template<typename T, typename... Args>
    inline Component<T> add(Args&&... args) {
      auto* ptr = _entityManager.add(_id, typeid(T), sizeof(T));
      new (static_cast<T*>(ptr)) T(std::forward<Args>(args)...);
      return get<T>();
    }

    template<typename T>
    inline void remove() {
      return _entityManager.remove(_id, typeid(T));
    }

    template<typename T>
    inline Component<T> get() const {
      return Component<T>(_entityManager, _id, typeid(T));
    }

  private:
    EntityManager& _entityManager;
    uint64_t _id;
};

}  // namespace linguine
