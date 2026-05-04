#pragma once

#include <any>
#include <cstdint>
#include <typeindex>
#include <unordered_map>
#include <raylib.h>

namespace crogersdev {

using Entity = uint16_t;

class Registry {
private:
    template <typename ...Ts>
    struct Exclude {};

    template <typename T>
    struct is_exclude_tag {
        static constexpr bool value = false;
    };

    template <typename ...Ts>
    struct is_exclude_tag<Exclude<Ts...>> {
        static constexpr bool value = true;
    };

    template <typename T>
    struct excluded_pack;

    Entity next_id = 0;
    std::unordered_map<std::type_index, std::any> pools;
    std::unordered_map<Entity, std::vector<std::type_index>> membership_index;
    std::unordered_map<std::type_index, std::function<void(Entity)>> erasers;

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
     *     entity_id: 0 -> { dir{0, 0},  speed 10 } // Velocity of entity 0
     *     ...
     * }
     * PlayerInput -> {
     *     and so forth
     * }
     *
    */

    template<typename T>
    std::unordered_map<Entity, T>& get_pool() {
        auto key = std::type_index(typeid(T));
        if (pools.find(key) == pools.end()) {
            pools[key] = std::unordered_map<Entity, T>{};
        }
        return std::any_cast<std::unordered_map<Entity, T>&>(pools[key]);
    }

public:
    Entity create() { return next_id++; }

    void destroy(Entity e) {
        for (auto component : membership_index[e]) {
            erasers[component](e);
        }
        membership_index.erase(e);
    }

    template<typename T>
    void add(Entity e, T component) {
        get_pool<T>()[e] = std::move(component);

        auto component_type = std::type_index(typeid(T));
        membership_index[e].push_back(component_type);

        // note: why do we create the erase function at add time:
        //       if we tried to just do pools[component_type].erase(),
        //       we'd have to do a std::any_cast jus tlike we do when we
        //       execute get_pool.
        //
        //       recall:
        //       pools is a map(component type -> map(entity_id -> component))
        //       so if you want to remove an entity, you have to do it across
        //       many pools.
        //
        //       so we keep track of which entities have which components in
        //       membership_index.
        //
        //       we could just brute force lookup every entity in every pool
        //       and delete the entity_id we care about if membership_index
        //       only held onto a human type of an entity.
        //
        //       but we don't need to.
        //
        //       we can use membership_index as map(entity_id -> list of components)
        //       and then iterate through erasers which is
        //       map(component type -> erase function(entity))
        //       which let's us NOT iterate through all components in the registry
        //       looking for our entity and instead just know ahead of time
        //       which component types our entity belongs to
        //       then erase our entity from every component pool it belongs to.
        //
        //       and we need to do it here, where we can std::any_cast to 
        //       component type from T.
        //

        erasers[component_type] = std::function<void(Entity)>([this, component_type](Entity e) {
            auto& pool = std::any_cast<std::unordered_map<Entity, T>&>(pools[component_type]);
            pool.erase(e);
        });
    }

    template<typename T>
    T& get(Entity e) {
        return get_pool<T>().at(e);
    }

    template <typename T>
    bool has(Entity e) {
        auto& pool = get_pool<T>();
        return pool.find(e) != pool.end();
    }

    template <typename ...Ts>
    struct excluded_pack<Exclude<Ts...>> {
        static bool none_present(Registry& r, Entity id) {
            return (!r.has<Ts>(id) && ...);
        }
    };

    template <typename T>
    bool search_with_filter(Entity id) {
        if constexpr (is_exclude_tag<T>::value) {
            return excluded_pack<T>::none_present(*this, id);
        } else {
            return has<T>(id);
        }
    }

    template<typename First, typename ...Rest>
    std::vector<Entity> view() {
        std::vector<Entity> matches = {};
        for (auto& [id, _] : get_pool<First>()) {
            if ((search_with_filter<Rest>(id) && ...)) {
                matches.push_back(id);
            }
        }
        return matches;
    }
};

}
