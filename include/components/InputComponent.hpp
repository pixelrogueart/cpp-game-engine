#pragma once

#include "Component.hpp"
#include <SDL2/SDL.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include <LuaManager.hpp>

class InputComponent : public Component {
public:
    std::unordered_map<int, std::unordered_map<uint32_t, std::string>> key_bindings; // Key -> {EventType -> Action} map

    explicit InputComponent();
    ~InputComponent() override;

    // Register key bindings for actions
    sol::environment RegisterAction(const std::string& action, const std::string& key_name, const std::string& event_type);

    // Check action state
    float GetActionStrength(const std::string& action);

    // Check if an action is triggered by an event
    bool IsActionTriggered(const sol::table& event_table, const sol::table& action_table);

    // Bind Lua functionality
    static void Register();

    // Emplace the component into the registry
    void Emplace(entt::entity owner) override;

private:
    // Initialize Lua bindings
    void InitializeLuaBindings();
};
