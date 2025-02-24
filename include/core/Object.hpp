#pragma once

#include <vector>
#include <memory>
#include <sol/sol.hpp>
#include <string>
#include <stdexcept>
#include <iostream>
#include <SDL2/SDL.h>
#include <RegistryManager.hpp>
#include <LuaManager.hpp>
#include <Component.hpp>

class Object : public std::enable_shared_from_this<Object> {
public:
    entt::entity parent_entity;
    entt::entity entity;
    sol::environment environment;
    std::vector<entt::entity> children;
    std::vector<entt::entity> components;

    explicit Object();

    static std::shared_ptr<Object> Create();

    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;
    Object(Object&&) = delete;
    Object& operator=(Object&&) = delete;

    virtual ~Object() = default;

    // Register the Object class in Lua
    static void Register();

    virtual void SetScript(const std::string& file_path);
    virtual void Process(float delta);
    virtual void ProcessInput(const SDL_Event& event);
    void AddChild(entt::entity child_entity);
    void AddComponent(entt::entity component_entity);

    // Retrieve the Lua environment for this Object
    sol::environment& GetEnvironment();
};
