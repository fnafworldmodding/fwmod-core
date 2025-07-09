#ifndef LOGGER_H  
#define LOGGER_H  
#include "common.h"      // DLLCALL macro

#include <string>         // std::string
#include <vector>         // std::vector  
#include <functional>     // std::function  
#include <memory>         // std::unique_ptr, std::make_unique  
#include <fstream>        // std::ofstream  
#include <mutex>          // std::mutex, std::lock_guard  
#include <stdexcept>      // std::runtime_error  
#include <sstream>        // std::ostringstream  
#include <iomanip>        // std::put_time

// TODO: add DLL interface for Logger class
class DLLCALL Logger
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
   // better to use some kind of an class/struct than a factory function, to allow clean up as file handlers never close
   static std::function<void(const std::string&)> CreateFileHandler(const std::string& path);  
   static std::function<void(const std::string&)> CreateConsoleHandler();  
   static std::function<void(const std::string&)> CreateCoreFileHandle(const std::string& filePath = "")  
   {  
       static std::unique_ptr<std::ofstream> fileStream;  
       static std::mutex fileMutex;  

       {  
           std::lock_guard<std::mutex> lock(fileMutex);  
           if (!fileStream)  
           {  
               if (filePath.empty())  
               {  
                   throw std::runtime_error("File path must be provided on the first call to CreateCoreFileHandle.");  
               }  
               fileStream = std::make_unique<std::ofstream>(filePath, std::ios::out | std::ios::app);  
               if (!fileStream->is_open())  
               {  
                   throw std::runtime_error("Failed to open log file: " + filePath);  
               }  
           }  
       }  

       return [](const std::string& message)  
       {  
           std::lock_guard<std::mutex> lock(fileMutex);  
           if (fileStream && fileStream->is_open())  
           {  
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

   std::string datetimeFormat;
private:
   inline std::string FormatLog(LogLevel level);  
   inline void WriteLog(LogLevel level, const std::string& text);  
};  

#endif // LOGGER_H
