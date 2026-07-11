#include "handlers.hpp"

namespace isa
{
    uint32_t extract_bits(int l, int r, uint32_t instruction){
        int width = l - r + 1;
        uint32_t mask = (width >= 32) ? 0xFFFFFFFFu : ((1u << width) - 1u);
        return ((instruction >> r) & mask);
    }

    void writeRegister(uint32_t result, int rd, uint32_t* registers){
        if(rd == 0) return;
        registers[rd] = result;
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

        std::pair<int,int> get_source_registers(uint32_t instruction){
            int rs1{static_cast<int>(extract_bits(19, 15, instruction))};
            int rs2{static_cast<int>(extract_bits(24, 20, instruction))};
            return {rs1, rs2};
        }
        int get_destination_register(uint32_t instruction){
            int rd{static_cast<int>(extract_bits(11, 7, instruction))};
            return rd;
        }

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

        void handle_R_type_instr(uint32_t instruction, uint32_t* registers){
            auto [rs1, rs2] = get_source_registers(instruction);
            int rd =      get_destination_register(instruction);

            uint16_t funct7{static_cast<uint16_t>(extract_bits(31, 25, instruction))};
            uint16_t funct3{static_cast<uint16_t>(extract_bits(14, 12, instruction))};
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

        int get_source_register(uint32_t instruction){
            int rs1{static_cast<int>(extract_bits(19, 15, instruction))};
            return rs1;
        }
        int get_destination_register(uint32_t instruction){
            int rd{static_cast<int>(extract_bits(11, 7, instruction))};
            return rd;
        }
        uint32_t get_imm(uint32_t instruction){
            uint32_t raw{extract_bits(31, 20, instruction)};
            // raw is a 12-bit field sitting in the low bits. Shifting it up so its
            // own bit 11 lands on bit 31 (the sign position of a 32-bit word), then
            // arithmetic-shifting back down, sign-extends it -- same arithmetic-shift
            // behavior already relied on in R::sra.
            int32_t imm = static_cast<int32_t>(raw << 20) >> 20;
            return static_cast<uint32_t>(imm);
        }

        void addi  (int rs1, int imm, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            writeRegister(rs1_val + imm, rd, registers);
        }
        void xori_ (int rs1, int imm, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            writeRegister(rs1_val ^ imm, rd, registers);
        }
        void ori_  (int rs1, int imm, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            writeRegister(rs1_val | imm, rd, registers);
        }
        void andi_ (int rs1, int imm, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            writeRegister(rs1_val & imm, rd, registers);
        }
        void slli  (int rs1, int imm, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t shamt = imm & 0x1F;
            writeRegister(rs1_val << shamt, rd, registers);
        }
        void srli  (int rs1, int imm, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t shamt = imm & 0x1F;
            writeRegister(rs1_val >> shamt, rd, registers);
        }
        void srai  (int rs1, int imm, int rd, uint32_t* registers){
            int32_t rs1_val = static_cast<int32_t>(registers[rs1]); 
            uint32_t shamt = imm & 0x1F;
            writeRegister(static_cast<uint32_t>(rs1_val >> shamt), rd, registers);
        }
        void slti  (int rs1, int imm, int rd, uint32_t* registers){
            int32_t rs1_val = static_cast<int32_t>(registers[rs1]);
            int32_t imm_val = static_cast<int32_t>(imm);
            writeRegister((rs1_val < imm_val) ? 1 : 0, rd, registers);
        }
        void sltiu (int rs1, int imm, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            writeRegister((rs1_val < static_cast<uint32_t>(imm)) ? 1 : 0, rd, registers);
        }

        void handle_I_type_instr(uint32_t instruction, uint32_t* registers){
            int rs1 =      get_source_register(instruction);
            int rd =  get_destination_register(instruction);
            uint32_t imm =             get_imm(instruction);

            uint16_t funct3{static_cast<uint16_t>(extract_bits(14, 12, instruction))};
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
        void lb    (int rs1, int imm, int rd, uint32_t* registers, uint8_t* memory);
        void lh    (int rs1, int imm, int rd, uint32_t* registers, uint8_t* memory);
        void lw    (int rs1, int imm, int rd, uint32_t* registers, uint8_t* memory);
        void lbu   (int rs1, int imm, int rd, uint32_t* registers, uint8_t* memory);
        void lhu   (int rs1, int imm, int rd, uint32_t* registers, uint8_t* memory);
        void jalr  (int rs1, int imm, int rd, uint32_t* registers, uint32_t& pc);
    }

    namespace S
    {
    
    }

    namespace B
    {
     
    }

    namespace J
    {
         
    }

    namespace U
    {
       
    }
}
