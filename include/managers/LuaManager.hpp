#pragma once

#include <sol/sol.hpp>
#include <iostream>

class LuaManager {
public:
    // Retrieve the singleton instance of the Lua state
    static sol::state& GetInstance() {
        static sol::state lua_state; // Initialize the Lua state only once
        static bool initialized = false;
        if (!initialized) {
            Initialize(lua_state);
            initialized = true;
        }
        return lua_state;
    }

    // Deleted constructors to prevent instantiation
    LuaManager() = delete;
    ~LuaManager() = delete;
    LuaManager(const LuaManager&) = delete;
    LuaManager& operator=(const LuaManager&) = delete;

private:
    // Initialize the Lua state
    static void Initialize(sol::state& lua_state) {
        lua_state.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);
        std::cout << "Lua state initialized.\n";
    }
};