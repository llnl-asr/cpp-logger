//
// Created by haridev on 2/14/22.
//

#ifndef CPPLOGGER_LOGGER_H
#define CPPLOGGER_LOGGER_H

#include <cstdarg>
#include <memory>
#include <unordered_map>
#include <string>

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
  FILE *_file;
  static std::unordered_map<std::string, std::shared_ptr<Logger>> instance_map;

 public:
  LoggerType level;

  explicit Logger(std::string app_name, FILE *file = stdout)
      : _app_name(app_name), _file(file), level(LoggerType::CPP_LOGGER_ERROR) {}

  static std::shared_ptr<Logger> Instance(std::string app_name = "LOGGER", FILE *file = stdout) {
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

  void log(LoggerType type, const char *string, ...) {
    va_list args;
    va_start(args, string);
    char buffer[4096];
    int resu = vsprintf(buffer, string, args);
    (void)resu;
    switch (type) {
      case LoggerType::CPP_LOGGER_PRINT: {
        if (level >= LoggerType::CPP_LOGGER_PRINT) {
          fprintf(_file, "[%s PRINT]: %s\n", _app_name.c_str(), buffer);
          fflush(_file);
        }
        break;
      }
      case LoggerType::CPP_LOGGER_TRACE: {
        if (level >= LoggerType::CPP_LOGGER_TRACE) {
          fprintf(_file, "[%s TRACE]: %s\n", _app_name.c_str(), buffer);
          fflush(_file);
        }
        break;
      }
      case LoggerType::CPP_LOGGER_DEBUG: {
        if (level >= LoggerType::CPP_LOGGER_DEBUG) {
          fprintf(_file, "[%s DEBUG]: %s\n", _app_name.c_str(), buffer);
          fflush(_file);
        }
        break;
      }
      case LoggerType::CPP_LOGGER_INFO: {
        if (level >= LoggerType::CPP_LOGGER_INFO) {
          fprintf(_file, "[%s INFO]: %s\n", _app_name.c_str(), buffer);
          fflush(_file);
        }
        break;
      }
      case LoggerType::CPP_LOGGER_WARN: {
        if (level >= LoggerType::CPP_LOGGER_WARN) {
          fprintf(_file, "[%s WARN]: %s\n", _app_name.c_str(), buffer);
          fflush(_file);
        }
        break;
      }
      case LoggerType::CPP_LOGGER_ERROR: {
        if (level >= LoggerType::CPP_LOGGER_ERROR) {
          fprintf(_file, "[%s ERROR]: %s\n", _app_name.c_str(), buffer);
          fflush(_file);
        }
        break;
      }
      default: {
          break;
      }
    }

    va_end(args);
  }
};

}  // namespace cpplogger

#endif  // CPPLOGGER_LOGGER_H
