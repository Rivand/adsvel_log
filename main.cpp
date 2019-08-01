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
    int counter{0};
    while (true) {
        std::this_thread::sleep_for(100ms);
        Logger::Info("Hello kitty {1} {0}", counter++, 666 );
    }
    return 0;
}
