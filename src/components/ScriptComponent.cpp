#include <ScriptComponent.hpp>

ScriptComponent::ScriptComponent()
    : Component() {
    InitializeLuaBindings();
}

sol::environment ScriptComponent::AddScript(const std::string& scriptPath) {
    try {
        sol::state& lua = LuaManager::GetInstance();
        sol::environment scriptEnv(lua, sol::create, lua.globals());
        lua.script_file(scriptPath, scriptEnv);
        scripts[scriptPath] = scriptEnv;
        std::cout << "Loaded script: " << scriptPath << "\n";
        return scriptEnv; // Return the environment of the added script
    } catch (const sol::error& e) {
        std::cerr << "Error loading script: " << e.what() << "\n";
        throw; // Rethrow to notify Lua of the error
    }
}

void ScriptComponent::RemoveScript(const std::string& scriptPath) {
    auto it = scripts.find(scriptPath);
    if (it != scripts.end()) {
        scripts.erase(it);
        std::cout << "Removed script: " << scriptPath << "\n";
    } else {
        std::cerr << "Script not found: " << scriptPath << "\n";
    }
}

sol::environment ScriptComponent::GetScriptEnvironment(const std::string& scriptPath) {
    auto it = scripts.find(scriptPath);
    if (it != scripts.end()) {
        return it->second;
    } else {
        throw std::runtime_error("Script not found: " + scriptPath);
    }
}

std::vector<std::string> ScriptComponent::ListScripts() const {
    std::vector<std::string> scriptPaths;
    for (const auto& [path, _] : scripts) {
        scriptPaths.push_back(path);
    }
    return scriptPaths;
}

void ScriptComponent::Emplace(entt::entity owner) {
    owner_entity = owner;

    // Explicitly cast the base pointer to the derived type
    auto self = std::dynamic_pointer_cast<ScriptComponent>(shared_from_this());
    if (!self) {
        throw std::runtime_error("Failed to cast to ScriptComponent");
    }

    // Register the explicitly casted pointer
    RegistryManager::GetInstance().emplace<std::shared_ptr<ScriptComponent>>(owner, self);
}

void ScriptComponent::Register() {
    sol::state& lua = LuaManager::GetInstance();
    lua.new_usertype<ScriptComponent>("ScriptComponent",
        sol::constructors<ScriptComponent()>(),
        "new", sol::factories([]() {
            auto script_instance = std::make_shared<ScriptComponent>();
            RegistryManager::GetInstance().emplace<std::shared_ptr<Component>>(script_instance->entity, script_instance);
            RegistryManager::GetInstance().emplace<std::string>(script_instance->entity, "ScriptComponent");
            return script_instance->GetEnvironment();
        }),
        "entity", &ScriptComponent::entity,
        "add_script", &ScriptComponent::AddScript,
        "remove_script", &ScriptComponent::RemoveScript,
        "get_script", &ScriptComponent::GetScriptEnvironment,
        "list_scripts", &ScriptComponent::ListScripts,
        sol::base_classes, sol::bases<Component>()
    );
}

ScriptComponent::~ScriptComponent() {
    std::cout << "ScriptComponent destroyed for entity ID: " << static_cast<int>(entity) << "\n";
}

void ScriptComponent::InitializeLuaBindings() {
    environment["add_script"] = [this](const std::string& scriptPath) {
        return AddScript(scriptPath);
    };
    environment["remove_script"] = [this](const std::string& scriptPath) {
        RemoveScript(scriptPath);
    };
    environment["get_script"] = [this](const std::string& scriptPath) {
        return GetScriptEnvironment(scriptPath);
    };
    environment["list_scripts"] = [this]() -> std::vector<std::string> {
        return ListScripts();
    };
}
