#include "handlers.hpp"
#include <cstring>
#include <cstdint>

namespace isa
{
    uint32_t extract_bits(int l, int r, uint32_t data){
        int width = l - r + 1;
        uint32_t mask = (width >= 32) ? 0xFFFFFFFFu : ((1u << width) - 1u);
        return ((data >> r) & mask);
    }
    void writeRegister(uint32_t result, int rd, uint32_t* registers){
        if(rd == 0) return;
        registers[rd] = result;
    }
    int get_source_register_1   (uint32_t instruction){
        int rs1{static_cast<int>(extract_bits(19, 15, instruction))};
        return rs1;
    }
    int get_source_register_2   (uint32_t instruction){
        int rs2{static_cast<int>(extract_bits(24, 20, instruction))};
        return rs2;
    }
    int get_destination_register(uint32_t instruction){
        int rd{static_cast<int>(extract_bits(11, 7, instruction))};
        return rd;
    }
    uint16_t get_funct_3        (uint32_t instruction){
        uint16_t funct_3 {static_cast<uint16_t> (extract_bits(14, 12, instruction))};
        return funct_3;
    }
    uint16_t get_funct_7        (uint32_t instruction){
        uint16_t funct_7 {static_cast<uint16_t> (extract_bits(31, 25, instruction))};
        return funct_7;
    }

    namespace R
    {
        const uint16_t ADD  {0x0000};
        const uint16_t SUB  {0x0020};
        const uint16_t XOR  {0x0400};
        const uint16_t OR   {0x0600};
        const uint16_t AND  {0x0700};
        const uint16_t SLL  {0x0100};
        const uint16_t SRL  {0x0500};
        const uint16_t SRA  {0x0520};
        const uint16_t SLT  {0x0200};
        const uint16_t SLTU {0x0300};

        void add   (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t rs2_val = registers[rs2];
            writeRegister(rs1_val + rs2_val, rd, registers);
        }
        void sub   (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t rs2_val = registers[rs2];
            writeRegister(rs1_val - rs2_val, rd, registers);
        }
        void xor_  (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t rs2_val = registers[rs2];
            writeRegister(rs1_val ^ rs2_val, rd, registers);
        }
        void or_   (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t rs2_val = registers[rs2];
            writeRegister(rs1_val | rs2_val, rd, registers);
        }
        void and_  (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t rs2_val = registers[rs2];
            writeRegister(rs1_val & rs2_val, rd, registers);
        }
        void sll   (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t shamt = registers[rs2] & 0x1F;
            writeRegister(rs1_val << shamt, rd, registers);
        }
        void srl   (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t shamt = registers[rs2] & 0x1F;
            writeRegister(rs1_val >> shamt, rd, registers);
        }
        void sra   (int rs1, int rs2, int rd, uint32_t* registers){
            int32_t rs1_val = static_cast<int32_t>(registers[rs1]); 
            uint32_t shamt = registers[rs2] & 0x1F;
            writeRegister(static_cast<uint32_t>(rs1_val >> shamt), rd, registers);
        }
        void slt   (int rs1, int rs2, int rd, uint32_t* registers){
            int32_t rs1_val = static_cast<int32_t>(registers[rs1]);
            int32_t rs2_val = static_cast<int32_t>(registers[rs2]);
            writeRegister((rs1_val < rs2_val) ? 1 : 0, rd, registers);
        }
        void sltu  (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t rs2_val = registers[rs2];
            writeRegister((rs1_val < rs2_val) ? 1 : 0, rd, registers);
        }

