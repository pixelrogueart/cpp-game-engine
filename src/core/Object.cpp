#include <Object.hpp>

Object::Object() : entity(RegistryManager::GetInstance().create()) {
    std::cout << "Object created with entity ID: " << static_cast<int>(entity) << "\n";

    sol::state& lua = LuaManager::GetInstance();
    environment = sol::environment(lua, sol::create, lua.globals());
    environment["instance"] = this;
    environment["self"] = environment;
    environment["entity"] = entity;
    environment["parent"] = [this]() -> sol::object {
        auto parent = RegistryManager::GetInstance().get<std::shared_ptr<Object>>(parent_entity);
        if (parent) {
            return parent->GetEnvironment();
        }
        return sol::lua_nil;
    };
    environment["add_child"] = [this](sol::environment child) {
        if (child["entity"].valid()) {
            sol::optional<int> entity_id = child["entity"];
            entt::entity child_entity = static_cast<entt::entity>(entity_id.value());
            AddChild(child_entity);
        } else {
            throw std::runtime_error("Entity not found in userdata environment.");
        }
    };
    environment["add_component"] = [this](sol::environment component) {
        if (component["entity"].valid()) {
            sol::optional<int> entity_id = component["entity"];
            entt::entity component_entity = static_cast<entt::entity>(entity_id.value());
            AddComponent(component_entity);
        } else {
            throw std::runtime_error("Entity not found in userdata environment.");
        }
    };
    environment["set_script"] = [this](const std::string& file_path) {
        SetScript(file_path);
    };
}

std::shared_ptr<Object> Object::Create() {
    auto obj_instance = std::make_shared<Object>();
    RegistryManager::GetInstance().emplace<std::shared_ptr<Object>>(obj_instance->entity, obj_instance);
    RegistryManager::GetInstance().emplace<std::string>(obj_instance->entity, "Object");
    return obj_instance;
}

void Object::Register() {
    sol::state& lua = LuaManager::GetInstance();
    lua.new_usertype<Object>("Object",
        sol::constructors<Object()>(),
        "new", sol::factories([]() {
            return Object::Create()->GetEnvironment();
        }),
        "entity", &Object::entity
    );
}

void Object::SetScript(const std::string& file_path) {
    try {
        LuaManager::GetInstance().script_file(file_path, environment);
    } catch (const sol::error& err) {
        throw std::runtime_error("Failed to load Lua script file: " + std::string(err.what()));
    }
}

void Object::Process(float delta) {
    for (const entt::entity child_entity : children) {
        auto& child = RegistryManager::GetInstance().get<std::shared_ptr<Object>>(child_entity);
        child->Process(delta);
    }

    if (environment["process"].valid()) {
        try {
            environment["process"](delta);
        } catch (const sol::error& e) {
            std::cerr << "Error processing in Lua: " << e.what() << "\n";
        }
    }
}

void Object::ProcessInput(const SDL_Event& event) {
    if (environment["process_input"].valid()) {
        try {
            environment["process_input"](event);
        } catch (const sol::error& e) {
            std::cerr << "Error processing input in Lua: " << e.what() << "\n";
        }
    }

    for (const entt::entity child_entity : children) {
        auto& child = RegistryManager::GetInstance().get<std::shared_ptr<Object>>(child_entity);
        child->ProcessInput(event);
    }
}

void Object::AddChild(entt::entity child_entity) {
    std::cout << "Adding child to Object with entity ID: " << static_cast<int>(entity) << "\n";
    if (!RegistryManager::GetInstance().valid(child_entity)) {
        std::cerr << "Invalid child entity ID: " << static_cast<int>(child_entity) << "\n";
        return;
    }

    auto& child = RegistryManager::GetInstance().get<std::shared_ptr<Object>>(child_entity);
    std::cout << "Child entity ID: " << static_cast<int>(child->entity) << "\n";
    if (!child) {
        std::cerr << "Failed to retrieve the instance for entity ID: " << static_cast<int>(child_entity) << "\n";
        return;
    }
    child->parent_entity = entity;
    children.push_back(child_entity);
    std::cout << "Child added to Object with entity ID: " << static_cast<int>(child_entity) << "\n";
}

void Object::AddComponent(entt::entity component_entity) {
    if (!RegistryManager::GetInstance().valid(component_entity)) {
        std::cerr << "Invalid component entity ID: " << static_cast<int>(component_entity) << "\n";
        return;
    }

    auto& component = RegistryManager::GetInstance().get<std::shared_ptr<Component>>(component_entity);
    std::cout << "Component entity ID: " << static_cast<int>(component->entity) << "\n";
    if (!component) {
        std::cerr << "Failed to retrieve the instance for entity ID: " << static_cast<int>(component_entity) << "\n";
        return;
    }

    for (const auto& existing_entity : components) {
        auto& existing_component = RegistryManager::GetInstance().get<std::shared_ptr<Component>>(existing_entity);
        if (typeid(*component) == typeid(*existing_component)) {
            std::cerr << "A component of type " << typeid(*component).name() 
                      << " already exists for this object.\n";
            return;
        }
    }

    component->Emplace(entity);
    std::cout << "Component added to Object with entity ID: " << static_cast<int>(component_entity) << "\n";
}

sol::environment& Object::GetEnvironment() {
    return environment;
}
