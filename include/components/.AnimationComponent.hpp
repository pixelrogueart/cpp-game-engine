#pragma once
#include "Component.hpp"
#include "SpriteComponent.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

struct AnimationFrame {
    int frame;      // Frame index
    float duration; // Duration in seconds
};

struct Animation {
    std::string name;                   // Animation name
    std::vector<AnimationFrame> frames; // Vector of frames with durations
    bool loop;                          // Whether the animation should loop
};

struct AnimationComponent : public Component {
    SpriteComponent* sprite;                          // Reference to the associated SpriteComponent
    std::unordered_map<std::string, Animation> animations; // Animation map
    std::string current_animation;                   // Current animation name
    float timer = 0.0f;                              // Timer for animation progress
    size_t current_frame_index = 0;                  // Current frame index in the animation

    AnimationComponent(SpriteComponent* sprite_ref, sol::state& lua)
        : Component(lua), sprite(sprite_ref) {
        if (!sprite) {
            throw std::runtime_error("AnimationComponent requires a valid SpriteComponent.");
        }

        // Lua integration
        lua_env["play"] = [this](const std::string& anim_name) {
            Play(anim_name);
        };
        lua_env["add_animation"] = [this](const std::string& name, sol::table frames_table, bool loop) {
            AddAnimation(name, frames_table, loop);
        };
        lua_env["stop"] = [this]() {
            Stop();
        };
    }

    void AddAnimation(const std::string& name, sol::table frames_table, bool loop) {
        Animation animation{name, {}, loop};
        for (auto& [key, value] : frames_table) {
            if (value.get_type() == sol::type::table) {
                sol::table frame_data = value;
                int frame = frame_data["frame"].get<int>();
                float duration = frame_data["duration"].get_or(0.1f);
                animation.frames.push_back({frame, duration});
            }
        }
        animations[name] = animation;
        std::cout << "Added animation: " << name << " with " << animation.frames.size() << " frames.\n";
    }

    void Play(const std::string& name) {
        if (animations.find(name) == animations.end()) {
            std::cerr << "Animation " << name << " not found.\n";
            return;
        }
        current_animation = name;
        current_frame_index = 0;
        timer = 0.0f;
        sprite->set_frame(animations[name].frames[current_frame_index].frame);
        std::cout << "Playing animation: " << name << "\n";
    }

    void Stop() {
        current_animation = "";
        timer = 0.0f;
        current_frame_index = 0;
        std::cout << "Animation stopped.\n";
    }

    void Process(float delta_time) override {
        if (current_animation.empty()) return;

        auto& anim = animations[current_animation];
        timer += delta_time;

        while (timer >= anim.frames[current_frame_index].duration) {
            timer -= anim.frames[current_frame_index].duration;
            current_frame_index++;

            // Handle end of animation
            if (current_frame_index >= anim.frames.size()) {
                if (anim.loop) {
                    current_frame_index = 0;
                } else {
                    Stop();
                    return;
                }
            }
        }

        // Update the sprite's current frame
        sprite->set_frame(anim.frames[current_frame_index].frame);
    }

    void Emplace(entt::entity entity) override {
        Registry::GetInstance().emplace<AnimationComponent>(entity, *this);
    }

    void UpdateRegistry(entt::entity entity) override {
        auto& animation = Registry::GetInstance().get<AnimationComponent>(entity);
        animation.animations = animations;
        animation.current_animation = current_animation;
    }

    static void Register(sol::state& lua) {
        lua.set_function("AnimationComponent", [&lua](SpriteComponent* sprite) -> std::shared_ptr<Component> {
            return std::make_shared<AnimationComponent>(sprite, lua);
        });
    }

    ~AnimationComponent() override {
        std::cout << "AnimationComponent destroyed\n";
    }
};