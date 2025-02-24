#include <InputComponent.hpp>

// Constructor
InputComponent::InputComponent() : Component() {
    InitializeLuaBindings();
}

// Destructor
InputComponent::~InputComponent() {
    std::cout << "InputComponent destroyed for entity ID: " << static_cast<int>(entity) << "\n";
}

// Register key bindings for actions
sol::environment InputComponent::RegisterAction(const std::string& action, const std::string& key_name, const std::string& event_type) {
    int key_sym = SDL_GetKeyFromName(key_name.c_str());
    uint32_t sdl_event_type = (event_type == "pressed") ? SDL_KEYDOWN :
                              (event_type == "released") ? SDL_KEYUP : 0;
    sol::state& lua = LuaManager::GetInstance();
    if (key_sym != 0 && sdl_event_type != 0) {
        // Add to key_bindings for internal C++ logic
        key_bindings[key_sym][sdl_event_type] = action;

        // Ensure lua_env["actions"] exists
        if (!environment["actions"].valid() || environment["actions"].get_type() != sol::type::table) {
            environment["actions"] = lua.create_table();
        }

        // Access or create the subtable for the specific action
        sol::table actions_table = environment["actions"];
        sol::table action_table;
        if (!actions_table[action].valid() || actions_table[action].get_type() != sol::type::table) {
            action_table = lua.create_table();
            actions_table[action] = action_table;
        } else {
            action_table = actions_table[action];
        }
        action_table[action_table.size() + 1] = key_name;
    }

    // Return the action table for this action
    return environment["actions"][action];
}

// Check action state
float InputComponent::GetActionStrength(const std::string& action) {
    for (const auto& [key, event_map] : key_bindings) {
        for (const auto& [event_type, mapped_action] : event_map) {
            if (mapped_action == action) {
                const Uint8* keyboard_state = SDL_GetKeyboardState(nullptr);
                if (keyboard_state[SDL_GetScancodeFromKey(key)]) {
                    return 1.0f; // Fully pressed
                }
            }
        }
    }
    return 0.0f; // Not pressed
}

// Check if an action is triggered by an event
bool InputComponent::IsActionTriggered(const sol::table& event_table, const sol::table& action_table) {
    // Retrieve the key symbol and event state from the event table
    int event_key_sym = event_table["key_sym"].get_or(-1); // Default to -1 if key_sym is not found
    std::string event_state = event_table["state"].get_or(std::string(""));

    if (event_key_sym == -1 || event_state.empty()) {
        return false;
    }

    // Iterate through the action table to find a match
    for (const auto& [action_index, action_key_name] : action_table) {
        if (action_key_name.get_type() == sol::type::string) {
            std::string key_name = action_key_name.as<std::string>();

            int action_key_sym = SDL_GetKeyFromName(key_name.c_str());

            if (action_key_sym == event_key_sym) {
                // Check if the state matches
                uint32_t expected_event_type = (event_state == "pressed") ? SDL_KEYDOWN :
                                               (event_state == "released") ? SDL_KEYUP : 0;

                if (expected_event_type != 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Bind Lua functionality
void InputComponent::Register() {
    sol::state& lua = LuaManager::GetInstance();
    lua.new_usertype<InputComponent>("InputComponent",
        sol::constructors<InputComponent()>(),
        "new", sol::factories([]() {
            auto input_instance = std::make_shared<InputComponent>();
            RegistryManager::GetInstance().emplace<std::shared_ptr<Component>>(input_instance->entity, input_instance);
            RegistryManager::GetInstance().emplace<std::string>(input_instance->entity, "InputComponent");
            return input_instance->GetEnvironment();
        }),
        "entity", &InputComponent::entity,
        sol::base_classes, sol::bases<Component>()
    );
}

// Emplace the component into the registry
void InputComponent::Emplace(entt::entity owner) {
    owner_entity = owner;

    // Explicitly cast the base pointer to the derived type
    auto self = std::dynamic_pointer_cast<InputComponent>(shared_from_this());
    if (!self) {
        throw std::runtime_error("Failed to cast to InputComponent");
    }

    // Register the explicitly casted pointer
    RegistryManager::GetInstance().emplace<std::shared_ptr<InputComponent>>(owner, self);
}

// Initialize Lua bindings
void InputComponent::InitializeLuaBindings() {
    environment["register_action"] = [this](const std::string& action, const std::string& key_name, const std::string& event_type) {
        return RegisterAction(action, key_name, event_type);
    };

    environment["get_action_strength"] = [this](const std::string& action) -> float {
        return GetActionStrength(action);
    };

    environment["is_action_triggered"] = [this](const sol::table& event_table, const sol::table& action_table) -> bool {
        return IsActionTriggered(event_table, action_table);
    };
}
