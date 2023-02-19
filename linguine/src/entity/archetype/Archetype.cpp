#include "Archetype.h"

namespace linguine::archetype {

Archetype::Archetype(const std::unordered_map<std::type_index, size_t>& types)
    : _types{}, _store(types) {
  for (const auto& entry : types) {
    _types.insert(entry.first);
  }
}

bool Archetype::has(const std::type_info& typeInfo) const {
  return _types.find(typeInfo) != _types.end();
}

bool Archetype::has(const std::type_index& typeIndex) const {
  return _types.find(typeIndex) != _types.end();
}

const std::set<std::type_index>& Archetype::getTypes() const {
  return _types;
}

Archetype* Archetype::getParent(const std::type_info& missingType) {
  const auto parent = _parents.find(missingType);

  if (parent != _parents.end()) {
    return parent->second;
  }

  return nullptr;
}

void Archetype::addParent(const std::type_info& missingType, Archetype& archetype) {
  _parents.insert({ missingType, &archetype });
}

const std::unordered_map<std::type_index, Archetype*>& Archetype::getChildren() const {
  return _children;
}

Archetype* Archetype::getChild(const std::type_info& additionalType) {
  const auto child = _children.find(additionalType);

  if (child != _children.end()) {
    return child->second;
  }

  return nullptr;
}

void Archetype::addChild(const std::type_info& additionalType, Archetype& archetype) {
  _children.insert({additionalType, &archetype});
}

Archetype Archetype::branch(const std::type_info& additionalType, size_t size) const {
  auto types = std::unordered_map<std::type_index, size_t>();

  for (const auto& type : _types) {
    auto typeSize = _store.getSizeOf(type);
    types.insert({type, typeSize});
  }

  types.insert({additionalType, size});

  return Archetype(types);
}

Archetype Archetype::trunk(const std::type_info& missingType) const {
  auto types = std::unordered_map<std::type_index, size_t>();

  for (const auto& type : _types) {
    if (type == missingType) {
      continue;
    }

    auto typeSize = _store.getSizeOf(type);
    types.insert({type, typeSize});
  }

  return Archetype(types);
}

void Archetype::addEntity(uint64_t entityId) {
  if (_indicesById.find(entityId) != _indicesById.end()) {
    return;
  }

  const auto index = _store.reserve();
  _indicesById.insert({entityId, index});
}

void Archetype::removeEntity(uint64_t entityId) {
  const auto& idIndex = _indicesById.find(entityId);

  if (idIndex == _indicesById.end()) {
    return;
  }

  const auto index = idIndex->second;
  _store.release(index);
  _indicesById.erase(entityId);
}

void Archetype::each(const std::function<void(uint64_t)>& function) const {
  for (const auto& entry : _indicesById) {
    function(entry.first);
  }
}

void* Archetype::getComponent(uint64_t entityId, const std::type_info& typeInfo) const {
  const auto& idIndex = _indicesById.find(entityId);

  if (idIndex == _indicesById.end()) {
    return nullptr;
  }

  return _store.get(idIndex->second, typeInfo);
}

void Archetype::migrate(uint64_t entityId, Archetype& source, Archetype& destination) {
  const auto srcIndex = source._indicesById[entityId];

  destination.addEntity(entityId);
  const auto dstIndex = destination._indicesById[entityId];

  const auto& sourceTypes = source.getTypes();
  const auto& destinationTypes = destination.getTypes();

  const auto& typesToIterate = sourceTypes.size() < destinationTypes.size()
                                   ? sourceTypes : destinationTypes;

  for (const auto& type : typesToIterate) {
    auto* src = source._store.get(srcIndex, type);
    auto* dst = destination._store.get(dstIndex, type);
    const auto size = destination._store.getSizeOf(type);
    memcpy(dst, src, size);
  }

  source.removeEntity(entityId);
}

}  // namespace linguine::archetype