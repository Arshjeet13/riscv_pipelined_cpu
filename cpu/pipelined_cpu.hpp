#pragma once
#include "cache.hpp"
#include "memory.hpp"
#include "pipeline_registers.hpp"
#include <cstdint>

class PipelinedCPU {
public:
    PipelinedCPU(Memory& memory, DCache& dcache, ICache& icache);

    void run();

private:
    void fetch();
    void decode();
    void execute_stage();
    void memory_stage();
    void writeback();

    bool done() const;

    DCache& dcache;
    ICache& icache;
    uint32_t registers[32] = {};
    uint32_t pc {0};
    uint32_t last_instruction_addr {0};

    IF_ID_Reg  if_id  {};
    ID_EX_Reg  id_ex  {};
    EX_MEM_Reg ex_mem {};
    MEM_WB_Reg mem_wb {};
};
