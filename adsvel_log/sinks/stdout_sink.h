/**
***************************************************************************************************************************************************************
* @file     stdout_sink.h
* @author   Kuznetsov A.(RivandBlack).
* @version  v 0.0.0
* @date     ....
* @brief    stdout sink.
* @details  file_sink
***************************************************************************************************************************************************************
*/
#pragma once

#include <atomic>
#include <iostream>
#include "../adsvel_log.h"
namespace adsvel::log {
    class StdoutSink : public BaseSink {
       public:
        StdoutSink(LogLevels in_log_level) : log_level_(in_log_level){};
        void SetLevel(LogLevels in_level) override {}
        void Log(const LogMessage& in_msg) override {
            if (log_level_ <= in_msg.level) {
                std::cout << in_msg.message << std::endl;
            }
        }

       private:
        LogLevels log_level_{LogLevels::Info};
    };

}  // namespace adsvel::log
