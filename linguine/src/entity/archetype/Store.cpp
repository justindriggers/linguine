#include "Store.h"

#include <algorithm>

namespace linguine::archetype {

Store::Store(const std::unordered_map<std::type_index, size_t>& types)
    : _types{}, _chunk{}, _size{}, _maxEntities{}, _nextIndex{} {
  size_t offset = 0;
  for (const auto& entry : types) {
    _types.insert({entry.first, {entry.second, offset}});
    offset += entry.second;
  }

  _entitySize = offset;
}

Store::~Store() {
  if (_chunk) {
    free(_chunk);
  }
}

size_t Store::reserve() {
  if (!_availableIndices.empty()) {
    auto index = _availableIndices.front();
    _availableIndices.pop();
    return index;
  }

  const auto index = _nextIndex++;

  while (_entitySize > 0 && index >= _maxEntities) {
    ensureSize(std::max(static_cast<size_t>(16 * 1024), _size * 2));
  }

  return index;
}

void Store::release(size_t index) {
  _availableIndices.push(index);
}

void* Store::get(size_t index, const std::type_index& typeInfo) const {
  const auto typeMetadata = _types.find(typeInfo);

  if (typeMetadata == _types.end()) {
    return nullptr;
  }

  return static_cast<std::byte*>(_chunk)
         + _entitySize * index
         + typeMetadata->second.offset;
}

size_t Store::getSizeOf(const std::type_index& typeInfo) const {
  return _types.at(typeInfo).size;
}

void Store::ensureSize(size_t size) {
  if (size <= _size) {
    return;
  }

  auto* chunk = malloc(size);

  if (_chunk) {
    memcpy(chunk, _chunk, _size);
    free(_chunk);
  }

  _chunk = chunk;
  _size = size;
  _maxEntities = _size / _entitySize;
}

}  // namespace linguine::archetype
