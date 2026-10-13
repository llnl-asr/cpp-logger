#include <cpp-logger/logger.h>

#include <cstdio>

int main(int argc, char** argv) {
  if (argc < 2) return 2;
  FILE* file = fopen(argv[1], "w");
  if (file == nullptr) return 2;
  auto logger = cpplogger::Logger::Instance("PROBE", file);
  logger->level = cpplogger::CPP_LOGGER_INFO;
  for (int i = 0; i < 20; i++) {
    logger->log(cpplogger::CPP_LOGGER_INFO, "line %d", i);
  }
  fclose(file);
  return 0;
}
