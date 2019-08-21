/**
***************************************************************************************************************************************************************
* @file     stdout_sink.h
* @author   Kuznetsov A.(RivandBlack).
* @version  v 0.0.4
* @date     01.07.2019 12:39:04
* @brief    stdout sink.
* @details  file_sink
***************************************************************************************************************************************************************
*/
#pragma once

#include <atomic>
#include <iomanip>
#include <iostream>
#include "../adsvel_log.h"
namespace adsvel::log {
    class StdoutSink : public BaseSink {
       public:
        StdoutSink(LogLevels in_log_level) : log_level_{in_log_level} {}
        LogLevels GetLevel() override final { return log_level_; }
        void SetLevel(LogLevels in_level) override final { log_level_ = in_level; }

        void Log(const LogMessage& in_msg) override {
            if (log_level_ <= in_msg.level) {
                std::time_t time = std::chrono::system_clock::to_time_t(in_msg.time);
                std::tm timetm{};
#ifdef __GNUC__
                localtime_r(&time, &timetm);
#else
                localtime_s(&timetm, &time);
#endif
                char date_time_format[] = "%Y.%m.%d %H:%M:%S";
                char time_str[] = "yyyy.mm.dd HH:MM:SS.mmm---";
                strftime(time_str, strlen(time_str), date_time_format, &timetm);
                std::cout << colors_.at(static_cast<uint16_t>(in_msg.level)) << "[" << time_str << "." << std::setfill('0') << std::setw(3) << std::chrono::duration_cast<std::chrono::milliseconds>(in_msg.time.time_since_epoch()).count() % 1000 << "][" << LogLevelsStr.at(static_cast<uint16_t>(in_msg.level))
                          << "]\x1b[0m " << in_msg.message << std::endl;
            }
        }
        void Flush() override { std::cout << std::flush; }

       private:
        std::string message_pattern_{""};
        constexpr static std::array<std::string_view, static_cast<size_t>(LogLevels::_EnumEndDontUseThis_)> colors_{"\x1b[37m", "\x1b[37m", "\x1b[36m", "\x1b[33m", "\x1b[31m", "\x1b[31m", ""};
        LogLevels log_level_{LogLevels::Info};
    };

}  // namespace adsvel::log
