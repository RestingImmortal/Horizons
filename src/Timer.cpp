// Copyright 2025 RestingImmortal

#include "Timer.hpp"

#include <algorithm>

void Timer::start() {
    m_elapsed = 0;
    m_active = true;
}

void Timer::start(float new_duration) {
    m_duration = new_duration;
    start();
}

void Timer::update(float delta) {
    if (!m_active) return;
    m_elapsed += delta;
}

bool Timer::is_done() const {
    return m_active && (m_elapsed >= m_duration);
}

void Timer::stop() {
    m_active = false;
}

bool Timer::is_active() const {
    return m_active;
}

float Timer::get_progress() const {
    if (m_duration <= 0.0f) {
        return m_active ? 1.0f : 0.0f;
    }
    return std::clamp(m_elapsed / m_duration, 0.0f, 1.0f);
}

float Timer::get_duration() const {
    return m_duration;
}
