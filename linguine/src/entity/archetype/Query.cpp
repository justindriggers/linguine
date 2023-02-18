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
    insertRecursively(results, visited, node);
    return;
  }

  visited.insert(node);

  for (const auto& child : node->getChildren()) {
    const auto* next = child.second;

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

void Query::insertRecursively(std::set<const Archetype*>& results,
                              std::set<const Archetype*>& visited,
                              const Archetype* node) {
  visited.insert(node);
  results.insert(node);

  for (const auto& child : node->getChildren()) {
    const auto* next = child.second;

    if (visited.find(next) != visited.end()) {
      continue;
    }

    insertRecursively(results, visited, next);
  }
}

}  // namespace linguine::archetype