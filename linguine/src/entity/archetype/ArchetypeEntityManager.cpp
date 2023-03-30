#include "ArchetypeEntityManager.h"

#include "Archetype.h"
#include "ArchetypeResult.h"
#include "Query.h"

namespace linguine::archetype {

ArchetypeEntityManager::ArchetypeEntityManager()
    : _componentArchetypes{{{}, Archetype({})}} {
  _rootArchetype = &_componentArchetypes.at({});
}

std::shared_ptr<Entity> ArchetypeEntityManager::create() {
  if (!_availableIndices.empty()) {
    const auto index = _availableIndices.front();
    _availableIndices.pop();
    return std::make_shared<Entity>(*this, index);
  }

  const auto index = _entities.size();
  _rootArchetype->addEntity(index);
  _entities.push_back({_rootArchetype, {}});

  return std::make_shared<Entity>(*this, index);
}

std::shared_ptr<Entity> ArchetypeEntityManager::getById(uint64_t id) {
  return std::make_shared<Entity>(*this, id);
}

void ArchetypeEntityManager::destroy(uint64_t id) {
  auto& entity = _entities[id];
  auto currentArchetype = entity.currentArchetype;

  for (const auto& type : currentArchetype->getTypes()) {
    auto removalListener = entity.removalListeners.find(type);

    if (removalListener != entity.removalListeners.end()) {
      // Copy and erase prior to execution to prevent recursive execution
      auto function = removalListener->second;
      entity.removalListeners.erase(type);
      function(Entity(*this, id));
    }
  }

  Archetype::migrate(id, *currentArchetype, *_rootArchetype);
  entity.currentArchetype = _rootArchetype;
  _availableIndices.push(id);
}

std::shared_ptr<Result> ArchetypeEntityManager::find(std::set<std::type_index> types) {
  const auto query = Query(_rootArchetype, std::move(types));
  const auto archetypes = query.execute();

  return std::make_shared<ArchetypeResult>(*this, archetypes);
}

bool ArchetypeEntityManager::has(uint64_t id, const std::type_info& typeInfo) const {
  const auto archetype = _entities[id].currentArchetype;
  return archetype->has(typeInfo);
}

void* ArchetypeEntityManager::add(uint64_t id, const std::type_info& typeInfo, size_t size) {
  auto& entity = _entities[id];
  auto currentArchetype = entity.currentArchetype;

  if (currentArchetype->has(typeInfo)) {
    return get(id, typeInfo);
  }

  auto* newArchetype = currentArchetype->getChild(typeInfo);

  if (!newArchetype) {
    auto types = std::set<std::type_index>(currentArchetype->getTypes());
    types.insert(typeInfo);

    auto existingArchetype = _componentArchetypes.find(types);

    if (existingArchetype != _componentArchetypes.end()) {
      newArchetype = &existingArchetype->second;
    } else {
      _componentArchetypes.insert({types, std::move(currentArchetype->branch(typeInfo, size))});
      newArchetype = &_componentArchetypes.at(types);
    }

    currentArchetype->addChild(typeInfo, *newArchetype);
    newArchetype->addParent(typeInfo, *currentArchetype);
  }

  Archetype::migrate(id, *currentArchetype, *newArchetype);
  entity.currentArchetype = newArchetype;

  return newArchetype->getComponent(id, typeInfo);
}

void ArchetypeEntityManager::remove(uint64_t id, const std::type_info& typeInfo) {
  auto& entity = _entities[id];
  auto currentArchetype = entity.currentArchetype;

  if (!currentArchetype->has(typeInfo)) {
    return;
  }

  auto* newArchetype = currentArchetype->getParent(typeInfo);

  if (!newArchetype) {
    auto types = std::set<std::type_index>(currentArchetype->getTypes());
    types.erase(typeInfo);

    auto existingArchetype = _componentArchetypes.find(types);

    if (existingArchetype != _componentArchetypes.end()) {
      newArchetype = &existingArchetype->second;
    } else {
      _componentArchetypes.insert({types, std::move(currentArchetype->trunk(typeInfo))});
      newArchetype = &_componentArchetypes.at(types);
    }

    currentArchetype->addParent(typeInfo, *newArchetype);
    newArchetype->addChild(typeInfo, *currentArchetype);
  }

  auto removalListener = entity.removalListeners.find(typeInfo);

  if (removalListener != entity.removalListeners.end()) {
    // Copy and erase prior to execution to prevent recursive execution
    auto function = removalListener->second;
    entity.removalListeners.erase(typeInfo);
    function(Entity(*this, id));
  }

  Archetype::migrate(id, *currentArchetype, *newArchetype);
  entity.currentArchetype = newArchetype;
}

void* ArchetypeEntityManager::get(uint64_t id, const std::type_info& typeInfo) const {
  const auto archetype = _entities[id].currentArchetype;
  return archetype->getComponent(id, typeInfo);
}

void ArchetypeEntityManager::setRemovalListener(uint64_t id, const std::type_info& typeInfo,
                                                std::function<void(const Entity)> function) {
  auto& entity = _entities[id];
  entity.removalListeners.insert({typeInfo, std::move(function)});
}

}  // namespace linguine::archetype