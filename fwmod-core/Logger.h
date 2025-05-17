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
    static std::function<void(const std::string&)> CreateCoreFileHandle(const std::string& filePath = "") {
        static std::unique_ptr<std::ofstream> fileStream;
        static std::mutex fileMutex;

        // Initialize the file stream if it hasn't been created yet
        {
            std::lock_guard<std::mutex> lock(fileMutex);
            if (!fileStream) {
                if (filePath.empty()) {
                    throw std::runtime_error("File path must be provided on the first call to CreateCoreFileHandle.");
                }
                fileStream = std::make_unique<std::ofstream>(filePath, std::ios::out | std::ios::app);
                if (!fileStream->is_open()) {
                    throw std::runtime_error("Failed to open log file: " + filePath);
                }
            }
        }

        // Return a lambda function that writes to the shared file stream
        return [](const std::string& message) {
            std::lock_guard<std::mutex> lock(fileMutex);
            if (fileStream && fileStream->is_open()) {
                *fileStream << message << std::endl;
            }
            };
    }

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
