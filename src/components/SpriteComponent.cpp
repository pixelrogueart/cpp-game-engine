#include <SpriteComponent.hpp>

SpriteComponent::SpriteComponent(const std::string& path)
    : texturePath(path), texture(nullptr) {
    InitializeLuaBindings();
}

SpriteComponent::~SpriteComponent() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
    std::cout << "SpriteComponent destroyed for entity ID: " << static_cast<int>(entity) << "\n";
}

void SpriteComponent::LoadTexture(const std::string& path, SDL_Renderer* renderer) {
    if (texturePath != path || !texture) { // Check if the texturePath has changed
        texturePath = path;

        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }

        if (texturePath.empty()) {
            std::cerr << "Texture path is empty. Cannot load texture.\n";
            return;
        }

        SDL_Surface* surface = IMG_Load(texturePath.c_str());
        if (!surface) {
            std::cerr << "Failed to load texture: " << IMG_GetError() << "\n";
            return;
        }

        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture) {
            std::cerr << "Failed to create texture: " << SDL_GetError() << "\n";
            return;
        }

        UpdateFrameCoords();
        std::cout << "Texture loaded successfully from: " << texturePath << "\n";
    }
}

void SpriteComponent::SetFrame(int f) {
    frame = f;
    UpdateFrameCoords();
}

void SpriteComponent::UpdateFrameCoords() {
    if (!texture) return;

    int texture_width, texture_height;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_width, &texture_height);

    int frame_width = texture_width / hframes;
    int frame_height = texture_height / vframes;

    frame_coords.x = (frame % hframes) * frame_width;
    frame_coords.y = (frame / hframes) * frame_height;
    frame_coords.w = frame_width;
    frame_coords.h = frame_height;
}

void SpriteComponent::Render(SDL_Renderer* renderer, int x, int y) {
    if (texturePath.empty()) {
        std::cerr << "No texture path set. Cannot render.\n";
        return;
    }

    if (!texture) {
        LoadTexture(texturePath, renderer);
    }

    if (!texture) {
        std::cerr << "Failed to load texture for rendering.\n";
        return;
    }

    if (currentTexturePath != texturePath) {
        LoadTexture(texturePath, renderer);
        currentTexturePath = texturePath;
    }

    // Center the sprite on the given position
    SDL_Rect dest_rect = {
        x - frame_coords.w / 2, // Adjust x to center horizontally
        y - frame_coords.h / 2, // Adjust y to center vertically
        frame_coords.w,
        frame_coords.h
    };

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (flipped_h) flip = (SDL_RendererFlip)(flip | SDL_FLIP_HORIZONTAL);
    if (flipped_v) flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);

    if (SDL_RenderCopyEx(renderer, texture, &frame_coords, &dest_rect, 0, nullptr, flip) != 0) {
        std::cerr << "Failed to render texture: " << SDL_GetError() << "\n";
    }
}

void SpriteComponent::Emplace(entt::entity owner) {
    owner_entity = owner;

    // Explicitly cast the base pointer to the derived type
    auto self = std::dynamic_pointer_cast<SpriteComponent>(shared_from_this());
    if (!self) {
        throw std::runtime_error("Failed to cast to SpriteComponent");
    }

    // Register the explicitly casted pointer
    RegistryManager::GetInstance().emplace<std::shared_ptr<SpriteComponent>>(owner, self);
}

void SpriteComponent::SetTexturePath(const std::string& path) {
    texturePath = path;
}

void SpriteComponent::Register() {
    sol::state& lua = LuaManager::GetInstance();
    lua.new_usertype<SpriteComponent>("SpriteComponent",
        sol::constructors<SpriteComponent(const std::string&)>(),
        "new", sol::factories([](const std::string& path) {
            auto sprite_instance = std::make_shared<SpriteComponent>(path);
            RegistryManager::GetInstance().emplace<std::shared_ptr<Component>>(sprite_instance->entity, sprite_instance);
            RegistryManager::GetInstance().emplace<std::string>(sprite_instance->entity, "SpriteComponent");
            return sprite_instance->GetEnvironment();
        }),
        "entity", &SpriteComponent::entity,
        sol::base_classes, sol::bases<Component>()
    );
}

void SpriteComponent::InitializeLuaBindings() {
    environment["texture_path"] = std::ref(texturePath);
    environment["hframes"] = std::ref(hframes);
    environment["vframes"] = std::ref(vframes);
    environment["frame"] = std::ref(frame);
    environment["flipped_h"] = std::ref(flipped_h);
    environment["flipped_v"] = std::ref(flipped_v);
    environment["set_texture"] = [this](const std::string& path) {
        SetTexturePath(path);
    };
}