        void handle_instr(uint32_t instruction, uint32_t* registers){
            int rs1 =        get_source_register_1(instruction);
            int rs2 =        get_source_register_2(instruction);
            int rd =      get_destination_register(instruction);

            uint16_t funct7{get_funct_7(instruction)};
            uint16_t funct3{get_funct_3(instruction)};
            uint16_t code = (funct3 << 8) | funct7;

            switch(code)
            {
            case ADD:
                add (rs1, rs2, rd, registers);
                break;
            case SUB:
                sub (rs1, rs2, rd, registers);
                break;
            case XOR:
                xor_(rs1, rs2, rd, registers);
                break;
            case OR:
                or_ (rs1, rs2, rd, registers);
                break;
            case AND:
                and_(rs1, rs2, rd, registers);
                break;
            case SLL:
                sll (rs1, rs2, rd, registers);
                break;
            case SRL:
                srl (rs1, rs2, rd, registers);
                break;
            case SRA:
                sra (rs1, rs2, rd, registers);
                break;
            case SLT:
                slt (rs1, rs2, rd, registers);
                break;
            case SLTU:
                sltu(rs1, rs2, rd, registers);
                break;
            default:
                break;
            }
        }
    }

    namespace I_IMM
    {
        const uint16_t ADDI  {0x00};
        const uint16_t XORI  {0x04};
        const uint16_t ORI   {0x06};
        const uint16_t ANDI  {0x07};
        const uint16_t SLLI  {0x01};
        const uint16_t SRLI  {0x05};
        const uint16_t SRAI  {0x05};
        const uint16_t SLTI  {0x02};
        const uint16_t SLTIU {0x03};

        uint32_t get_imm(uint32_t instruction){
            uint32_t raw{extract_bits(31, 20, instruction)};
            int32_t imm = static_cast<int32_t>(raw << 20) >> 20;
            return static_cast<uint32_t>(imm);
        }

        void addi  (int rs1,  uint32_t imm, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            writeRegister(rs1_val + imm, rd, registers);
        }
        void xori_ (int rs1,  uint32_t imm, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            writeRegister(rs1_val ^ imm, rd, registers);
        }
        void ori_  (int rs1,  uint32_t imm, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            writeRegister(rs1_val | imm, rd, registers);
        }
        void andi_ (int rs1,  uint32_t imm, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            writeRegister(rs1_val & imm, rd, registers);
        }
        void slli  (int rs1,  uint32_t imm, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t shamt = imm & 0x1F;
            writeRegister(rs1_val << shamt, rd, registers);
        }
        void srli  (int rs1,  uint32_t imm, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t shamt = imm & 0x1F;
            writeRegister(rs1_val >> shamt, rd, registers);
        }
        void srai  (int rs1,  uint32_t imm, int rd, uint32_t* registers){
            int32_t rs1_val = static_cast<int32_t>(registers[rs1]); 
            uint32_t shamt = imm & 0x1F;
            writeRegister(static_cast<uint32_t>(rs1_val >> shamt), rd, registers);
        }
        void slti  (int rs1,  uint32_t imm, int rd, uint32_t* registers){
            int32_t rs1_val = static_cast<int32_t>(registers[rs1]);
            int32_t imm_val = static_cast<int32_t>(imm);
            writeRegister((rs1_val < imm_val) ? 1 : 0, rd, registers);
        }
        void sltiu (int rs1,  uint32_t imm, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            writeRegister((rs1_val < static_cast<uint32_t>(imm)) ? 1 : 0, rd, registers);
        }

        void handle_instr(uint32_t instruction, uint32_t* registers){
            int rs1 =        get_source_register_1(instruction);
            int rd =      get_destination_register(instruction);

            uint32_t imm =                 get_imm(instruction);

            uint16_t funct3{get_funct_3(instruction)};
            int code = funct3;

            switch (code)
            {
            case ADDI:
                addi (rs1, imm, rd, registers);
                break;
            case XORI:
                xori_(rs1, imm, rd, registers);
                break;
            case ORI :
                ori_ (rs1, imm, rd, registers);
                break;
            case ANDI:
                andi_(rs1, imm, rd, registers);
                break;
            case SLLI:
                slli (rs1, imm, rd, registers);
                break;
            case 0x05: // SRLI and SRAI have same funct3 values, they are to be differentiated using imm[5:11]
                {
                    uint32_t imm_5_11 = extract_bits(11, 5, imm);
                
                    switch (imm_5_11)
                    {
                        case (0x00):
                            srli(rs1, imm, rd, registers);
                            break;
                        case (0x20):
                            srai(rs1, imm, rd, registers);
                            break;
                        default:
                            break;
                    }
                }

                break;
            case SLTI:
                slti (rs1, imm, rd, registers);
                break;
            case SLTIU:
                sltiu(rs1, imm, rd, registers);
                break;
            default:
                break;
            }
        }
    }

