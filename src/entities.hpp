#pragma once

#include "components.hpp"

#include <any>
#include <cstdint>
#include <typeindex>
#include <unordered_map>
#include <raylib.h>

namespace crogersdev {

using Entity = uint16_t;

class Registry {
private:
    Entity nextId = 0;
    std::unordered_map<std::type_index, std::any> pools;

    /*
     * Top level entry is:
     * unordered_map<ComponentType, unordered_map<EntityID, ComponentType> >
     * where both CompnentTypes are the same.  
     * e.g.
     * "Here's all the entities that have Transform!"
     *
     * Transform -> {
     *     entity_id: 0 -> { pos, speed } // the Transform of entity 0
     *     entity_id: 1 -> { pos, speed }
     *     ...
     *   }
     * Velocity -> {
     *     entity_id::wq:wqa
     *     0 -> { dir{0, 0},  speed 10 } // Velocity of entity 0
     *     ...
     * }
     * PlayerInput -> {
     *     and so forth
     * }
     *
    */

    template<typename T>
    std::unordered_map<Entity, T>& getPool() {
        auto key = std::type_index(typeid(T));
        if (pools.find(key) == pools.end()) {
            pools[key] = std::unordered_map<Entity, T>{};
        }
        return std::any_cast<std::unordered_map<Entity, T>&>(pools[key]);
    }
public:
    Entity create() { return nextId++; }

    template<typename T>
    void add(Entity e, T component) {
        getPool<T>()[e] = std::move(component);
    }

    template<typename T>
    T& get(Entity e) {
        return getPool<T>().at(e);
    }

    template <typename T>
    bool has(Entity e) {
        auto& pool = getPool<T>();
        return pool.find(e) != pool.end();
    }

    template<typename First, typename... Rest>
    std::vector<Entity> view() {
        std::vector<Entity> matches = {};
        for (auto& [id, _] : getPool<First>()) {
            if ((has<Rest>(id) && ...)) {
                matches.push_back(id);
            }
        }
        return matches;
    }
};

}
