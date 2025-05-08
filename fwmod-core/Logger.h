#pragma once
#ifndef LOGGER_H
#define LOGGER_H  

#include <string>  
#include <vector>  
#include <functional>  
#include <mutex>  
#include <fstream>  
#include <iostream>  
#include <ctime>  

class Logger
{
public:
    std::vector<std::function<void(const std::string&)>> handlers;

    enum class LogLevel
    {
        TRACE,
        INFO,
        DEBUG,
        WARNING,
        EERROR,
        FATAL
    };

    Logger();
    void AddHandler(std::function<void(const std::string&)> handler);
    void RemoveHandler(std::function<void(const std::string&)> handler);
    static std::function<void(const std::string&)> CreateFileHandler(const std::string& path);
    static std::function<void(const std::string&)> CreateConsoleHandler();

    void Debug(const std::string& text);
    void Error(const std::string& text);
    void Fatal(const std::string& text);
    void Info(const std::string& text);
    void Trace(const std::string& text);
    void Warning(const std::string& text);

private:
    std::string datetimeFormat;
    std::mutex fileLock;

    //void WriteLine(const std::string& text);
    inline std::string FormatLog(LogLevel level);
    inline void WriteLog(LogLevel level, const std::string& text);
};

#endif // LOGGER_H
