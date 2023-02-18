#pragma once

#include <map>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Store.h"

namespace linguine::archetype {

class Archetype {
  public:
    static const uint64_t InvalidEntityId = UINT64_MAX;

    explicit Archetype(const std::unordered_map<std::type_index, size_t>& types);

    [[nodiscard]] bool has(const std::type_info& typeInfo) const;

    [[nodiscard]] bool has(const std::type_index& typeIndex) const;

    [[nodiscard]] const std::set<std::type_index>& getTypes() const;

    Archetype* getParent(const std::type_info& missingType);

    void addParent(const std::type_info& missingType, Archetype& archetype);

    [[nodiscard]] const std::unordered_map<std::type_index, Archetype*>& getChildren() const;

    Archetype* getChild(const std::type_info& additionalType);

    void addChild(const std::type_info& additionalType, Archetype& archetype);

    [[nodiscard]] Archetype branch(const std::type_info& additionalType, size_t size) const;

    [[nodiscard]] Archetype trunk(const std::type_info& missingType) const;

    [[nodiscard]] const std::vector<uint64_t>& getEntityIds() const;

    void addEntity(uint64_t entityId);

    void removeEntity(uint64_t entityId);

    [[nodiscard]] void* getComponent(uint64_t entityId, const std::type_info& typeInfo) const;

    static void migrate(uint64_t entityId, Archetype& source, Archetype& destination);

  private:
    std::set<std::type_index> _types;
    std::unordered_map<std::type_index, Archetype*> _parents;
    std::unordered_map<std::type_index, Archetype*> _children;

    Store _store;
    std::vector<uint64_t> _entityIds;
    std::map<uint64_t, size_t> _indicesById;
};

}  // namespace linguine::archetype