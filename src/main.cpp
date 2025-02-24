#define SDL_MAIN_HANDLED
#include <chrono>
#include <thread>
#include <iostream>
#include <filesystem>
#include <SDL2/SDL.h>
#include <RegistryManager.hpp>
#include <Object.hpp>
#include <Object2D.hpp>
#include <ComponentManager.hpp>
#include <LuaManager.hpp>
#include <Renderer2D.hpp>
#include <ProjectManager.hpp>

// Function to load and set the window icon
void SetWindowIcon(SDL_Window* window, const std::string& iconPath) {
    SDL_Surface* icon = SDL_LoadBMP(iconPath.c_str());
    if (!icon) {
        std::cerr << "Failed to load window icon: " << SDL_GetError() << std::endl;
        return;
    }
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);
}

int main(int argc, char* argv[]) {
    // Set the working directory to the executable's directory
    std::filesystem::path exe_path = std::filesystem::absolute(argv[0]).parent_path();
    std::filesystem::current_path(exe_path);

    std::cout << "Working directory set to: " << std::filesystem::current_path() << "\n";

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create SDL window
    SDL_Window* window = SDL_CreateWindow("Rogue Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    SetWindowIcon(window, "assets/icon.ico");

    // Create SDL renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    sol::state& lua = LuaManager::GetInstance();
    ProjectManager::GetInstance();
    RegistryManager::GetInstance();

    // Register Lua environment
    RegisterComponents();

    // Create the Renderer instance
    Renderer2D::GetInstance().Initialize(renderer);
    Renderer2D& ecsRenderer = Renderer2D::GetInstance();
    auto root = Object::Create();
    root->SetScript("scripts/main.lua");
    // Define target FPS and frame duration
    constexpr int target_fps = 60;
    constexpr float frame_duration = 1.0f / target_fps;
    
    SDL_Event event;

    // Game loop
    bool running = true;
    while (running) {
        auto frame_start = std::chrono::high_resolution_clock::now();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            root->ProcessInput(event); // Process input events

        }
        // Process all objects, including the root
        root->Process(frame_duration);

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 164, 157, 157, 255);
        SDL_RenderClear(renderer);

        // Render all entities
        ecsRenderer.Render(frame_duration);

        // Present the rendered frame
        SDL_RenderPresent(renderer);

        // Frame limiting: Sleep to maintain 60 FPS
        auto frame_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = frame_end - frame_start;
        if (elapsed.count() < frame_duration) {
            std::this_thread::sleep_for(std::chrono::duration<float>(frame_duration - elapsed.count()));
        }
    }

    std::cout << "Game loop exited. Simulation complete.\n";

    // Clean up SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    // manager.DestroyAllObjects();  // Destroy all objects
    RegistryManager::GetInstance().clear();  // Clear all entities and components
    lua.collect_garbage();  // Explicitly collect garbage to clean up Lua objects
    std::cout << "Cleaned up Registry and Lua.\n";

    return 0;
}
