#pragma once
#include <Component.hpp>
#include <RegistryManager.hpp>
#include <LuaManager.hpp>
#include <stdexcept>
#include <iostream>

class CameraComponent : public Component {
public:
    bool current;       // Indicates if this is the active camera
    bool centered;      // Indicates if the camera is centered on the target
    float smooth;       // Smooth transition factor (0 = instant, >0 = smoother)
    float zoom;         // Zoom factor (1.0 = no zoom)

    explicit CameraComponent(bool is_current = false);
    ~CameraComponent() override;

    void Emplace(entt::entity owner) override;

    static void Register();

    void SetCurrent(bool is_current);
    float GetSmooth() const;
    float GetZoom() const;
    float GetCentered() const;
    float GetCurrent() const;

private:
    void InitializeLuaBindings();
};
