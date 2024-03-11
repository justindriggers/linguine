#pragma once

#include <typeinfo>

#include "EntityManager.h"

namespace linguine {

template<typename T>
class Component {
  public:
    Component(EntityManager& entityManager, uint64_t entityId, const std::type_info& typeInfo)
        : _entityManager(entityManager), _entityId(entityId), _typeInfo(typeInfo) {}

    inline T* operator->() const {
      return static_cast<T*>(_entityManager.get(_entityId, _typeInfo));
    }

    inline T& operator*() const {
      return *static_cast<T*>(_entityManager.get(_entityId, _typeInfo));
    }

    inline void setRemovalListener(std::function<void(const Entity)> function) {
      _entityManager.setRemovalListener(_entityId, _typeInfo, std::move(function));
    }

  private:
    EntityManager& _entityManager;
    uint64_t _entityId;
    const std::type_info& _typeInfo;
};

}  // namespace linguine
