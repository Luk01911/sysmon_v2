sysmon_v2
A lightweight, zero-dependency POSIX TCP telemetry server written in C for Linux system monitoring. Designed for low overhead and high performance, sysmon_v2 features a custom thread-safe arena allocator, stack-buffered /proc/meminfo parsing, and multi-threaded socket handling.

Key Features
Custom Arena Allocator: Thread-safe bump-pointer allocator operating on a pre-allocated contiguous memory block, avoiding repeated dynamic heap allocations.

Zero-Heap Kernel Parser: Reads /proc/meminfo directly using stack buffers to eliminate memory allocations during request cycles.

Concurrent TCP Engine: Serves client connections using detached POSIX threads (pthreads) with SO_REUSEADDR socket handling.

Graceful Shutdown: Intercepts SIGINT and SIGTERM signals for clean socket teardown and complete memory pool deallocation.

Sanitizer Ready: Configured Makefile targets for GCC/Clang with AddressSanitizer (-fsanitize=address,undefined).

Request Pipeline
Client Request → POSIX Socket Listener → Worker Thread → Arena Allocator & procfs Parser → JSON Payload Response

Directory Layout
Plaintext
sysmon_v2/
├── include/
│   ├── arena.h
│   ├── server.h
│   └── sysmon.h
├── src/
│   ├── arena.c
│   ├── main.c
│   ├── server.c
│   └── sysmon.c
├── Makefile
└── README.md
Requirements
Linux Kernel 3.10+

C Compiler (gcc or clang)

GNU make

pthread library

Building
Compile release binary:

Bash
make
Compile debug binary with AddressSanitizer:

Bash
make debug
Clean build artifacts:

Bash
make clean
Running
Start the server (defaults to port 8080 if omitted):

Bash
./sysmon [port]
Usage & Output
Query the telemetry endpoint using curl:

Bash
curl -s http://127.0.0.1:8080

Memory Verification
Verify zero memory leaks during execution and shutdown using Valgrind:

Bash
valgrind --leak-check=full --show-leak-kinds=all ./sysmon 8080
License
MIT
