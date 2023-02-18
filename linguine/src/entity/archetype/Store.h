#pragma once

#include <queue>
#include <typeindex>
#include <unordered_map>

namespace linguine::archetype {

class Store {
  public:
    explicit Store(const std::unordered_map<std::type_index, size_t>& types);

    size_t reserve();

    void release(size_t index);

    [[nodiscard]] void* get(size_t index, const std::type_index& typeInfo) const;

    [[nodiscard]] size_t getSizeOf(const std::type_index& typeInfo) const;

  private:
    struct TypeMetadata {
      size_t size;
      size_t offset;
    };

    std::unordered_map<std::type_index, TypeMetadata> _types;
    size_t _entitySize;

    void* _chunk;
    size_t _size;
    size_t _maxEntities;

    size_t _nextIndex;
    std::queue<size_t> _availableIndices;

    void ensureSize(size_t size);
};

}  // namespace linguine::archetype
