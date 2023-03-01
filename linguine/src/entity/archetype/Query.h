#pragma once

#include <set>

#include "Archetype.h"

namespace linguine::archetype {

class Query {
  public:
    explicit Query(const Archetype* root, std::set<std::type_index> requiredTypes)
        : _root(root), _requiredTypes(std::move(requiredTypes)) {}

    [[nodiscard]] std::set<const Archetype*> execute() const;

  private:
    const Archetype* _root;
    std::set<std::type_index> _requiredTypes;

    void visit(std::set<const Archetype*>& results,
               std::set<const Archetype*>& visited,
               const Archetype* node) const;

    bool test(const Archetype* node) const;
};

}  // namespace linguine::archetype