#include <Component.hpp>


// Constructor
Component::Component() : entity(RegistryManager::GetInstance().create()) {
    std::cout << "Component created with entity ID: " << static_cast<int>(entity) << "\n";

    // Create Lua environment and bind the component
    sol::state& lua = LuaManager::GetInstance();
    environment = sol::environment(lua, sol::create, lua.globals());
    environment["instance"] = this;
    environment["entity"] = entity;
}

// Destructor
Component::~Component() {
    std::cout << "Component destroyed for entity ID: " << static_cast<int>(entity) << "\n";
}

// Register the Component class in Lua
void Component::Register() {
    sol::state& lua = LuaManager::GetInstance();
    lua.new_usertype<Component>("Component",
        sol::constructors<Component()>(),
        "new", sol::factories([]() {
            auto comp_instance = std::make_shared<Component>();
            RegistryManager::GetInstance().emplace<std::shared_ptr<Component>>(comp_instance->entity, comp_instance);
            RegistryManager::GetInstance().emplace<std::string>(comp_instance->entity, "Component");
            return comp_instance->GetEnvironment();
        }),
        "entity", &Component::entity
    );
}

// Emplace logic
void Component::Emplace(entt::entity owner) {
    owner_entity = owner;
    RegistryManager::GetInstance().emplace<std::shared_ptr<Component>>(owner, shared_from_this());
}

// Update logic (can be overridden by derived classes)
void Component::Update(float delta) {
    // Default implementation (can be overridden in derived classes)
}

// Access Lua environment
sol::environment& Component::GetEnvironment() {
    return environment;
}
