/**
***************************************************************************************************************************************************************
* @file     adsvel_log.h
* @author   Kuznetsov A.(RivandBlack).
* @version  v 0.0.5
* @date     05.07.2019 14:12:10
* @brief    Adsvel logging library.
* @details  I – info, W – warning, E – error, C – critical, D – debug.
*
*           |                          | (in cout - cout on)| (in cout - cout off)| (in file)|
*           | ------------------------ | ------------------ | ------------------- | -------- |
*           | disabled                 |       LS           |       S             |    -     |
*           | only_important_anomalies |       LSI          |       S             |    SI    |
*           | all_anomalies            |       LSIA         |       -             |    SIA   |
*           | detailed_logging         |       LSIAG        |       -             |    SIAL  |
*           | --------------------------------------------------------------------------------
***************************************************************************************************************************************************************
*/
#pragma once
#include <fmt/format.h>
#include <array>
#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
namespace adsvel::log {
    enum class LogLevels : uint8_t { Debug, Trace, Info, Warning, Error, Critical, Off, _EnumEndDontUseThis_ };
    const std::array<std::string_view, static_cast<int>(LogLevels::_EnumEndDontUseThis_)> LogLevelsStr{"Debug", "Trace", "Info", "Warnng", "Error", "Critic", "Off"};

    class LogMessage {
       public:
        LogMessage(LogLevels in_level, std::string in_message) : message(in_message), time(std::chrono::system_clock::now()), level(in_level) {}
        std::string message;
        std::chrono::system_clock::time_point time;
        LogLevels level;
    };

    class BaseSink {
       public:
        virtual LogLevels GetLevel() = 0;
        virtual void SetLevel(LogLevels in_level) = 0;
        virtual void Log(const LogMessage& in_msg) = 0;
        virtual void Flush() = 0;
        virtual ~BaseSink() = default;
    };

    class Logger {
       public:
        static void Initialize() {
            std::lock_guard lock(mut_);
            if (th_ == nullptr) {
                th_ = new std::thread{[]() -> void {
                    while (true) {
                        std::this_thread::sleep_for(log_interval_);
                        {
                            std::lock_guard lock(mut_);
                            for (auto& c : messages_) {  // First the messages then sinks. It's faster from the point of view of the cache.
                                for (auto& sink : sinks_) {
                                    sink->Log(c);
                                }
                            }
                            for (auto& sink : sinks_) {
                                sink->Flush();
                            }
                            messages_.clear();
                        }
                    };
                }};
                th_->detach();
            }
        }
        static void AddSink(std::unique_ptr<BaseSink> in_sink) {
            std::lock_guard lock(mut_);
            if (in_sink->GetLevel() < log_level_) log_level_ = in_sink->GetLevel();
            sinks_.push_back(std::move(in_sink));
        }
        static void SetLogInterval(std::chrono::steady_clock::duration in_interval) {
            std::lock_guard lock(mut_);
            log_interval_ = in_interval;
        }

        static void Flush() {
            std::lock_guard lock(mut_);
            for (auto& sink : sinks_) {
                sink->Flush();
            }
        }

        template <class... Args>
        static void Log(LogLevels in_level, const std::string_view& in_msg, const Args&... in_args) {
            if (log_level_.load(std::memory_order::memory_order_relaxed) > in_level) return;
            std::lock_guard lock(mut_);
            messages_.push_back(LogMessage{in_level, fmt::format(in_msg, in_args...)});
        }

        template <class... Args>
        inline static void Debug(const std::string_view& in_msg, const Args&... in_args) {
            Log(LogLevels::Debug, in_msg, in_args...);
        }
        template <class... Args>
        inline static void Trace(const std::string_view& in_msg, const Args&... in_args) {
            Log(LogLevels::Trace, in_msg, in_args...);
        }
        template <class... Args>
        inline static void Info(const std::string_view& in_msg, const Args&... in_args) {
            Log(LogLevels::Info, in_msg, in_args...);
        }
        template <class... Args>
        inline static void Warning(const std::string_view& in_msg, const Args&... in_args) {
            Log(LogLevels::Warning, in_msg, in_args...);
        }
        template <class... Args>
        inline static void Error(const std::string_view& in_msg, const Args&... in_args) {
            Log(LogLevels::Error, in_msg, in_args...);
        }
        template <class... Args>
        inline static void Critical(const std::string_view& in_msg, const Args&... in_args) {
            Log(LogLevels::Critical, in_msg, in_args...);
        }

       private:
        static std::vector<LogMessage> messages_;
        static std::vector<std::unique_ptr<BaseSink>> sinks_;
        static std::chrono::steady_clock::duration log_interval_;
        static std::mutex mut_;
        static std::atomic<LogLevels> log_level_;  // Maximum logging level of the logger's sinks.
        static std::thread* th_;
    };
}  // namespace adsvel::log
