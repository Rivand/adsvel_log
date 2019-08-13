/**
***************************************************************************************************************************************************************
* @file     file_sink.h
* @author   Kuznetsov A.(RivandBlack).
* @version  v 0.0.4
* @date     01.07.2019 12:39:04
* @brief    file_sink
***************************************************************************************************************************************************************
*/
#pragma once
#include <experimental/filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include "../adsvel_log.h"
namespace adsvel::log {
    using std::string;
    class FileSink : public BaseSink {
       public:
        FileSink(LogLevels in_log_level, const string in_file_name_pattern, size_t in_max_log_file_size_mb, size_t in_amount_of_log_files)
            : file_name_pattern_{in_file_name_pattern}, log_level_{in_log_level}, max_log_file_size_mb_{in_max_log_file_size_mb}, amount_of_log_files_{in_amount_of_log_files}, time_of_last_attempt_open_log_file_(std::chrono::steady_clock::now() - kPeriodBetweenAttemptsOpenLogFile) {
            if (amount_of_log_files_ > kMaxAmountOfLogFile) {
                amount_of_log_files_ = kMaxAmountOfLogFile;
                throw std::length_error("Exceeded the 'in_amount_of_log_files' during FileSink initialization.");
            }
        }
        LogLevels GetLevel() override final { return log_level_; }
        void SetLevel(LogLevels in_level) override final { log_level_ = in_level; }

        void Log(const LogMessage& in_msg) override final {
            if (log_level_ <= in_msg.level) {
                std::time_t time = std::chrono::system_clock::to_time_t(in_msg.time);
                std::tm timetm{};
                localtime_r(&time, &timetm);
                char date_time_format[] = "%Y.%m.%d %H:%M:%S";
                char time_str[] = "yyyy.mm.dd HH:MM:SS.mmm---";
                strftime(time_str, strlen(time_str), date_time_format, &timetm);
                string line{fmt::format("[{0}.{1:03}][{2:6}] {3}\n", time_str, std::chrono::duration_cast<std::chrono::milliseconds>(in_msg.time.time_since_epoch()).count() % 1000, LogLevelsStr.at(static_cast<uint16_t>(in_msg.level)), in_msg.message)};
                TryWriteToLogFile_(line);
            }
        }

       private:
        string MakeLogsFileFullName_(std::size_t in_file_num) {
            return fmt::format(file_name_pattern_, in_file_num);
        }
        void RemoveIrrelevantLogsFile(size_t in_current_file_num) {
            size_t delete_file_num = in_current_file_num - amount_of_log_files_;
            if (delete_file_num < 1) delete_file_num += kMaxNumberOfLogFile;
            std::experimental::filesystem::path logs_file{MakeLogsFileFullName_(delete_file_num)};
            if (exists(logs_file)) remove(logs_file);
        }
        void OpenRelevantLogFile_() {
            uint16_t counter{kMaxNumberOfLogFile};
            logs_file_full_name_ = MakeLogsFileFullName_(counter);
            if (exists(logs_file_full_name_)) {
                counter = kMaxAmountOfLogFile;  // Если есть 9999тый файл логов, то это означает что был пройден круг с 1 до 9999 и нам надо искать файл с середины.
                logs_file_full_name_ = MakeLogsFileFullName_(counter);
            }
            // Ищем существующий файл с логами начиная с номера 9999 (или 5000) и спускаемся вниз пока не найдёт его(или не достигнем counter = 0 что будет означать что актуальных файлов нет).
            while (true) {
                if (exists(logs_file_full_name_)) {  // Нашли подходящий файл.
                    current_size_of_log_file_ = file_size(logs_file_full_name_);
                    if (current_size_of_log_file_ < max_log_file_size_mb_ * 1024 * 1024) {  // Был заполнен этот файл до конца или там еще осталось место.
                        logs_file_stream_.open(logs_file_full_name_.string(), std::ofstream::out | std::ofstream::app);
                        current_file_num_ = counter;
                        return;
                    } else {
                        counter++;
                        if (counter >= kMaxNumberOfLogFile) counter = kFirstNumberOfLogFile;
                        logs_file_full_name_ = MakeLogsFileFullName_(counter);
                        logs_file_stream_.open(logs_file_full_name_.string(), std::ofstream::out | std::ofstream::app);
                        current_size_of_log_file_ = 0;
                        current_file_num_ = counter;
                        return;
                    }
                }
                counter--;
                logs_file_full_name_ = MakeLogsFileFullName_(counter);
                if (counter == 0) {
                    logs_file_full_name_ = MakeLogsFileFullName_(kFirstNumberOfLogFile);
                    logs_file_stream_.open(logs_file_full_name_.string(), std::ofstream::out | std::ofstream::app);
                    current_size_of_log_file_ = 0;
                    current_file_num_ = counter;
                    return;
                }
            }
        }
        void RotateLogFile_() {
            logs_file_stream_.flush();
            logs_file_stream_.close();
            current_file_num_++;
            if (current_file_num_ > kMaxNumberOfLogFile) current_file_num_ = kFirstNumberOfLogFile;
            RemoveIrrelevantLogsFile(current_file_num_);
            logs_file_full_name_ = MakeLogsFileFullName_(current_file_num_);
            std::experimental::filesystem::path logs_file{logs_file_full_name_};
            if (exists(logs_file)) remove(logs_file);
            logs_file_stream_.open(logs_file_full_name_.string(), std::ofstream::out | std::ofstream::app);
            current_size_of_log_file_ = 0;
        }

