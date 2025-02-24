#pragma once

#include <entt/entt.hpp>
#include <SDL2/SDL.h>
#include "Object2D.hpp"
#include <ComponentManager.hpp>
#include <RegistryManager.hpp>
#include <memory>
#include <iostream>

class Renderer2D {
public:
    static Renderer2D& GetInstance();

    void Initialize(SDL_Renderer* sdlRenderer);

    void Render(float frame_duration);

    SDL_Renderer* GetSDLRenderer();

private:
    SDL_Renderer* renderer = nullptr;
    Vector2 last_camera_position = {0, 0};

    Renderer2D() = default;
    ~Renderer2D() = default;

    // Disallow copying and moving
    Renderer2D(const Renderer2D&) = delete;
    Renderer2D& operator=(const Renderer2D&) = delete;
    Renderer2D(Renderer2D&&) = delete;
    Renderer2D& operator=(Renderer2D&&) = delete;
};
