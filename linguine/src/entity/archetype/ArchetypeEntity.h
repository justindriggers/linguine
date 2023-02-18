#pragma once

#include "entity/Entity.h"

#include "ArchetypeEntityManager.h"

namespace linguine::archetype {

class ArchetypeEntity : public Entity {
  public:
    ArchetypeEntity(ArchetypeEntityManager& entityManager, uint64_t id)
        : _entityManager(entityManager), _id(id) {}

    ~ArchetypeEntity() override = default;

    uint64_t getId() override {
      return _id;
    }

  private:
    [[nodiscard]] bool has(const std::type_info& typeInfo) const override;

    void* add(const std::type_info& typeInfo, size_t size) override;

    void remove(const std::type_info& typeInfo) override;

    [[nodiscard]] void* get(const std::type_info& typeInfo) const override;

    ArchetypeEntityManager& _entityManager;
    uint64_t _id;
};

}  // namespace linguine::archetype