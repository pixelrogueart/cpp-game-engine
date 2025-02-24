#pragma once

#include <entt/entt.hpp>
#include <iostream>

class ProjectManager {
public:
    // Retrieve the singleton instance of the registry
    static entt::registry& GetInstance() {
        static entt::registry instance;
        return instance;
    }
    // Deleted constructors to prevent instantiation
    ProjectManager() = delete;
    ~ProjectManager() = delete;
    ProjectManager(const ProjectManager&) = delete;
    ProjectManager& operator=(const ProjectManager&) = delete;
};