    namespace I_MEM
    {
        const uint16_t LB  {0x00};
        const uint16_t LH  {0x01};
        const uint16_t LW  {0x02};
        const uint16_t LBU {0x04};
        const uint16_t LHU {0x05};

        uint32_t get_imm(uint32_t instruction){
            uint32_t raw{extract_bits(31, 20, instruction)};
            int32_t imm = static_cast<int32_t>(raw << 20) >> 20;
            return static_cast<uint32_t>(imm);
        }

        void lb    (int rs1, uint32_t imm, int rd, uint32_t* registers, uint8_t* memory){
            uint32_t addr = registers[rs1] + imm;
            int8_t  data;
            memcpy(&data, memory + addr, sizeof(data));
            uint32_t result = static_cast<uint32_t>(static_cast<int32_t>(data));
            writeRegister(result, rd, registers);
        }
        void lh    (int rs1, uint32_t imm, int rd, uint32_t* registers, uint8_t* memory){
            uint32_t addr = registers[rs1] + imm;
            int16_t  data;
            memcpy(&data, memory + addr, sizeof(data));
            uint32_t result = static_cast<uint32_t>(static_cast<int32_t>(data));
            writeRegister(result, rd, registers);
        }
        void lw    (int rs1, uint32_t imm, int rd, uint32_t* registers, uint8_t* memory){
            uint32_t addr = registers[rs1] + imm;
            int32_t  data;
            memcpy(&data, memory + addr, sizeof(data));
            uint32_t result = static_cast<uint32_t>(static_cast<int32_t>(data));
            writeRegister(result, rd, registers);
        }
        void lbu   (int rs1, uint32_t imm, int rd, uint32_t* registers, uint8_t* memory){
            uint32_t addr = registers[rs1] + imm;
            uint8_t  data;
            memcpy(&data, memory + addr, sizeof(data));
            uint32_t result = static_cast<uint32_t>(data);
            writeRegister(result, rd, registers);
        }
        void lhu   (int rs1, uint32_t imm, int rd, uint32_t* registers, uint8_t* memory){
            uint32_t addr = registers[rs1] + imm;
            uint16_t  data;
            memcpy(&data, memory + addr, sizeof(data));
            uint32_t result = static_cast<uint32_t>(data);
            writeRegister(result, rd, registers);
        }

        void handle_instr(uint32_t instruction, uint32_t* registers, uint8_t* memory){
            int rs1      {get_source_register_1(instruction)};
            int rd       {get_destination_register(instruction)};
            uint32_t imm {get_imm(instruction)};

            uint16_t code = get_funct_3(instruction);

            switch (code) {
                case LB :
                    lb (rs1, imm, rd, registers, memory);
                    break;
                case LH :
                    lh (rs1, imm, rd, registers, memory);
                    break;
                case LW :
                    lw (rs1, imm, rd, registers, memory);
                    break;
                case LBU:
                    lbu(rs1, imm, rd, registers, memory);
                    break;
                case LHU:
                    lhu(rs1, imm, rd, registers, memory);
                    break;
            }
        }
    }

    namespace I_JMP
    {
        uint32_t get_imm(uint32_t instruction){
            uint32_t raw{extract_bits(31, 20, instruction)};
            int32_t imm = static_cast<int32_t>(raw << 20) >> 20;
            return static_cast<uint32_t>(imm);
        }

