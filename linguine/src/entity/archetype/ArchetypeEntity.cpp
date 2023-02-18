#include "ArchetypeEntity.h"

namespace linguine::archetype {

bool ArchetypeEntity::has(const std::type_info& typeInfo) const {
  return _entityManager.has(_id, typeInfo);
}

void* ArchetypeEntity::add(const std::type_info& typeInfo, size_t size) {
  return _entityManager.add(_id, typeInfo, size);
}

void ArchetypeEntity::remove(const std::type_info& typeInfo) {
  return _entityManager.remove(_id, typeInfo);
}

void* ArchetypeEntity::get(const std::type_info& typeInfo) const {
  return _entityManager.get(_id, typeInfo);
}

}  // namespace linguine::archetype