        void TryWriteToLogFile_(const std::string& in_text) try {
            // Если файл логов открыт то просто записывает туда данные.
            if (logs_file_stream_.is_open()) {
                logs_file_stream_ << in_text << std::flush;
                current_size_of_log_file_ += in_text.size();
                if (current_size_of_log_file_ >= max_log_file_size_mb_ * 1024 * 1024) {
                    RotateLogFile_();
                    current_size_of_log_file_ = 0;
                }
                return;
            } else {
                // Иначе записываем данные в string что бы записать их потом.
                accumulated_logs_to_log_file_.append(in_text);
                // Если размер accumulated_logs_to_log_file_ уже превышен указанный порог то просто выкидываем ранее накопленные логи на помойку.
                if (accumulated_logs_to_log_file_.size() > kMaxSizeOfDelayedWriteToLoggingFile) {
                    accumulated_logs_to_log_file_.clear();
                }
                // Если файл закрыт то проверяем время с момента последней попытки его открытия и если время прошло то пытаемся открыть его снова.
                if (time_of_last_attempt_open_log_file_ + kPeriodBetweenAttemptsOpenLogFile <= std::chrono::steady_clock::now()) {
                    time_of_last_attempt_open_log_file_ = std::chrono::steady_clock::now();
                    OpenRelevantLogFile_();
                    // logs_file_stream_.open(logs_file_full_name_, std::ofstream::out | std::ofstream::app);
                    if (!logs_file_stream_.is_open()) {
                        return;
                    } else {
                        // Если файл с логами открыть наконец удалось, то скидываем туда всё что накопили.
                        logs_file_stream_ << "=========================== START A NEW RECORD ==========================" << std::flush << std::endl;
                        logs_file_stream_ << accumulated_logs_to_log_file_;
                        accumulated_logs_to_log_file_.clear();
                    }
                }
            }
        } catch (...) {  // Этот cathc тут нужен обязательно.
        }
        static constexpr uint16_t kMaxNumberOfLogFile{9999};
        static constexpr uint16_t kMaxAmountOfLogFile{5000};
        static constexpr uint16_t kFirstNumberOfLogFile{1};
        static constexpr std::chrono::duration kPeriodBetweenAttemptsOpenLogFile{std::chrono::seconds(30)};
        static constexpr uint32_t kMaxSizeOfDelayedWriteToLoggingFile{100 * 1024 * 1024};
        const std::string file_name_pattern_;
        std::string message_pattern_{""};
        LogLevels log_level_{LogLevels::Info};
        std::ofstream logs_file_stream_;  ///< Поток файла с логами.
        size_t current_file_num_{0};
        std::experimental::filesystem::path logs_file_full_name_{""};                 ///< Полный путь к файлу с логами, с которым в текущий момент работает логер.
        std::chrono::steady_clock::time_point time_of_last_attempt_open_log_file_{};  ///< Время последней попытки открыть файл с логами.
        /** @brief      Тут накапливаются логи в случаях, когда файл с логами недоступен. Как только этот файл снова удаётся открыть то туда записывается всё
         *              содержимое этого контейнера. Если размер accumulated_logs_to_log_file_ превысит bs_consts::kMaxSizeOfDelayedWriteToLoggingFile то эти
         *              логи будут потеряны навсегда.                                                                                                           */
        inline static std::string accumulated_logs_to_log_file_{};
        size_t max_log_file_size_mb_{0};
        size_t amount_of_log_files_{0};
        size_t current_size_of_log_file_{0};
    };  // namespace adsvel::log

}  // namespace adsvel::log
