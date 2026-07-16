# Minimum CMake version this file requires -- just a compatibility floor.
cmake_minimum_required(VERSION 3.16)

# Names the project. CXX means "this is a C++ project."
project(riscv_pipelined_cpu CXX)

# Which C++ standard to compile against, and refuse to silently fall back
# to an older one if the compiler doesn't support it.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Turn on extra compiler warnings for every target below.
# Worth having from day one -- catches real bugs (e.g. sign-extension
# mistakes, uninitialized variables) before they become debugging sessions.
add_compile_options(-Wall -Wextra)

# Build the CPU logic as its own library ("cpu_core"), separate from the
# executable. This is what lets the same decode/execute code be reused
# later by both the main emulator binary and a separate test binary,
# without copy-pasting anything.
add_library(cpu_core
    cpu/cpu.cpp
    cpu/handlers.cpp
    cpu/memory.cpp 
    cpu/cache.cpp
)

# Anything that links cpu_core can now write `#include "cpu.hpp"`
# and CMake knows to look inside the cpu/ directory for it.
target_include_directories(cpu_core PUBLIC cpu)

# The actual emulator executable: just main.cpp, linked against cpu_core.
add_executable(riscv_emulator main.cpp)
target_link_libraries(riscv_emulator PRIVATE cpu_core)
