# cpp-logger

A simple C++ logger 0.0.6

## Use with GOTCHA

DFTracer wraps libc I/O functions with GOTCHA.
By default on Linux, cpp-logger finds the real `vsnprintf`, `fputs` and `fflush` when the library loads.
Its output then never reaches the wrappers.

Use cpp-logger as a shared library with DFTracer.
A copy built into the executable loads after DFTracer binds and loses the bypass.
Start DFTracer from `main` in that case.

Set `-DCPP_LOGGER_LIBC_BYPASS=OFF` to call libc directly.

## License

cpp-logger is distributed under the terms of the MIT license.
All new contributions must be made under this license.

See [LICENSE](LICENSE) and [NOTICE](NOTICE) for details.

SPDX-License-Identifier: MIT

LLNL-CODE-2024514 — Applied Storage Research (ASR)
