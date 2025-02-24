#pragma once

#include "Component.hpp"
#include <LuaManager.hpp>
#include <RegistryManager.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <stdexcept>

class ScriptComponent : public Component {
public:
    std::unordered_map<std::string, sol::environment> scripts;

    explicit ScriptComponent();

    // Add a script to the component and return its environment
    sol::environment AddScript(const std::string& scriptPath);

    // Remove a script from the component
    void RemoveScript(const std::string& scriptPath);

    // Get the Lua environment for a specific script
    sol::environment GetScriptEnvironment(const std::string& scriptPath);

    // List all loaded scripts (return paths as a table to Lua)
    std::vector<std::string> ListScripts() const;

    void Emplace(entt::entity owner) override;

    // Lua Registration
    static void Register();

    ~ScriptComponent() override;

private:
    // Initialize Lua environment bindings
    void InitializeLuaBindings();
};
