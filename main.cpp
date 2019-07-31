#include <chrono>
#include <iostream>
#include "adsvel_log/adsvel_log.h"
#include "adsvel_log/sinks/stdout_sink.h"
using namespace std::chrono_literals;

int main() {
    using adsvel::log::Logger;
    Logger::Initialize();
    std::unique_ptr<adsvel::log::StdoutSink> cout_sink = std::make_unique<adsvel::log::StdoutSink>(adsvel::log::LogLevels::Info);
    Logger::AddSink(std::move (cout_sink));
    Logger::Log(adsvel::log::LogLevels::Warning, "Hello kitty");
    while (true) {
        std::this_thread::sleep_for(1s);
    }
    return 0;
}