        void jalr   (int rs1, int rd, uint32_t imm, uint32_t* registers, uint32_t& pc){
            writeRegister(pc + 4, rd, registers);
            pc = (registers[rs1] + imm) & ~1u; // spec requires clearing bit 0 of target
                                               // This is mentioned in riscv unpriveleged isa spec, 
                                               // not in the condensed manual
        }

        void handle_instr(uint32_t instruction, uint32_t* registers, uint32_t& pc){
            int rs1      {get_source_register_1(instruction)};
            int rd       {get_destination_register(instruction)};
            uint32_t imm {get_imm(instruction)};

            jalr(rs1, rd, imm, registers, pc);
        }
    }

    namespace S
    {
        const uint16_t SB {0x00};
        const uint16_t SH {0x01};
        const uint16_t SW {0x02};

        uint32_t get_imm(uint32_t instruction){
            uint32_t raw1{extract_bits(31, 25, instruction)};
            uint32_t raw2{extract_bits(11,  7, instruction)};
            int32_t imm = static_cast<int32_t>(((raw1 << 5) | raw2) << 20) >> 20;
            return static_cast<uint32_t>(imm);
        }
        
        void sb    (int rs1, int rs2, uint32_t imm, uint32_t* registers, uint8_t* memory){
            uint32_t addr = registers[rs1] + imm;
            uint8_t  data = static_cast<uint8_t> (registers[rs2]);
            memcpy(memory + addr, &data, sizeof(data));         
        }
        void sh    (int rs1, int rs2, uint32_t imm, uint32_t* registers, uint8_t* memory){
            uint32_t addr = registers[rs1] + imm;
            uint16_t  data = static_cast<uint16_t> (registers[rs2]);
            memcpy(memory + addr, &data, sizeof(data));      
        }
        void sw    (int rs1, int rs2, uint32_t imm, uint32_t* registers, uint8_t* memory){
            uint32_t addr = registers[rs1] + imm;
            uint32_t  data = static_cast<uint32_t> (registers[rs2]);
            memcpy(memory + addr, &data, sizeof(data));      
        }
        void handle_instr(uint32_t instruction, uint32_t* registers, uint8_t* memory){
            int rs1      {get_source_register_1(instruction)};
            int rs2      {get_source_register_2(instruction)};
            uint32_t imm {get_imm(instruction)};

            uint16_t code = get_funct_3(instruction);

            switch (code)
            {
            case SB:
                sb(rs1, rs2, imm, registers, memory);
                break;
            case SH:
                sh(rs1, rs2, imm, registers, memory);
                break;
            case SW:
                sw(rs1, rs2, imm, registers, memory);
                break;
            }   
        }
    }

    namespace B
    {
        const uint16_t BEQ  {0x00};
        const uint16_t BNE  {0x01};
        const uint16_t BLT  {0x04};
        const uint16_t BGE  {0x05};
        const uint16_t BLTU {0x06};
        const uint16_t BGEU {0x07};

        uint32_t get_imm (uint32_t instruction){
            uint32_t bit_12   = extract_bits(31, 31, instruction) << 12;
            uint32_t bit_11   = extract_bits(7,   7, instruction) <<  7;
            uint32_t bit_10_5 = extract_bits(30, 25, instruction) <<  5;
            uint32_t bit_4_1  = extract_bits(11,  8, instruction) <<  1;
            int32_t imm = static_cast<int32_t> ((bit_12 | bit_11 | bit_10_5 | bit_4_1) << 19) >> 19;
            return static_cast<uint32_t> (imm);
        }

