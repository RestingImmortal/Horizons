#pragma once

#include <entt/entt.hpp>

namespace Events {
    struct Collision {
        entt::entity a;
        entt::entity b;
    };
}
