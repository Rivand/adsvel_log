/**
***************************************************************************************************************************************************************
* @file     adsvel_log.cpp
* @author   Kuznetsov A.(RivandBlack).
* @version  v 0.0.4 
* @date     01.07.2019 12:39:04
* @brief    Adsvel logging library.
***************************************************************************************************************************************************************
*/
#include "adsvel_log.h"

std::vector<std::unique_ptr<adsvel::log::BaseSink>> adsvel::log::Logger::sinks_{};
std::vector<adsvel::log::LogMessage> adsvel::log::Logger::messages_{};
std::chrono::steady_clock::duration adsvel::log::Logger::log_interval_{std::chrono::milliseconds(500)};
std::atomic<adsvel::log::LogLevels> adsvel::log::Logger::log_level_ {adsvel::log::LogLevels::Off};
std::mutex adsvel::log::Logger::mut_{};
std::thread* adsvel::log::Logger::th_{nullptr};


