#include "logger.h"
#include <chrono>
#include <ctime>
#include <map>
#include <iomanip>

using namespace logger;
static const std::map<LogLevel, const char*> mapLog = {
    {LogLevel::DEBUG, "DEBUG"},
    {LogLevel::INFO, "INFO"},
    {LogLevel::ERROR, "ERROR"},
    {LogLevel::WARNING, "WARNING"}
};
//缓冲区重载 operator<< 接收文本信息并暂时保存
std::ostream& operator<<(std::ostream& stream, const struct tm *time){
    return stream << 1900 + time->tm_year << '-'
        << std::setfill('0') << std::setw(2) << time->tm_mon + 1 << '-'
        << std::setfill('0') << std::setw(2) << time->tm_mday << ' '
        << std::setfill('0') << std::setw(2) << time->tm_hour << ':'
        << std::setfill('0') << std::setw(2) << time->tm_min << ':'
        << std::setfill('0') << std::setw(2) << time->tm_sec;
}

BaseLogger::LogStream BaseLogger::operator()(LogLevel mLevel){
    //默认是LogLevel::info
    return LogStream(*this, mLevel);
}
struct tm* BaseLogger::getLocalTime(){
    auto now = std::chrono::system_clock::now();
    auto in_time_t  = std::chrono::system_clock::to_time_t(now);
    localtime_r(&in_time_t, &localTime_);
    return &localTime_;
}

void BaseLogger::endline(LogLevel nLevel, std::string &&message){
    MutexLock lock(mutex_);
    output(getLocalTime(), mapLog.find(nLevel)->second, message.c_str());
}

void ConsoleLogger::output(const struct tm *time, const char* myLevel, const char *str_message){
    std::cout << "[" << time << "]\t" << "level = " << myLevel << "\tmessage = " << str_message << std::endl;
}
