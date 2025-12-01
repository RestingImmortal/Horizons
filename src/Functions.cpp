#include "Functions.hpp"

HitQuadrant calculate_direction(const Components::Transform& a, const Components::Transform& b) {
    const raylib::Vector2 delta = b.position - a.position;
    const float angle_rad = std::atan2(delta.y, delta.x);
    const float angle_deg = angle_rad * RAD2DEG;

    float relative_angle = angle_deg - a.rotation - 270;

    relative_angle = std::fmod(relative_angle, 360.0f);
    if (relative_angle < 0) {
        relative_angle += 360.0f;
    }

    if (relative_angle <= 45 || relative_angle >= 315) {
        return HitQuadrant::Front;
    }
    if (relative_angle >= 45 && relative_angle <= 135) {
        return HitQuadrant::Right;
    }
    if (relative_angle >= 135 && relative_angle <= 225) {
        return HitQuadrant::Back;
    }
    if (relative_angle >= 225 && relative_angle <= 315) {
        return HitQuadrant::Left;
    }

    return HitQuadrant::Back;
}
