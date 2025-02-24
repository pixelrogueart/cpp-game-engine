#include <Object2D.hpp>

Object2D::Object2D() : Object(), position(0.0f, 0.0f), global_position(0.0f, 0.0f) {
    sol::state& lua = LuaManager::GetInstance();
    environment["position"] = sol::as_table(std::ref(position));
    environment["global_position"] = sol::as_table(std::ref(global_position));
    environment["set_position"] = [this](float x, float y) {
        SetPosition(x, y);
    };
    environment["get_position"] = [this]() {
        return GetPosition();
    };
    environment["set_global_position"] = [this](float x, float y) {
        SetGlobalPosition(x, y);
    };
    environment["get_global_position"] = [this]() {
        return GetGlobalPosition();
    };
}

void Object2D::Register() {
    sol::state& lua = LuaManager::GetInstance();
    lua.new_usertype<Object2D>("Object2D",
        sol::constructors<Object2D()>(),
        "new", sol::factories([]() {
            return Object2D::Create()->GetEnvironment();
        }),
        "entity", &Object2D::entity,
        sol::base_classes, sol::bases<Object>() // Inherit from Object
    );
}

void Object2D::Process(float delta) {
    Object::Process(delta);

    if (position_dirty) {
        UpdateGlobalPosition();
        position_dirty = false;
    }
    if (global_position_dirty) {
        UpdateLocalPosition();
        global_position_dirty = false;
    }
}

void Object2D::SetPosition(float x, float y) {
    if (this->position.x != x || this->position.y != y) {
        this->position.x = x;
        this->position.y = y;
        this->position_dirty = true;
    }
}

Vector2 Object2D::GetPosition() {
    if (position_dirty) {
        UpdateLocalPosition();
        position_dirty = false;
    }
    return position;
}

void Object2D::SetGlobalPosition(float x, float y) {
    if (this->global_position.x != x || this->global_position.y != y) {
        this->global_position.x = x;
        this->global_position.y = y;
        global_position_dirty = true;
    }
}

Vector2 Object2D::GetGlobalPosition() {
    if (global_position_dirty) {
        UpdateGlobalPosition();
        global_position_dirty = false;
    }
    return global_position;
}

void Object2D::SetParent(entt::entity parent_entity_ref) {
    parent_entity = parent_entity_ref;
    position_dirty = true; // Parent affects global position
}

entt::entity Object2D::GetParent() const {
    return parent_entity;
}

std::shared_ptr<Object2D> Object2D::Create() {
    auto obj_instance = std::make_shared<Object2D>();
    RegistryManager::GetInstance().emplace<std::shared_ptr<Object>>(obj_instance->entity, obj_instance);
    RegistryManager::GetInstance().emplace<std::string>(obj_instance->entity, "Object2D");
    return obj_instance;
}

void Object2D::UpdateGlobalPosition() {
    auto& registry = RegistryManager::GetInstance();
    if (registry.valid(parent_entity)) { // Ensure the parent entity exists
        auto& parent = registry.get<std::shared_ptr<Object>>(parent_entity);
        if (parent) {
            auto parent2D = std::dynamic_pointer_cast<Object2D>(parent);
            if (parent2D) {
                auto parent_global = parent2D->GetGlobalPosition();
                this->global_position.x = this->position.x + parent_global.x;
                this->global_position.y = this->position.y + parent_global.y;
            } else {
                global_position = this->position; // Parent isn't 2D
            }
        } else {
            this->global_position = this->position; // No parent
        }
    } else {
        this->global_position = this->position; // No parent
    }
}

void Object2D::UpdateLocalPosition() {
    auto& registry = RegistryManager::GetInstance();
    if (registry.valid(parent_entity)) { // Ensure the parent entity exists
        auto& parent = registry.get<std::shared_ptr<Object>>(parent_entity);
        if (parent) {
            auto parent2D = std::dynamic_pointer_cast<Object2D>(parent);
            if (parent2D) {
                auto parent_global = parent2D->GetGlobalPosition();
                this->position.x = this->global_position.x - parent_global.x;
                this->position.y = this->global_position.y - parent_global.y;
            } else {
                this->position = this->global_position; // Parent isn't 2D
            }
        } else {
            this->position = this->global_position; // No parent
        }
    } else {
        this->position = this->global_position; // No parent
    }
}
