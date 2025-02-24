#pragma once
#include <cmath>
#include <iostream>
#include <algorithm>
#include <sol/sol.hpp>

struct Vector2 {
    float x, y;

    // Constructors
    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    sol::userdata lua_instance; // Store the Lua instance

    // Friend Operator Overloads
    friend Vector2 operator+(const Vector2& lhs, const Vector2& rhs) {
        return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    friend Vector2 operator-(const Vector2& lhs, const Vector2& rhs) {
        return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    friend Vector2 operator*(const Vector2& lhs, float scalar) {
        return Vector2(lhs.x * scalar, lhs.y * scalar);
    }

    // Friend operator for float * Vector2 (commutative property)
    friend Vector2 operator*(float scalar, const Vector2& rhs) {
        return Vector2(rhs.x * scalar, rhs.y * scalar);
    }

    friend Vector2 operator/(const Vector2& lhs, float scalar) {
        if (scalar == 0) throw std::runtime_error("Division by zero!");
        return Vector2(lhs.x / scalar, lhs.y / scalar);
    }

    friend bool operator==(const Vector2& lhs, const Vector2& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    friend bool operator!=(const Vector2& lhs, const Vector2& rhs) {
        return !(lhs == rhs);
    }

    // Compound Assignment Operators
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2& operator/=(float scalar) {
        if (scalar == 0) throw std::runtime_error("Division by zero!");
        x /= scalar;
        y /= scalar;
        return *this;
    }

    Vector2& operator*=(const Vector2& other) {
        this->x *= other.x;
        this->y *= other.y;
        return *this;
    }

    Vector2 operator*(const Vector2& other) const {
        return Vector2(this->x * other.x, this->y * other.y);
    }

    
    // Static Methods for Vector Math
    static Vector2 Add(const Vector2& lhs, const Vector2& rhs) {
        return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    static Vector2 Subtract(const Vector2& lhs, const Vector2& rhs) {
        return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    static Vector2 Multiply(const Vector2& lhs, float scalar) {
        return Vector2(lhs.x * scalar, lhs.y * scalar);
    }

    static Vector2 Divide(const Vector2& lhs, float scalar) {
        if (scalar == 0) throw std::runtime_error("Division by zero!");
        return Vector2(lhs.x / scalar, lhs.y / scalar);
    }

    static bool Equal(const Vector2& lhs, const Vector2& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    static float Magnitude(const Vector2& vec) {
        return std::sqrt(vec.x * vec.x + vec.y * vec.y);
    }

    static Vector2 Normalize(const Vector2& vec) {
        float mag = Magnitude(vec);
        return mag > 0.0f ? Divide(vec, mag) : Vector2(0, 0);
    }

    static float Dot(const Vector2& lhs, const Vector2& rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }

    static float Cross(const Vector2& lhs, const Vector2& rhs) {
        return lhs.x * rhs.y - lhs.y * rhs.x;
    }

    static float Distance(const Vector2& lhs, const Vector2& rhs) {
        return Magnitude(Subtract(lhs, rhs));
    }

    static Vector2 Reflect(const Vector2& vec, const Vector2& normal) {
        return Subtract(vec, Multiply(normal, 2 * Dot(vec, normal)));
    }

    static Vector2 Project(const Vector2& vec, const Vector2& onto) {
        return Multiply(onto, Dot(vec, onto) / Dot(onto, onto));
    }

    static Vector2 Lerp(const Vector2& start, const Vector2& end, float t) {
        return start + (end - start) * std::clamp(t, 0.0f, 1.0f);
    }

    static Vector2 Rotate(const Vector2& vec, float angle) {
        float rad = angle * (3.14159265359f / 180.0f);
        float s = std::sin(rad);
        float c = std::cos(rad);
        return Vector2(vec.x * c - vec.y * s, vec.x * s + vec.y * c);
    }

    static Vector2 Perpendicular(const Vector2& vec) {
        return Vector2(-vec.y, vec.x);
    }

    static Vector2 Length(const Vector2& vec) {
        return Vector2(std::abs(vec.x), std::abs(vec.y));
    }

    static Vector2 Min(const Vector2& lhs, const Vector2& rhs) {
        return Vector2(std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y));
    }

    static Vector2 Max(const Vector2& lhs, const Vector2& rhs) {
        return Vector2(std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y));
    }

    static Vector2 Abs(const Vector2& vec) {
        return Vector2(std::abs(vec.x), std::abs(vec.y));
    }

    static Vector2 Floor(const Vector2& vec) {
        return Vector2(std::floor(vec.x), std::floor(vec.y));
    }

    static Vector2 Ceil(const Vector2& vec) {
        return Vector2(std::ceil(vec.x), std::ceil(vec.y));
    }

    static Vector2 Round(const Vector2& vec) {
        return Vector2(std::round(vec.x), std::round(vec.y));
    }

    static Vector2 Clamp(const Vector2& vec, const Vector2& min, const Vector2& max) {
        return Vector2(std::clamp(vec.x, min.x, max.x), std::clamp(vec.y, min.y, max.y));
    }

    void set_instance(const sol::userdata& instance) {
        lua_instance = instance;
    }

    sol::userdata get_instance() const {
        return lua_instance;
    }

    // Lua Registration
    static void Register() {
        sol::state& lua = LuaManager::GetInstance();
        lua.new_usertype<Vector2>("Vector2",
            sol::constructors<Vector2(), Vector2(float, float)>(),
            sol::call_constructor, [](sol::this_state ts, float x, float y) {
                sol::state_view lua(ts);
                sol::userdata instance = lua["Vector2"].call<sol::userdata>(x, y);
                auto& obj = instance.as<Vector2>();
                obj.set_instance(instance);
                return obj;
            },
            "x", &Vector2::x,
            "y", &Vector2::y,
            "magnitude", &Vector2::Magnitude,
            "normalized", &Vector2::Normalize,
            "dot", &Vector2::Dot,
            "cross", &Vector2::Cross,
            "distance", &Vector2::Distance,
            "reflect", &Vector2::Reflect,
            "project", &Vector2::Project,
            "lerp", &Vector2::Lerp,
            "rotate", &Vector2::Rotate,
            "perpendicular", &Vector2::Perpendicular,
            "min", &Vector2::Min,
            "max", &Vector2::Max,
            "abs", &Vector2::Abs,
            "floor", &Vector2::Floor,
            "ceil", &Vector2::Ceil,
            "round", &Vector2::Round,
            "clamp", &Vector2::Clamp,
            sol::meta_function::addition, &Vector2::Add,
            sol::meta_function::subtraction, &Vector2::Subtract,
            sol::meta_function::multiplication, &Vector2::Multiply,
            sol::meta_function::division, &Vector2::Divide,
            sol::meta_function::equal_to, &Vector2::Equal
        );
    }
};
