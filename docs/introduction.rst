Introduction
============

``cpp-logger`` is a lightweight logging library providing a C++ singleton
logger and a plain C interface. It supports per-application named loggers,
six log levels, and printf-style formatting, writing to ``stdout`` or a
user-supplied ``FILE*``.

Features
--------

- Named logger instances (one per application/component) via
  ``cpplogger::Logger::Instance("APP")``.
- Log levels: ``PRINT``, ``ERROR``, ``WARN``, ``INFO``, ``DEBUG``, ``TRACE``.
- printf-style message formatting.
- C API (``clogger.h``) for use from C code.
- Built as a shared library ``libcpp-logger`` with CMake.

Building
--------

.. code-block:: bash

   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build -j
   cmake --install build --prefix <install-dir>

Quick start (C++)
-----------------

.. code-block:: cpp

   #include <cpp-logger/logger.h>

   auto logger = cpplogger::Logger::Instance("MYAPP");
   logger->level = cpplogger::CPP_LOGGER_INFO;
   logger->log(cpplogger::CPP_LOGGER_INFO, "hello %s", "world");

Quick start (C)
---------------

.. code-block:: c

   #include <cpp-logger/clogger.h>

   cpp_logger_clog_level(CPP_LOGGER_C_INFO, "MYAPP");
   cpp_logger_clog(CPP_LOGGER_C_INFO, "MYAPP", "hello %s", "world");
