#pragma once

#include <memory>
#include <set>
#include <typeindex>

namespace linguine {

class Entity;
class Result;

class EntityManager {
  public:
    virtual ~EntityManager() = default;

    virtual std::shared_ptr<Entity> create() = 0;

    virtual std::shared_ptr<Entity> getById(uint64_t id) = 0;

    template<typename... T>
    inline std::shared_ptr<Result> find() {
      return find({ typeid(T)... });
    }

  private:
    template<typename T>
    friend class Component;
    friend class Entity;

    virtual std::shared_ptr<Result> find(std::set<std::type_index> types) = 0;

    [[nodiscard]] virtual bool has(uint64_t id, const std::type_info& typeInfo) const = 0;

    virtual void* add(uint64_t id, const std::type_info& typeInfo, size_t size) = 0;

    virtual void remove(uint64_t id, const std::type_info& typeInfo) = 0;

    [[nodiscard]] virtual void* get(uint64_t id, const std::type_info& typeInfo) const = 0;
};

}  // namespace linguine