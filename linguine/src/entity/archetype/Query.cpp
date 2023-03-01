#include "Query.h"

namespace linguine::archetype {

std::set<const Archetype*> Query::execute() const {
  auto results = std::set<const Archetype*>();
  auto visited = std::set<const Archetype*>();

  visit(results, visited, _root);

  return results;
}

void Query::visit(std::set<const Archetype*>& results,
                  std::set<const Archetype*>& visited,
                  const Archetype* node) const {
  if (test(node)) {
    results.insert(node);
  }

  visited.insert(node);

  for (const auto& child : node->getChildren()) {
    const auto* next = child.second;

    if (visited.find(next) != visited.end()) {
      continue;
    }

    visit(results, visited, next);
  }

  for (const auto& parent : node->getParents()) {
    const auto* next = parent.second;

    if (visited.find(next) != visited.end()) {
      continue;
    }

    visit(results, visited, next);
  }
}

bool Query::test(const Archetype* node) const {
  for (const auto& type : _requiredTypes) {
    if (!node->has(type)) {
      return false;
    }
  }

  return true;
}

}  // namespace linguine::archetype