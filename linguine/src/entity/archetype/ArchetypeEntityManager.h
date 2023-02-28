#pragma once

#include "entity/EntityManager.h"

#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Archetype.h"
#include "entity/Entity.h"
#include "entity/Result.h"

template<>
struct std::hash<std::set<std::type_index>> {
  size_t operator()(const std::set<std::type_index>& types) const {
    const auto typeHasher = std::hash<std::type_index>();

    size_t res = 17;

    for (const auto& type : types) {
      res = res * 31 + typeHasher(type);
    }

    return res;
  }
};

namespace linguine::archetype {

class ArchetypeEntityManager : public EntityManager {
  public:
    ArchetypeEntityManager();

    ~ArchetypeEntityManager() override = default;

    std::shared_ptr<Entity> create() override;

    std::shared_ptr<Entity> getById(uint64_t id) override;

  private:
    std::shared_ptr<Result> find(std::set<std::type_index> types) override;

    [[nodiscard]] bool has(uint64_t id, const std::type_info& typeInfo) const override;

    void* add(uint64_t id, const std::type_info& typeInfo, size_t size) override;

    void remove(uint64_t id, const std::type_info& typeInfo) override;

    [[nodiscard]] void* get(uint64_t id, const std::type_info& typeInfo) const override;

    Archetype* _rootArchetype;
    std::vector<std::reference_wrapper<Archetype>> _entityArchetypes;
    std::unordered_map<std::set<std::type_index>, Archetype> _componentArchetypes;
};

}  // namespace linguine::archetype