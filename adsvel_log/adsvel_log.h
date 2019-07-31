/**
***************************************************************************************************************************************************************
* @file     adsvel_log.h
* @author   Kuznetsov A.(RivandBlack).
* @version  v 0.0.0
* @date     ....
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
#include <chrono>
#include <memory>
#include <thread>
#include <vector>
namespace adsvel::log {
    enum class LogLevels : uint8_t { Debug, Info, Warning, Error, Critical };

    class LogMessage {
       public:
        LogMessage(LogLevels in_level, std::string in_message) : message(in_message), time(std::chrono::high_resolution_clock::now()), level(in_level) {}
        std::string message;
        std::chrono::high_resolution_clock::time_point time;
        LogLevels level;
    };

    class BaseSink {
       public:
        virtual void SetLevel(LogLevels in_level) = 0;
        virtual void Log(const LogMessage& in_msg) = 0;
        virtual ~BaseSink() = default;
    };

    class Logger {
       public:
        static void Initialize() { th_.detach(); }
        static void AddSink(std::unique_ptr<BaseSink> in_sink) { sinks_.push_back(std::move(in_sink)); }
        static void SetLogInterval(std::chrono::steady_clock::time_point in_interval);
        template <class... Args>
        static void Log(LogLevels in_level, const char* in_msg, const Args&... in_args) {
            messages_.push_back(LogMessage{in_level, fmt::format(in_msg, in_args...)});
        }

        template <class... Args>
        inline static void Info(const std::string in_msg, const Args&... in_args);
        template <class... Args>
        inline static void Warning(const std::string in_msg, const Args&... in_args);
        template <class... Args>
        inline static void Error(const std::string in_msg, const Args&... in_args);
        template <class... Args>
        inline static void Critical(const std::string in_msg, const Args&... in_args);
        template <class... Args>
        inline static void Debug(const std::string in_msg, const Args&... in_args);

       private:
        static std::vector<LogMessage> messages_;
        static std::vector<std::unique_ptr<BaseSink>> sinks_;
        static std::chrono::steady_clock::duration log_interval_;
        static std::thread th_;
    };
}  // namespace adsvel::log
