API Reference
=============

C++ API (``cpp-logger/logger.h``)
---------------------------------

Namespace ``cpplogger``.

Log levels
^^^^^^^^^^

.. code-block:: cpp

   enum LoggerType {
     CPP_LOGGER_NONE  = 0,
     CPP_LOGGER_PRINT = 1,
     CPP_LOGGER_ERROR = 2,
     CPP_LOGGER_WARN  = 3,
     CPP_LOGGER_INFO  = 4,
     CPP_LOGGER_DEBUG = 5,
     CPP_LOGGER_TRACE = 6,
   };

A message is emitted when its level is <= the logger's configured
``level`` (higher values are more verbose).

Class ``Logger``
^^^^^^^^^^^^^^^^

.. cpp:function:: static std::shared_ptr<Logger> Logger::Instance(std::string app_name = "LOGGER", FILE *file = stdout)

   Return (creating on first use) the named logger instance for
   ``app_name``. Output goes to ``file``.

.. cpp:function:: void Logger::log(LoggerType type, const char *format, ...)

   Log a printf-style formatted message at level ``type``. Messages are
   prefixed ``[<APP> <LEVEL>]:`` and flushed immediately.

.. cpp:member:: LoggerType Logger::level

   Current verbosity threshold (default ``CPP_LOGGER_ERROR``). Assign
   directly to change it.

C API (``cpp-logger/clogger.h``)
--------------------------------

Levels are the macros ``CPP_LOGGER_C_PRINT`` (1) through
``CPP_LOGGER_C_TRACE`` (6), mirroring the C++ enum.

.. c:function:: void cpp_logger_clog(const int logger_level, const char* name, const char* format, ...)

   Log a printf-style message at ``logger_level`` on the named logger.

.. c:function:: void cpp_logger_clog_level(const int logger_level, const char* name)

   Set the verbosity threshold of the named logger.

.. c:function:: void cpp_logger_clog_level_file(const int logger_level, const char* name, FILE* file)

   Set the verbosity threshold and output ``FILE*`` of the named logger.
