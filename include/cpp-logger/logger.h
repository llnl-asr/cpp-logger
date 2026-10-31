//
// Created by haridev on 2/14/22.
//

#ifndef CPPLOGGER_LOGGER_H
#define CPPLOGGER_LOGGER_H

#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>
#include <unordered_map>

#if defined(__GNUC__)
#define CPP_LOGGER_PRINTF(fmt, args) __attribute__((format(printf, fmt, args)))
#else
#define CPP_LOGGER_PRINTF(fmt, args)
#endif

namespace cpplogger {
enum LoggerType {
  CPP_LOGGER_NONE = 0,
  CPP_LOGGER_PRINT = 1,
  CPP_LOGGER_ERROR = 2,
  CPP_LOGGER_WARN = 3,
  CPP_LOGGER_INFO = 4,
  CPP_LOGGER_DEBUG = 5,
  CPP_LOGGER_TRACE = 6,
};

class Logger {
 private:
  std::string _app_name;
  FILE* _file;
  static std::unordered_map<std::string, std::shared_ptr<Logger>> instance_map;

 public:
  LoggerType level;

  explicit Logger(std::string app_name, FILE* file = stdout)
      : _app_name(app_name), _file(file), level(LoggerType::CPP_LOGGER_ERROR) {}

  static std::shared_ptr<Logger> Instance(std::string app_name = "LOGGER",
                                          FILE* file = stdout) {
    auto iter = instance_map.find(app_name);
    std::shared_ptr<Logger> instance;
    if (iter == instance_map.end()) {
      instance = std::make_shared<Logger>(app_name, file);
      instance_map.emplace(app_name, instance);
    } else {
      instance = iter->second;
    }
    return instance;
  }

  void log(LoggerType type, const char* string, ...) CPP_LOGGER_PRINTF(3, 4);
  void vlog(LoggerType type, const char* string, va_list args)
      CPP_LOGGER_PRINTF(3, 0);
};

}  // namespace cpplogger

#endif  // CPPLOGGER_LOGGER_H
