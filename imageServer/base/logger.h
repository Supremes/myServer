#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <mutex>
#include <iostream>
#include "MutexLock.h"
//struct tm;
namespace logger
{
enum class LogLevel{ DEBUG, INFO, WARNING, ERROR };

class BaseLogger{
        //为了解决在外类中调用纯虚函数所造成的窘境
    class LogStream;
public:
    BaseLogger(){
        std::cout << "BaseLogger" << std::endl;
    }
    //返回缓冲区对象(临时的,保证在行末分号处被销毁),
    //因为生成的是一个临时对象,所以在函数体内会产生一个对象来调用默认构造函数,而ostringstream类中禁止调用该函数,所以继承时需要重写LogStream(const LogStream &mStream);
    LogStream operator()(LogLevel mLevel = LogLevel::INFO);

    struct tm* getLocalTime();
    //供缓冲区对象析构的时候调用
    void endline(LogLevel nLevel, std::string &&message);
    
    virtual void output(const struct tm *time, const char* myLevel, const char *str_message) = 0;
    virtual ~BaseLogger(){
        std::cout << "~BaseLogger" << std::endl;
    }

private:
    struct tm localTime_;
    MutexLock mutex_;
};

class BaseLogger::LogStream: public std::ostringstream{
private:
    BaseLogger &mLogger_;
    LogLevel myLevel_;

public:
    LogStream(BaseLogger &mLogger, LogLevel level_):
            mLogger_(mLogger), myLevel_(level_){
                std::cout << "LogStream1" << std::endl;
            }
    LogStream(const LogStream &mStream): mLogger_(mStream.mLogger_), myLevel_(mStream.myLevel_){}
    ~LogStream()
    {
        //避免了在BaseLogger中调用纯虚函数的情况(引用,运行时抉择调用哪个output函数)
        std::cout << "~LogStream" << std::endl;
        mLogger_.endline(myLevel_, std::move(str()));
    }
};
class ConsoleLogger: public BaseLogger{
public:
    ConsoleLogger(){
        std::cout << "ConsoleLogger" << std::endl;
    }
    void output(const struct tm *time, const char* myLevel, const char *str_message);
    ~ConsoleLogger(){
        std::cout << "~ConsoleLogger" << std::endl;
    }
};
};