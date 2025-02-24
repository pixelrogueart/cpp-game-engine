#pragma once

#include <vector>
#include <memory>
#include <sol/sol.hpp>
#include <string>
#include <iostream>
#include <RegistryManager.hpp>
#include <LuaManager.hpp>

class Component : public std::enable_shared_from_this<Component> {
public:
    entt::entity owner_entity;  // The entity this component belongs to
    entt::entity entity;        // The component's own entity ID
    sol::environment environment;  // Lua environment for scripting

    explicit Component();
    virtual ~Component();

    // Register the Component class in Lua
    static void Register();

    virtual void Emplace(entt::entity owner);
    
    // Update logic (can be overridden by derived classes)
    virtual void Update(float delta);

    // Access Lua environment
    sol::environment& GetEnvironment();
};
