#include <cpp-logger/clogger.h>
#include <cpp-logger/logger.h>

void cpp_logger_clog(int logger_level, const char *name, const char *string,
                     ...) {
  va_list args;
  va_start(args, string);
  cpplogger::Logger::Instance(name)->vlog(
      static_cast<cpplogger::LoggerType>(logger_level), string, args);
  va_end(args);
}
void cpp_logger_clog_level(const int logger_level, const char *name) {
  cpplogger::Logger::Instance(name)->level =
      static_cast<cpplogger::LoggerType>(logger_level);
}

void cpp_logger_clog_level_file(const int logger_level, const char *name,
                                FILE *file) {
  cpplogger::Logger::Instance(name, file)->level =
      static_cast<cpplogger::LoggerType>(logger_level);
}
