//
// Created by hariharan on 8/16/22.
//
#include <cpp-logger/logger.h>

#if !defined(CPP_LOGGER_LIBC_BYPASS)
#if defined(__linux__)
#define CPP_LOGGER_LIBC_BYPASS 1
#else
#define CPP_LOGGER_LIBC_BYPASS 0
#endif
#endif

#if CPP_LOGGER_LIBC_BYPASS
#include <dlfcn.h>
#endif

namespace {
// Calls the real libc functions so GOTCHA wrappers of the same names (e.g.
// DFTracer's STDIO interception) never see logger output, which would
// recurse. They are resolved once when this library loads: GOTCHA wraps
// dlsym, so a lookup after it binds returns its wrapper, not libc.
// With CPP_LOGGER_LIBC_BYPASS off, the logger calls libc directly.
using vsnprintf_fn_t = int (*)(char *, size_t, const char *, va_list);
using fputs_fn_t = int (*)(const char *, FILE *);
using fflush_fn_t = int (*)(FILE *);

// Never take the address of a libc function here. In a non-PIE executable that
// makes its PLT stub the canonical address, and GOTCHA lookups then loop at
// exit.
struct LibcFns {
  vsnprintf_fn_t vsnprintf_fn = nullptr;
  fputs_fn_t fputs_fn = nullptr;
  fflush_fn_t fflush_fn = nullptr;

  LibcFns() {
#if CPP_LOGGER_LIBC_BYPASS
    void *handle = dlopen("libc.so.6", RTLD_LAZY | RTLD_NOLOAD);
    if (handle == nullptr) return;
    auto v = reinterpret_cast<vsnprintf_fn_t>(dlsym(handle, "vsnprintf"));
    auto p = reinterpret_cast<fputs_fn_t>(dlsym(handle, "fputs"));
    auto f = reinterpret_cast<fflush_fn_t>(dlsym(handle, "fflush"));
    if (v != nullptr && p != nullptr && f != nullptr) {
      vsnprintf_fn = v;
      fputs_fn = p;
      fflush_fn = f;
    }
#endif
  }
};

const LibcFns &libc_fns() {
  static const LibcFns fns;
  return fns;
}

const LibcFns &eager_libc_fns = libc_fns();

CPP_LOGGER_PRINTF(3, 0)
int real_vsnprintf(char *buf, size_t size, const char *fmt, va_list args) {
  const LibcFns &fns = libc_fns();
  if (fns.vsnprintf_fn != nullptr)
    return fns.vsnprintf_fn(buf, size, fmt, args);
  return vsnprintf(buf, size, fmt, args);
}

void real_puts_flush(const char *line, FILE *file) {
  const LibcFns &fns = libc_fns();
  if (fns.fputs_fn != nullptr && fns.fflush_fn != nullptr) {
    fns.fputs_fn(line, file);
    fns.fflush_fn(file);
    return;
  }
  fputs(line, file);
  fflush(file);
}
}  // namespace

std::unordered_map<std::string, std::shared_ptr<cpplogger::Logger>>
    cpplogger::Logger::instance_map =
        std::unordered_map<std::string, std::shared_ptr<cpplogger::Logger>>();

namespace cpplogger {
void Logger::log(LoggerType type, const char *string, ...) {
  va_list args;
  va_start(args, string);
  vlog(type, string, args);
  va_end(args);
}

void Logger::vlog(LoggerType type, const char *string, va_list args) {
  if (level < type) return;
  const char *tag = nullptr;
  switch (type) {
    case LoggerType::CPP_LOGGER_PRINT:
      tag = "PRINT";
      break;
    case LoggerType::CPP_LOGGER_TRACE:
      tag = "TRACE";
      break;
    case LoggerType::CPP_LOGGER_DEBUG:
      tag = "DEBUG";
      break;
    case LoggerType::CPP_LOGGER_INFO:
      tag = "INFO";
      break;
    case LoggerType::CPP_LOGGER_WARN:
      tag = "WARN";
      break;
    case LoggerType::CPP_LOGGER_ERROR:
      tag = "ERROR";
      break;
    case LoggerType::CPP_LOGGER_NONE:
      break;
  }
  if (tag == nullptr) return;
  char buffer[4096];
  real_vsnprintf(buffer, sizeof(buffer), string, args);
  const std::string line = "[" + _app_name + " " + tag + "]: " + buffer + "\n";
  real_puts_flush(line.c_str(), _file);
}
}  // namespace cpplogger
