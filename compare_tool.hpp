#pragma once
#include "cpu/cpu.hpp"
#include "cpu/pipelined_cpu.hpp"
#include <cstdint>

constexpr uint64_t MEM_SIZE  {4ULL * 1024 * 1024 * 1024}; // 4GB
constexpr int      REG_COUNT {32};

bool compare(CPU& cpu, Memory& memory, PipelinedCPU& p_cpu, Memory& p_memory);