        void beq   (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint32_t& pc){
            int32_t rs1_val = static_cast<int32_t> (registers[rs1]);
            int32_t rs2_val = static_cast<int32_t> (registers[rs2]);

            if (rs1_val == rs2_val) {
                pc += imm;
            }
        }
        void bne   (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint32_t& pc){
            int32_t rs1_val = static_cast<int32_t> (registers[rs1]);
            int32_t rs2_val = static_cast<int32_t> (registers[rs2]);

            if (rs1_val != rs2_val) {
                pc += imm;
            }
        }
        void blt   (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint32_t& pc){
            int32_t rs1_val = static_cast<int32_t> (registers[rs1]);
            int32_t rs2_val = static_cast<int32_t> (registers[rs2]);

            if (rs1_val < rs2_val) {
                pc += imm;
            }
        }
        void bge   (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint32_t& pc){
            int32_t rs1_val = static_cast<int32_t> (registers[rs1]);
            int32_t rs2_val = static_cast<int32_t> (registers[rs2]);

            if (rs1_val >= rs2_val) {
                pc += imm;
            }
        }
        void bltu  (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint32_t& pc){
            if (registers[rs1] < registers[rs2]) {
                pc += imm;
            }
        }
        void bgeu  (int rs1, int rs2,  uint32_t imm, uint32_t* registers, uint32_t& pc){
            if (registers[rs1] >= registers[rs2]) {
                pc += imm;
            }
        }

        void handle_instr(uint32_t instruction, uint32_t* registers, uint32_t& pc){
            int rs1 {get_source_register_1(instruction)};
            int rs2 {get_source_register_2(instruction)};
            uint32_t imm {get_imm(instruction)};

            uint16_t code {get_funct_3(instruction)};

            switch (code)
            {
            case BEQ:
                beq(rs1, rs2, imm, registers, pc);
                break;
            case BNE:
                bne(rs1, rs2, imm, registers, pc);
                break;
            case BLT:
                blt(rs1, rs2, imm, registers, pc);
                break;
            case BGE:
                bge(rs1, rs2, imm, registers, pc);
                break;
            case BLTU:
                bltu(rs1, rs2, imm, registers, pc);
                break;
            case BGEU:
                bgeu(rs1, rs2, imm, registers, pc);
                break;
            default:
                break;
            }
        }
    }

    namespace J
    {
        uint32_t get_imm (uint32_t instruction){
            uint32_t bit_20   = extract_bits(31, 31, instruction) << 20;
            uint32_t bit_10_1 = extract_bits(30, 21, instruction) <<  1;
            uint32_t bit_11 = extract_bits(20, 20, instruction) << 11;
            uint32_t bit_19_12  = extract_bits(19, 12, instruction) << 12;

            int32_t imm = static_cast<int32_t> ((bit_20 | bit_19_12 | bit_11 | bit_10_1) << 11) >> 11;
            return static_cast<uint32_t> (imm);
        }

        void jal   (int rd, uint32_t imm, uint32_t *registers, uint32_t& pc){
            writeRegister(pc + 4, rd, registers);  
            pc += imm;          
        }
        void handle_instr(uint32_t instruction, uint32_t* registers, uint32_t& pc){
            int rd {get_destination_register(instruction)};
            uint32_t imm {get_imm(instruction)};

            jal(rd, imm, registers, pc);
        }
    }

    namespace U
    {
        // NOTE : These fields store opcodes, and not funct_3 / funct_7 / both, since 
        //        there are only 2 U type instructions, both with different opcodes
        const uint8_t LUI   {0b0110111};
        const uint8_t AUIPC {0b0010111};

        uint32_t get_imm(uint32_t instruction){
            uint32_t raw {extract_bits(31, 12, instruction)};
            return raw;
        } 
        void lui   (int rd, uint32_t imm, uint32_t* registers){
            writeRegister(imm << 12, rd, registers);
        }
        void auipc (int rd, uint32_t imm, uint32_t* registers, uint32_t pc){
            writeRegister(pc + (imm << 12), rd, registers);
        }

        void handle_instr(uint32_t instruction, uint32_t* registers, uint32_t pc, uint8_t opcode){
            int rd {get_destination_register(instruction)};
            uint32_t imm {get_imm(instruction)};

            switch (opcode)
            {
            case LUI:
                lui(rd, imm, registers);
                break;
            case AUIPC:
                auipc(rd, imm, registers, pc);   
                break;         
            default:
                break;
            }
        }
    }
}
