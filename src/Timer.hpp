// Copyright 2025 RestingImmortal

#pragma once

class Timer {
public:
    Timer() : m_duration(0.0f) {}
    explicit Timer(const float seconds) : m_duration(seconds) {}

    void start();

    void start(float new_duration);

    void update(float delta);

    bool is_done() const;

    void stop();

    bool is_active() const;

    float get_progress() const;

    float get_duration() const;

private:
    float m_duration;
    float m_elapsed = 0;
    bool  m_active = false;
};
