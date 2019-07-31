/**
***************************************************************************************************************************************************************
* @file     adsvel_log.cpp
* @author   Kuznetsov A.(RivandBlack).
* @version  v 0.0.0
* @date     ....
* @brief    Adsvel logging library.
***************************************************************************************************************************************************************
*/
#include "adsvel_log.h"

std::vector<std::unique_ptr<adsvel::log::BaseSink>> adsvel::log::Logger::sinks_;
std::vector<adsvel::log::LogMessage> adsvel::log::Logger::messages_;
std::chrono::steady_clock::duration adsvel::log::Logger::log_interval_{std::chrono::milliseconds(500)};
std::thread adsvel::log::Logger::th_{[]() -> void {
    while (true) {
        std::this_thread::sleep_for(log_interval_);
        for (auto& c : messages_) {
            for (auto& sink : sinks_) {
                sink->Log(c);
            }
        }
        messages_.clear();
    };
}};
