#pragma once

#include <typeinfo>

namespace linguine {

class Entity {
  public:
    virtual ~Entity() = default;

    virtual uint64_t getId() = 0;

    template<typename T>
    [[nodiscard]] inline bool has() const {
      return has(typeid(T));
    }

    template<typename T, typename... Args>
    inline T* add(Args&&... args) {
      return new(static_cast<T*>(add(typeid(T), sizeof(T)))) T(std::forward<Args>(args)...);
    }

    template<typename T>
    inline void remove() {
      return remove(typeid(T));
    }

    template<typename T>
    inline T* get() const {
      return static_cast<T*>(get(typeid(T)));
    }

  private:
    [[nodiscard]] virtual bool has(const std::type_info& typeInfo) const = 0;

    virtual void* add(const std::type_info& typeInfo, size_t size) = 0;

    virtual void remove(const std::type_info& typeInfo) = 0;

    [[nodiscard]] virtual void* get(const std::type_info& typeInfo) const = 0;
};

}  // namespace linguine
