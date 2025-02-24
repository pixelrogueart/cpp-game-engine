#pragma once
#include <Vector2.hpp>
#include <Object.hpp>
#include <Object2D.hpp>
#include <LuaManager.hpp>
#include <CameraComponent.hpp>
#include <InputComponent.hpp>
#include <ScriptComponent.hpp>
#include <SpriteComponent.hpp>

inline void RegisterComponents() {
    sol::state& lua = LuaManager::GetInstance();
    Object::Register();
    Object2D::Register();
    Vector2::Register();
    Component::Register();
    SpriteComponent::Register();
    InputComponent::Register();
    ScriptComponent::Register();
    CameraComponent::Register();

}
