#pragma once

#include "entity/Result.h"

#include "Archetype.h"
#include "ArchetypeEntityManager.h"

namespace linguine::archetype {

class ArchetypeResult : public Result {
  public:
    explicit ArchetypeResult(ArchetypeEntityManager& entityManager,
                             std::set<const Archetype*> archetypes)
        : _entityManager(entityManager), _archetypes(std::move(archetypes)) {}

    ~ArchetypeResult() override = default;

    void each(const std::function<void(Entity&)>& function) const override;

    [[nodiscard]] std::vector<std::shared_ptr<Entity>> get() const override;

  private:
    ArchetypeEntityManager& _entityManager;
    std::set<const Archetype*> _archetypes;
};

}  // namespace linguine::archetype