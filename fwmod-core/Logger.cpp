#include "pch.h"
#include "Logger.h"
#include <sstream>
#include <iomanip>

// Constructor
Logger::Logger() : datetimeFormat("%Y-%m-%d %H:%M:%S") {}


void Logger::AddHandler(std::function<void(const std::string&)> handler) {
    handlers.push_back(handler);
}
void Logger::RemoveHandler(std::function<void(const std::string&)> handler) {  
    // TODO: make this readable and also test
    handlers.erase(std::remove_if(handlers.begin(), handlers.end(),  
       [&handler](const std::function<void(const std::string&)>& existingHandler) {  
           return handler.target_type() == existingHandler.target_type();  
       }),  
       handlers.end());  
}


std::function<void(const std::string&)> Logger::CreateFileHandler(const std::string& path) {
    return [path]() {
        std::mutex fileLock;
        return [path, &fileLock](const std::string& message) {
            std::lock_guard<std::mutex> lock(fileLock);
            std::ofstream file(path, std::ios::app);
            if (file.is_open()) {
                file << message << std::endl;
            }
            };
        }();
}

std::function<void(const std::string&)> Logger::CreateConsoleHandler() {
    return [](const std::string& message) {
        std::cout << message << std::endl;
        };
}

// Logging methods
void Logger::Debug(const std::string& text) {
    WriteLog(LogLevel::DEBUG, text);
}

void Logger::Error(const std::string& text) {
    WriteLog(LogLevel::EERROR, text);
}

void Logger::Fatal(const std::string& text) {
    WriteLog(LogLevel::FATAL, text);
}

void Logger::Info(const std::string& text) {
    WriteLog(LogLevel::INFO, text);
}

void Logger::Trace(const std::string& text) {
    WriteLog(LogLevel::TRACE, text);
}

void Logger::Warning(const std::string& text) {
    WriteLog(LogLevel::WARNING, text);
}

// Private methods
inline std::string Logger::FormatLog(LogLevel level) {
    std::ostringstream oss;
    std::time_t now = std::time(nullptr);
    std::tm localTime;
    if (localtime_s(&localTime, &now) == 0) { // Use localtime_s for thread safety
        oss << std::put_time(&localTime, datetimeFormat.c_str());
    }
    else {
        oss << "[INVALID TIME]";
    }

    switch (level) {
    case LogLevel::TRACE:   oss << " [TRACE] "; break;
    case LogLevel::INFO:    oss << " [INFO]  "; break;
    case LogLevel::DEBUG:   oss << " [DEBUG] "; break;
    case LogLevel::WARNING: oss << " [WARN]  "; break;
    case LogLevel::EERROR:  oss << " [ERROR] "; break;
    case LogLevel::FATAL:   oss << " [FATAL] "; break;
    }
    return oss.str();
}

inline void Logger::WriteLog(LogLevel level, const std::string& text) {
    std::string formattedMessage = FormatLog(level) + text;
    for (const auto& handler : handlers) {
        handler(formattedMessage);
    }
}