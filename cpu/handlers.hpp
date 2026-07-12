#pragma once
#include <cstdint>
#include <utility>

namespace isa
{
    const uint8_t op_R     {0b0110011};
    const uint8_t op_I_IMM {0b0010011};
    const uint8_t op_I_MEM {0b0000011};
    const uint8_t op_I_JMP {0b1100111};
    const uint8_t op_S     {0b0100011};
    const uint8_t op_B     {0b1100011};
    const uint8_t op_J     {0b1101111};
    const uint8_t op_U1    {0b0110111};
    const uint8_t op_U2    {0b0010111};

    uint32_t extract_bits(int l, int r, uint32_t data);
    void writeRegister(uint32_t result, int rd, uint32_t* registers);
    uint8_t get_opcode(uint32_t instruction);
    int get_source_register_1   (uint32_t instruction);
    int get_source_register_2   (uint32_t instruction);
    int get_destination_register(uint32_t instruction);
    uint16_t get_funct_3        (uint32_t instruction);
    uint16_t get_funct_7        (uint32_t instruction);

    namespace R
    {
        void add   (int rs1, int rs2, int rd, uint32_t* registers);
        void sub   (int rs1, int rs2, int rd, uint32_t* registers);
        void xor_  (int rs1, int rs2, int rd, uint32_t* registers);
        void or_   (int rs1, int rs2, int rd, uint32_t* registers);
        void and_  (int rs1, int rs2, int rd, uint32_t* registers);
        void sll   (int rs1, int rs2, int rd, uint32_t* registers);
        void srl   (int rs1, int rs2, int rd, uint32_t* registers);
        void sra   (int rs1, int rs2, int rd, uint32_t* registers);
        void slt   (int rs1, int rs2, int rd, uint32_t* registers);
        void sltu  (int rs1, int rs2, int rd, uint32_t* registers);

        void handle_instr(uint32_t instruction, uint32_t* registers);
    }

    namespace I_IMM
    {
        uint32_t get_imm (uint32_t instruction);
        void addi  (int rs1, uint32_t imm, int rd, uint32_t* registers);
        void xori_ (int rs1, uint32_t imm, int rd, uint32_t* registers);
        void ori_  (int rs1, uint32_t imm, int rd, uint32_t* registers);
        void andi_ (int rs1, uint32_t imm, int rd, uint32_t* registers);
        void slli  (int rs1, uint32_t imm, int rd, uint32_t* registers);
        void srli  (int rs1, uint32_t imm, int rd, uint32_t* registers);
        void srai  (int rs1, uint32_t imm, int rd, uint32_t* registers);
        void slti  (int rs1, uint32_t imm, int rd, uint32_t* registers);
        void sltiu (int rs1, uint32_t imm, int rd, uint32_t* registers);
        void handle_instr   (uint32_t instruction, uint32_t* registers);
    }

    namespace I_MEM
    {
        uint32_t get_imm (uint32_t instruction);
        void lb    (int rs1, uint32_t  imm, int rd, uint32_t* registers, uint8_t* memory);
        void lh    (int rs1, uint32_t  imm, int rd, uint32_t* registers, uint8_t* memory);
        void lw    (int rs1, uint32_t  imm, int rd, uint32_t* registers, uint8_t* memory);
        void lbu   (int rs1, uint32_t  imm, int rd, uint32_t* registers, uint8_t* memory);
        void lhu   (int rs1, uint32_t  imm, int rd, uint32_t* registers, uint8_t* memory);
        void handle_instr    (uint32_t instruction, uint32_t* registers, uint8_t* memory);
    }

    namespace I_JMP
    {
        uint32_t get_imm(uint32_t instruction);
        void jalr   (int rs1, int rd, uint32_t imm, uint32_t* registers, uint32_t& pc);
        void handle_instr(uint32_t instruction, uint32_t* registers, uint32_t& pc);
    }

    namespace S
    {
        uint32_t get_imm (uint32_t instruction);
        void sb    (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint8_t* memory);
        void sh    (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint8_t* memory);
        void sw    (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint8_t* memory);
        void handle_instr     (uint32_t instruction, uint32_t* registers, uint8_t* memory);
    }

    namespace B
    {
        uint32_t get_imm (uint32_t instruction);
        void beq   (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint32_t& pc);
        void bne   (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint32_t& pc);
        void blt   (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint32_t& pc);
        void bge   (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint32_t& pc);
        void bltu  (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint32_t& pc);
        void bgeu  (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint32_t& pc);
        void handle_instr     (uint32_t instruction, uint32_t* registers, uint32_t&  pc);
    }

    namespace J
    {
        uint32_t get_imm(uint32_t instruction);
        void jal   (int rd, uint32_t imm, uint32_t* registers, uint32_t& pc);
        void handle_instr(uint32_t instruction, uint32_t* registers, uint32_t& pc);
    }

    namespace U
    {
        uint32_t get_imm(uint32_t instruction);        
        void lui   (int rd, uint32_t imm, uint32_t* registers);
        void auipc (int rd, uint32_t imm, uint32_t* registers, uint32_t pc);
        void handle_instr(uint32_t instruction, uint32_t* registers, uint32_t pc, uint8_t opcode);
    }
}