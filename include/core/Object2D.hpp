#pragma once

#include <Object.hpp>
#include <Vector2.hpp>
#include <LuaManager.hpp>
#include <RegistryManager.hpp>
#include <memory>
#include <sol/sol.hpp>

class Object2D : public Object {
public:
    Vector2 position;         // Local position
    Vector2 global_position;  // Global position

    explicit Object2D();
    ~Object2D() override = default;

    static void Register();

    void Process(float delta) override;

    void SetPosition(float x, float y);
    Vector2 GetPosition();

    void SetGlobalPosition(float x, float y);
    Vector2 GetGlobalPosition();

    void SetParent(entt::entity parent_entity_ref);
    entt::entity GetParent() const;

    static std::shared_ptr<Object2D> Create();

private:
    void UpdateGlobalPosition();
    void UpdateLocalPosition();

    bool position_dirty;         // Tracks changes in local position
    bool global_position_dirty;  // Tracks changes in global position
};
