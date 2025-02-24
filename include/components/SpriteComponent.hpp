#pragma once

#include "Component.hpp"
#include <LuaManager.hpp>
#include <RegistryManager.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>
#include <memory>

class SpriteComponent : public Component {
public:
    std::string texturePath; // Path to the sprite texture
    std::string currentTexturePath; // Current texture path
    SDL_Texture* texture = nullptr; // Loaded SDL texture
    int hframes = 1;         // Number of horizontal frames
    int vframes = 1;         // Number of vertical frames
    int frame = 0;           // Current frame index
    SDL_Rect frame_coords = {0, 0, 0, 0}; // Coordinates of the frame in the texture
    bool flipped_h = false;  // Horizontal flip
    bool flipped_v = false;  // Vertical flip

    explicit SpriteComponent(const std::string& path = "");
    ~SpriteComponent() override;

    // Load the texture
    void LoadTexture(const std::string& path, SDL_Renderer* renderer);

    // Set the frame and update frame coordinates
    void SetFrame(int f);

    // Update frame coordinates based on the texture size
    void UpdateFrameCoords();

    // Render the sprite
    void Render(SDL_Renderer* renderer, int x, int y);

    void Emplace(entt::entity owner) override;

    void SetTexturePath(const std::string& path);

    // Lua Registration
    static void Register();

private:
    // Initialize Lua environment bindings
    void InitializeLuaBindings();
};
