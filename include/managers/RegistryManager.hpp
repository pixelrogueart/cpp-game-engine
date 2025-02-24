#pragma once

#include <entt/entt.hpp>
#include <iostream>

class RegistryManager {
public:
    // Retrieve the singleton instance of the registry
    static entt::registry& GetInstance() {
        static entt::registry instance;
        return instance;
    }

    // Log all entities in the registry
    static void LogAllEntities() {
        auto& instance = GetInstance();

        std::cout << "Logging all entities in the registry:\n";

        // Access all entities in the registry
        const auto& storage = instance.storage<entt::entity>();
        size_t count = 0;

        for (const auto entity : storage) {
            std::cout << "- Entity ID: " << static_cast<uint32_t>(entity) << "\n";
            ++count;
        }

        if (count == 0) {
            std::cout << "No entities in the registry.\n";
        }
    }

    // Deleted constructors to prevent instantiation
    RegistryManager() = delete;
    ~RegistryManager() = delete;
    RegistryManager(const RegistryManager&) = delete;
    RegistryManager& operator=(const RegistryManager&) = delete;
};
