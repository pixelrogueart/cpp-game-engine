#include <Renderer2D.hpp>

Renderer2D& Renderer2D::GetInstance() {
    static Renderer2D instance;
    return instance;
}

void Renderer2D::Initialize(SDL_Renderer* sdlRenderer) {
    if (!renderer) {
        renderer = sdlRenderer;
    } else {
        std::cerr << "Renderer is already initialized.\n";
    }
}

void Renderer2D::Render(float frame_duration) {
    if (!renderer) {
        std::cerr << "Renderer is not initialized.\n";
        return;
    }

    auto& registry = RegistryManager::GetInstance();

    // Camera variables
    Vector2 camera_position(0, 0);
    Vector2 target_camera_position(0, 0); // Target position for the camera
    float camera_zoom = 1.0f;
    bool centered = true;
    float smooth = 0.0f;

    // Find the active camera
    auto cameraView = registry.view<std::shared_ptr<Object>, std::string, std::shared_ptr<CameraComponent>>();
    for (auto entity : cameraView) {
        auto& obj = cameraView.get<std::shared_ptr<Object>>(entity);
        auto& type = cameraView.get<std::string>(entity);
        auto& camera = cameraView.get<std::shared_ptr<CameraComponent>>(entity);

        if (type == "Object2D" && camera->current) {
            auto obj2D = std::dynamic_pointer_cast<Object2D>(obj);
            if (obj2D) {
                target_camera_position = obj2D->GetGlobalPosition();
                camera_zoom = camera->GetZoom();
                centered = camera->GetCentered();
                smooth = camera->GetSmooth();
            }
            break;
        }
    }

    // Smooth camera transition
    if (smooth > 0.0f) {
        float adjusted_smooth = std::clamp(smooth * frame_duration, 0.01f, 1.0f);
        camera_position = Vector2::Lerp(last_camera_position, target_camera_position, adjusted_smooth);

        // Update last position only if we're still moving
        if (Vector2::Magnitude(camera_position - target_camera_position) > 0.01f) {
            last_camera_position = camera_position;
        } else {
            last_camera_position = target_camera_position; // Snap to target when close enough
        }
    } else {
        camera_position = target_camera_position;
        last_camera_position = target_camera_position;
    }

    // Adjust for centering
    if (centered) {
        SDL_Rect viewport;
        SDL_RenderGetViewport(renderer, &viewport);
        camera_position.x -= viewport.w / (2 * camera_zoom);
        camera_position.y -= viewport.h / (2 * camera_zoom);
    }

    // Render objects with SpriteComponent, adjusted for the camera
    auto objectView = registry.view<std::shared_ptr<Object>, std::string, std::shared_ptr<SpriteComponent>>();
    for (auto entity : objectView) {
        auto& obj = objectView.get<std::shared_ptr<Object>>(entity);
        auto& type = objectView.get<std::string>(entity);
        auto& sprite = objectView.get<std::shared_ptr<SpriteComponent>>(entity);

        if (type == "Object2D") {
            auto obj2D = std::dynamic_pointer_cast<Object2D>(obj);
            if (obj2D) {
                Vector2 obj_position = obj2D->GetGlobalPosition();

                // Adjust position based on the camera and zoom
                Vector2 render_position = (obj_position - camera_position) * camera_zoom;

                // Delegate rendering to SpriteComponent
                sprite->Render(renderer, static_cast<int>(render_position.x), static_cast<int>(render_position.y));
            }
        }
    }
}

SDL_Renderer* Renderer2D::GetSDLRenderer() {
    return renderer;
}
