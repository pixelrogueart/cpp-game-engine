#include <CameraComponent.hpp>

// Constructor
CameraComponent::CameraComponent(bool is_current)
    : current(is_current), centered(true), smooth(0.0f), zoom(1.0f) {
    InitializeLuaBindings();
}

// Destructor
CameraComponent::~CameraComponent() {
    std::cout << "CameraComponent destroyed for entity ID: " << static_cast<int>(entity) << "\n";
}

// Emplace function
void CameraComponent::Emplace(entt::entity owner) {
    owner_entity = owner;

    // Explicitly cast the base pointer to the derived type
    auto self = std::dynamic_pointer_cast<CameraComponent>(shared_from_this());
    if (!self) {
        throw std::runtime_error("Failed to cast to CameraComponent");
    }

    // Register the explicitly casted pointer
    RegistryManager::GetInstance().emplace<std::shared_ptr<CameraComponent>>(owner, self);
}

// Lua Registration
void CameraComponent::Register() {
    sol::state& lua = LuaManager::GetInstance();
    lua.new_usertype<CameraComponent>("CameraComponent",
        sol::constructors<CameraComponent(bool)>(),
        "new", sol::factories([](bool is_current) {
            auto camera_instance = std::make_shared<CameraComponent>(is_current);
            RegistryManager::GetInstance().emplace<std::shared_ptr<Component>>(camera_instance->entity, camera_instance);
            RegistryManager::GetInstance().emplace<std::string>(camera_instance->entity, "CameraComponent");
            return camera_instance->GetEnvironment();
        }),
        "entity", &CameraComponent::entity,
        sol::base_classes, sol::bases<Component>()
    );
}

// Set Current
void CameraComponent::SetCurrent(bool is_current) {
    if (is_current) {
        // Disable the current flag for all other CameraComponents
        auto view = RegistryManager::GetInstance().view<std::shared_ptr<CameraComponent>>();
        for (auto entity : view) {
            auto& camera = view.get<std::shared_ptr<CameraComponent>>(entity);
            if (camera && camera.get() != this) {
                camera->current = false;
                camera->environment["current"] = false;
            }
        }
    }
    current = is_current;
    environment["current"] = is_current;
}

// Get Smooth
float CameraComponent::GetSmooth() const {
    return std::ref(smooth);
}

// Get Zoom
float CameraComponent::GetZoom() const {
    return std::ref(zoom);
}

// Get Centered
float CameraComponent::GetCentered() const {
    return std::ref(centered);
}

// Get Current
float CameraComponent::GetCurrent() const {
    return std::ref(current);
}

// Initialize Lua Bindings
void CameraComponent::InitializeLuaBindings() {
    environment["current"] = sol::as_table(std::ref(current));
    environment["centered"] = sol::as_table(std::ref(centered));
    environment["smooth"] = sol::as_table(std::ref(smooth));
    environment["zoom"] = sol::as_table(std::ref(zoom));

    environment["set_current"] = [this]() {
        SetCurrent(true);
    };

    environment["set_centered"] = [this](bool value) {
        centered = value;
        environment["centered"] = value;
    };

    environment["set_smooth"] = [this](float value) {
        smooth = value;
        environment["smooth"] = value;
    };

    environment["set_zoom"] = [this](float value) {
        if (value <= 0) {
            throw std::invalid_argument("Zoom must be greater than 0");
        }
        zoom = value;
        environment["zoom"] = value;
    };
}
