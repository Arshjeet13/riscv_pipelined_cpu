#pragma once
#include <cstdint>

struct IF_ID_Reg
{
    uint32_t instruction {0};
    uint32_t pc           {0};
    bool     valid        {false};
};

struct ID_EX_Reg
{
    uint32_t pc        {0};
    uint32_t rs1_data  {0};
    uint32_t rs2_data  {0};
    uint32_t imm       {0};
    int      rs1       {0};
    int      rs2       {0};
    int      rd        {0};
    uint8_t  opcode    {0};
    uint16_t funct3    {0};
    uint16_t funct7    {0};
    bool     valid     {false};
};

struct EX_MEM_Reg
{
    uint32_t alu_result {0};
    uint32_t rs2_data   {0};
    int      rd         {0};
    uint8_t  opcode     {0};
    uint16_t funct3     {0};
    bool     mem_read   {false};
    bool     mem_write  {false};
    bool     reg_write  {false};
    bool     valid      {false};
};

struct MEM_WB_Reg
{
    uint32_t mem_data   {0};
    uint32_t alu_result {0};
    int      rd         {0};
    bool     reg_write  {false};
    bool     mem_to_reg {false};
    bool     valid      {false};
};
