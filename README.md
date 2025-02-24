# Rogue Engine

Rogue Engine is a 2D game engine built using C++, SDL2, and Lua. It provides a framework for creating 2D games with an entity-component-system (ECS) architecture, Lua scripting for game logic, and a renderer for rendering game objects.

## Features

- **Entity-Component-System (ECS):** Manage game entities and their components efficiently.
- **Lua Scripting:** Use Lua scripts to define game logic and behaviors.
- **SDL2 Integration:** Utilize SDL2 for window management, rendering, and input handling.
- **2D Renderer:** Render 2D game objects with support for animations and transformations.

## Project Structure

- `assets/`: Contains game assets such as images and icons.
- `build/`: Build output directory.
- `dependencies/`: External libraries and dependencies.
- `include/`: Header files for the engine's components and managers.
- `scripts/`: Lua scripts for game logic.
- `src/`: Source files for the engine's implementation.
- `.vscode/`: Visual Studio Code configuration files.
- `CMakeLists.txt`: CMake build configuration file.
- `icon.rc`: Resource file for the application icon.

## Getting Started

### Prerequisites

- C++17 or later
- CMake 3.10 or later
- SDL2
- Lua

### Building the Project

1. Clone the repository:
    ```sh
    git clone <repository_url>
    ```
2. Create a build directory and navigate to it:
    ```sh
    mkdir build && cd build
    ```
3. Run CMake to configure the project:
    ```sh
    cmake ..
    ```
4. Build the project:
    ```sh
    make
    ```

## Running the Game

After building the project, you can run the game executable:
```sh
./RogueEngine
```

## Usage

### Main Components

- **RegistryManager:** Manages the ECS registry.
- **LuaManager:** Manages the Lua state and scripting environment.
- **Renderer2D:** Handles rendering of 2D game objects.
- **Object:** Represents a game object with components.
- **ComponentManager:** Manages components for game objects.

### Example Code

Here is an example of how to set up and run the game loop in `main.cpp`:
```cpp
int main(int argc, char* argv[]) {
    // Set the working directory to the executable's directory
    std::filesystem::path exe_path = std::filesystem::absolute(argv[0]).parent_path();
    std::filesystem::current_path(exe_path);

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

    // Set window icon
    SetWindowIcon(window, "assets/icon.ico");

    // Create SDL renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize Lua and ECS
    sol::state& lua = LuaManager::GetInstance();
    ProjectManager::GetInstance();
    RegistryManager::GetInstance();
    RegisterComponents();

    // Initialize Renderer
    Renderer2D::GetInstance().Initialize(renderer);
    Renderer2D& ecsRenderer = Renderer2D::GetInstance();
    auto root = Object::Create();
    root->SetScript("scripts/main.lua");

    // Game loop
    bool running = true;
    while (running) {
        // Handle events and update game state
        // Render game objects
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    RegistryManager::GetInstance().clear();
    lua.collect_garbage();

    return 0;
}
```

## Contributing

Contributions are welcome! Please fork the repository and submit pull requests.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgements

- SDL2
- Lua
- EnTT

Feel free to customize this README to better fit your project's specifics and requirements.
