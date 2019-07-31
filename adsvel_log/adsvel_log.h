/**
***************************************************************************************************************************************************************
* @file     adsvel_log.h
* @author   Kuznetsov A.(RivandBlack).
* @version  v 0.0.0
* @date     ....
* @brief    Adsvel logging library.
* @details  Правила логирования и вывода сообщений на экран:
*           I – info, W – warning, E – error, C – critical, D – debug.
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
#include <memory>
#include <vector>
namespace adsvel::log {
    enum class LogLevels { Info, Warning, Error, Critical, Debug };
    class BaseSink {
       public:
        virtual void SetLevel(LogLevels in_level) = 0;
    };

    class Logger {
       public:
        static void AddSink(std::shared_ptr<BaseSink> in_sink);
        template <class ... T>
        inline static void Info(const std::string in_msg, const T&... in_args);
        template <class ... T>
        inline static void Warning(const std::string in_msg, const T&... in_args);
        template <class ... T>
        inline static void Error(const std::string in_msg, const T&... in_args);
        template <class ... T>
        inline static void Critical(const std::string in_msg, const T&... in_args);
        template <class ... T>
        inline static void Debug(const std::string in_msg, const T&... in_args);

       private:
        static std::vector<std::shared_ptr<BaseSink>> sinks_;
    };

}  // namespace adsvel::log
