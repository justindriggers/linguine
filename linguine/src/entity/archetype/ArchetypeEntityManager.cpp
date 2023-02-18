#include "ArchetypeEntityManager.h"

#include "Archetype.h"
#include "ArchetypeEntity.h"
#include "ArchetypeResult.h"
#include "Query.h"

namespace linguine::archetype {

ArchetypeEntityManager::ArchetypeEntityManager()
    : _componentArchetypes{{{}, Archetype({})}} {
  _rootArchetype = &_componentArchetypes.at({});
}

std::shared_ptr<Entity> ArchetypeEntityManager::create() {
  const auto index = _entityArchetypes.size();
  _rootArchetype->addEntity(index);
  _entityArchetypes.emplace_back(*_rootArchetype);

  return std::make_shared<ArchetypeEntity>(*this, index);
}

std::shared_ptr<Result> ArchetypeEntityManager::find(std::set<std::type_index> types) {
  const auto query = Query(_rootArchetype, std::move(types));
  const auto archetypes = query.execute();

  return std::make_shared<ArchetypeResult>(*this, archetypes);
}

bool ArchetypeEntityManager::has(uint64_t id, const std::type_info& typeInfo) const {
  const auto& archetype = _entityArchetypes[id].get();
  return archetype.has(typeInfo);
}

void* ArchetypeEntityManager::add(uint64_t id, const std::type_info& typeInfo, size_t size) {
  auto& currentArchetype = _entityArchetypes[id].get();

  if (currentArchetype.has(typeInfo)) {
    return get(id, typeInfo);
  }

  auto* newArchetype = currentArchetype.getChild(typeInfo);

  if (!newArchetype) {
    auto types = std::set<std::type_index>(currentArchetype.getTypes());
    types.insert(typeInfo);

    auto existingArchetype = _componentArchetypes.find(types);

    if (existingArchetype != _componentArchetypes.end()) {
      newArchetype = &existingArchetype->second;
    } else {
      _componentArchetypes.insert({types, std::move(currentArchetype.branch(typeInfo, size))});
      newArchetype = &_componentArchetypes.at(types);
    }

    currentArchetype.addChild(typeInfo, *newArchetype);
    newArchetype->addParent(typeInfo, currentArchetype);
  }

  Archetype::migrate(id, currentArchetype, *newArchetype);
  _entityArchetypes[id] = *newArchetype;

  return newArchetype->getComponent(id, typeInfo);
}

void ArchetypeEntityManager::remove(uint64_t id, const std::type_info& typeInfo) {
  auto& currentArchetype = _entityArchetypes[id].get();

  if (!currentArchetype.has(typeInfo)) {
    return;
  }

  auto* newArchetype = currentArchetype.getParent(typeInfo);

  if (!newArchetype) {
    auto types = std::set<std::type_index>(currentArchetype.getTypes());
    types.erase(typeInfo);

    auto existingArchetype = _componentArchetypes.find(types);

    if (existingArchetype != _componentArchetypes.end()) {
      newArchetype = &existingArchetype->second;
    } else {
      _componentArchetypes.insert({types, std::move(currentArchetype.trunk(typeInfo))});
      newArchetype = &_componentArchetypes.at(types);
    }

    currentArchetype.addParent(typeInfo, *newArchetype);
    newArchetype->addChild(typeInfo, *newArchetype);
  }

  Archetype::migrate(id, currentArchetype, *newArchetype);
  _entityArchetypes[id] = *newArchetype;
}

void* ArchetypeEntityManager::get(uint64_t id, const std::type_info& typeInfo) const {
  const auto& archetype = _entityArchetypes.at(id).get();

  if (!archetype.has(typeInfo)) {
    return nullptr;
  }

  return archetype.getComponent(id, typeInfo);
}

}  // namespace linguine::archetype