// Copyright 2025 RestingImmortal

#pragma once

#include <atomic>
#include <format>
#include <memory>
#include <mutex>
#include <print>
#include <string_view>
#include <vector>

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Off
};

// Interface for log sinks. Because we might have multiple kinds of em later.
struct ILogSink {
    virtual ~ILogSink() = default;
    virtual void write(std::string_view message) = 0;
};

// Implementation of a LogSink, for printing to the console.
// I don't want to deal with class hierarchy hell, so mark it final.
struct ConsoleSink final : ILogSink {
    void write(std::string_view message) override {
        std::println("{}", message);
    }
};

class Logger {
public:
    // We getting singleton up in here.
    static Logger& get() {
        static Logger instance;
        return instance;
    }

    static LogLevel get_level() noexcept {
        return s_level.load(std::memory_order_relaxed);
    }

    static void set_level(const LogLevel level) noexcept {
        s_level.store(level, std::memory_order_relaxed);
    }

    // Make the log push to a specific sink
    void add_sink(std::unique_ptr<ILogSink> sink) {
        m_sinks.push_back(std::move(sink));
    }

    static std::string_view to_string(const LogLevel level) {
        switch (level) {
            case LogLevel::Trace:    return "TRACE";
            case LogLevel::Debug:    return "DEBUG";
            case LogLevel::Info:     return "INFO";
            case LogLevel::Warning:  return "WARNING";
            case LogLevel::Error:    return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            default: return "OFF";
        }
    }

    static LogLevel from_string(const std::string_view string) {
        if (string == "TRACE")    return LogLevel::Trace;
        if (string == "DEBUG")    return LogLevel::Debug;
        if (string == "INFO")     return LogLevel::Info;
        if (string == "WARNING")  return LogLevel::Warning;
        if (string == "ERROR")    return LogLevel::Error;
        if (string == "CRITICAL") return LogLevel::Critical;
        return LogLevel::Off;
    }

    // Does all the logging. Should be called through the appropriate macro.
    void log(const LogLevel level, std::string_view context, std::string_view message) {
        // Ignore low log levels

        auto level_string = to_string(level);

        auto formatted = std::format("[{}] [{}] {}", level_string, context, message);

        std::scoped_lock lock(m_mutex);

        for (auto& sink : m_sinks) {
            sink->write(formatted);
        }
    }

private:
    Logger() = default;

    inline static std::atomic<LogLevel> s_level{LogLevel::Info}; // Memory black magic, to go along with our macro magic.
    std::vector<std::unique_ptr<ILogSink>> m_sinks;
    std::mutex m_mutex;
};

// Macros, cause why not? Not like we're using modules.
#define H_LOG(lvl, ctx, fmt, ...)                                                          \
    do {                                                                                   \
        if ((lvl) >= Logger::get_level()) {                                                \
            Logger::get().log((lvl), (ctx), std::format((fmt) __VA_OPT__(,) __VA_ARGS__)); \
        }                                                                                  \
    } while (0)

#define H_TRACE(ctx, fmt, ...)    H_LOG(LogLevel::Trace,    ctx, fmt __VA_OPT__(,) __VA_ARGS__)
#define H_DEBUG(ctx, fmt, ...)    H_LOG(LogLevel::Debug,    ctx, fmt __VA_OPT__(,) __VA_ARGS__)
#define H_INFO(ctx, fmt, ...)     H_LOG(LogLevel::Info,     ctx, fmt __VA_OPT__(,) __VA_ARGS__)
#define H_WARNING(ctx, fmt, ...)  H_LOG(LogLevel::Warning,  ctx, fmt __VA_OPT__(,) __VA_ARGS__)
#define H_ERROR(ctx, fmt, ...)    H_LOG(LogLevel::Error,    ctx, fmt __VA_OPT__(,) __VA_ARGS__)
#define H_CRITICAL(ctx, fmt, ...) H_LOG(LogLevel::Critical, ctx, fmt __VA_OPT__(,) __VA_ARGS__)
