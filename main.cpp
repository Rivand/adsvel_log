#include <chrono>
#include <iostream>
#include <string>
#include "adsvel_log/adsvel_log.h"
#include "adsvel_log/sinks/file_sink.h"
#include "adsvel_log/sinks/stdout_sink.h"

using namespace std::chrono_literals;

int main() {
    using adsvel::log::Logger;
    Logger::Initialize();
    std::unique_ptr cout_sink_1 = std::make_unique<adsvel::log::StdoutSink>(adsvel::log::LogLevels::Debug);
    std::unique_ptr cout_sink_2 = std::make_unique<adsvel::log::StdoutSink>(adsvel::log::LogLevels::Error);

    std::unique_ptr file_sink = std::make_unique<adsvel::log::FileSink>(adsvel::log::LogLevels::Trace, "LOG {}.txt", 1, 3);
    Logger::AddSink(std::move(cout_sink_1));
    Logger::AddSink(std::move(cout_sink_2));
    Logger::AddSink(std::move(file_sink));
    int counter{0};
    while (true) {
        std::this_thread::sleep_for(10ms);
        Logger::Debug("Debug");
        Logger::Trace("Trace");
        Logger::Info("123");
        Logger::Error("567");
        Logger::Warning("Hello kitty {1} {0}", counter++, 666);
        Logger::Critical("Critical error {}", true);
    }
    return 0;
}
