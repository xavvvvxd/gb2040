#include "core/cpu.h"
#include "core/console.h"

#include <cstdint>

#include <iostream>

namespace GB2040::Core
{

CPU::CPU(Console& console) 
: console(console) {
    // init
    
    initInstrTable();
    initCbInstrTable();
}

void CPU::push(uint16_t val) {
    SP -= 2;
    console.mmu.write16(SP, val);
}

uint16_t CPU::pop(void) {
    uint16_t val = console.mmu.read16(SP);
    SP += 2;

    return val;
}

void CPU::initInstrTable(void) {
    instrTable[0x00] = &CPU::NOP;
    instrTable[0x10] = &CPU::STOP;
    instrTable[0x20] = &CPU::JR_NZ_s8;
    instrTable[0x30] = &CPU::JR_NC_s8;

    instrTable[0x01] = &CPU::LD_BC_d16;
    instrTable[0x11] = &CPU::LD_DE_d16;
    instrTable[0x21] = &CPU::LD_HL_d16;
    instrTable[0x31] = &CPU::LD_SP_d16;

    instrTable[0x02] = &CPU::LD_mBC_A;
    instrTable[0x12] = &CPU::LD_mDE_A;
    instrTable[0x22] = &CPU::LD_mHLi_A;
    instrTable[0x32] = &CPU::LD_mHLd_A;

    instrTable[0x03] = &CPU::INC_BC;
    instrTable[0x13] = &CPU::INC_DE;
    instrTable[0x23] = &CPU::INC_HL;
    instrTable[0x33] = &CPU::INC_SP;

    instrTable[0x04] = &CPU::INC_B;
    instrTable[0x14] = &CPU::INC_D;
    instrTable[0x24] = &CPU::INC_H;
    instrTable[0x34] = &CPU::INC_mHL;

    instrTable[0x05] = &CPU::DEC_B;
    instrTable[0x15] = &CPU::DEC_D;
    instrTable[0x25] = &CPU::DEC_H;
    instrTable[0x35] = &CPU::DEC_mHL;

    instrTable[0x06] = &CPU::LD_B_d8;
    instrTable[0x16] = &CPU::LD_D_d8;
    instrTable[0x26] = &CPU::LD_H_d8;
    instrTable[0x36] = &CPU::LD_mHL_d8;

    instrTable[0x07] = &CPU::RLCA;
    instrTable[0x17] = &CPU::RLA;
    instrTable[0x27] = &CPU::DAA;
    instrTable[0x37] = &CPU::SCF;

    instrTable[0x08] = &CPU::LD_a16_SP;
    instrTable[0x18] = &CPU::JR_s8;
    instrTable[0x28] = &CPU::JR_Z_s8;
    instrTable[0x38] = &CPU::JR_C_s8;

    instrTable[0x09] = &CPU::ADD_HL_BC;
    instrTable[0x19] = &CPU::ADD_HL_DE;
    instrTable[0x29] = &CPU::ADD_HL_HL;
    instrTable[0x39] = &CPU::ADD_HL_SP;

    instrTable[0x0A] = &CPU::LD_A_mBC;
    instrTable[0x1A] = &CPU::LD_A_mDE;
    instrTable[0x2A] = &CPU::LD_A_mHLi;
    instrTable[0x3A] = &CPU::LD_A_mHLd;

    instrTable[0x0B] = &CPU::DEC_BC;
    instrTable[0x1B] = &CPU::DEC_DE;
    instrTable[0x2B] = &CPU::DEC_HL;
    instrTable[0x3B] = &CPU::DEC_SP;

    instrTable[0x0C] = &CPU::INC_C;
    instrTable[0x1C] = &CPU::INC_E;
    instrTable[0x2C] = &CPU::INC_L;
    instrTable[0x3C] = &CPU::INC_A;

    instrTable[0x0D] = &CPU::DEC_C;
    instrTable[0x1D] = &CPU::DEC_E;
    instrTable[0x2D] = &CPU::DEC_L;
    instrTable[0x3D] = &CPU::DEC_A;

    instrTable[0x0E] = &CPU::LD_C_d8;
    instrTable[0x1E] = &CPU::LD_E_d8;
    instrTable[0x2E] = &CPU::LD_L_d8;
    instrTable[0x3E] = &CPU::LD_A_d8;

    instrTable[0x0F] = &CPU::RRCA;
    instrTable[0x1F] = &CPU::RRA;
    instrTable[0x2F] = &CPU::CPL;
    instrTable[0x3F] = &CPU::CCF;

    instrTable[0x40] = &CPU::LD_B_B;
    instrTable[0x50] = &CPU::LD_D_B;
    instrTable[0x60] = &CPU::LD_H_B;
    instrTable[0x70] = &CPU::LD_mHL_B;

    instrTable[0x41] = &CPU::LD_B_C;
    instrTable[0x51] = &CPU::LD_D_C;
    instrTable[0x61] = &CPU::LD_H_C;
    instrTable[0x71] = &CPU::LD_mHL_C;

    instrTable[0x42] = &CPU::LD_B_D;
    instrTable[0x52] = &CPU::LD_D_D;
    instrTable[0x62] = &CPU::LD_H_D;
    instrTable[0x72] = &CPU::LD_mHL_D;

    instrTable[0x43] = &CPU::LD_B_E;
    instrTable[0x53] = &CPU::LD_D_E;
    instrTable[0x63] = &CPU::LD_H_E;
    instrTable[0x73] = &CPU::LD_mHL_E;

    instrTable[0x44] = &CPU::LD_B_H;
    instrTable[0x54] = &CPU::LD_D_H;
    instrTable[0x64] = &CPU::LD_H_H;
    instrTable[0x74] = &CPU::LD_mHL_H;

    instrTable[0x45] = &CPU::LD_B_L;
    instrTable[0x55] = &CPU::LD_D_L;
    instrTable[0x65] = &CPU::LD_H_L;
    instrTable[0x75] = &CPU::LD_mHL_L;

    instrTable[0x46] = &CPU::LD_B_mHL;
    instrTable[0x56] = &CPU::LD_D_mHL;
    instrTable[0x66] = &CPU::LD_H_mHL;
    instrTable[0x76] = &CPU::HALT;

    instrTable[0x47] = &CPU::LD_B_A;
    instrTable[0x57] = &CPU::LD_D_A;
    instrTable[0x67] = &CPU::LD_H_A;
    instrTable[0x77] = &CPU::LD_mHL_A;

    instrTable[0x48] = &CPU::LD_C_B;
    instrTable[0x58] = &CPU::LD_E_B;
    instrTable[0x68] = &CPU::LD_L_B;
    instrTable[0x78] = &CPU::LD_A_B;

    instrTable[0x49] = &CPU::LD_C_C;
    instrTable[0x59] = &CPU::LD_E_C;
    instrTable[0x69] = &CPU::LD_L_C;
    instrTable[0x79] = &CPU::LD_A_C;

    instrTable[0x4A] = &CPU::LD_C_D;
    instrTable[0x5A] = &CPU::LD_E_D;
    instrTable[0x6A] = &CPU::LD_L_D;
    instrTable[0x7A] = &CPU::LD_A_D;

    instrTable[0x4B] = &CPU::LD_C_E;
    instrTable[0x5B] = &CPU::LD_E_E;
    instrTable[0x6B] = &CPU::LD_L_E;
    instrTable[0x7B] = &CPU::LD_A_E;
    
    instrTable[0x4C] = &CPU::LD_C_H;
    instrTable[0x5C] = &CPU::LD_E_H;
    instrTable[0x6C] = &CPU::LD_L_H;
    instrTable[0x7C] = &CPU::LD_A_H;

    instrTable[0x4D] = &CPU::LD_C_L;
    instrTable[0x5D] = &CPU::LD_E_L;
    instrTable[0x6D] = &CPU::LD_L_L;
    instrTable[0x7D] = &CPU::LD_A_L;

    instrTable[0x4E] = &CPU::LD_C_mHL;
    instrTable[0x5E] = &CPU::LD_E_mHL;
    instrTable[0x6E] = &CPU::LD_L_mHL;
    instrTable[0x7E] = &CPU::LD_A_mHL;

    instrTable[0x4F] = &CPU::LD_C_A;
    instrTable[0x5F] = &CPU::LD_E_A;
    instrTable[0x6F] = &CPU::LD_L_A;
    instrTable[0x7F] = &CPU::LD_A_A;

    instrTable[0x80] = &CPU::ADD_A_B;
    instrTable[0x81] = &CPU::ADD_A_C;
    instrTable[0x82] = &CPU::ADD_A_D;
    instrTable[0x83] = &CPU::ADD_A_E;
    instrTable[0x84] = &CPU::ADD_A_H;
    instrTable[0x85] = &CPU::ADD_A_L;
    instrTable[0x86] = &CPU::ADD_A_mHL;
    instrTable[0x87] = &CPU::ADD_A_A;

    instrTable[0x90] = &CPU::SUB_B;
    instrTable[0x91] = &CPU::SUB_C;
    instrTable[0x92] = &CPU::SUB_D;
    instrTable[0x93] = &CPU::SUB_E;
    instrTable[0x94] = &CPU::SUB_H;
    instrTable[0x95] = &CPU::SUB_L;
    instrTable[0x96] = &CPU::SUB_mHL;
    instrTable[0x97] = &CPU::SUB_A;

    instrTable[0xA0] = &CPU::AND_B;
    instrTable[0xA1] = &CPU::AND_C;
    instrTable[0xA2] = &CPU::AND_D;
    instrTable[0xA3] = &CPU::AND_E;
    instrTable[0xA4] = &CPU::AND_H;
    instrTable[0xA5] = &CPU::AND_L;
    instrTable[0xA6] = &CPU::AND_mHL;
    instrTable[0xA7] = &CPU::AND_A;

    instrTable[0xB0] = &CPU::OR_B;
    instrTable[0xB1] = &CPU::OR_C;
    instrTable[0xB2] = &CPU::OR_D;
    instrTable[0xB3] = &CPU::OR_E;
    instrTable[0xB4] = &CPU::OR_H;
    instrTable[0xB5] = &CPU::OR_L;
    instrTable[0xB6] = &CPU::OR_mHL;
    instrTable[0xB7] = &CPU::OR_A;

    instrTable[0x88] = &CPU::ADC_A_B;
    instrTable[0x89] = &CPU::ADC_A_C;
    instrTable[0x8A] = &CPU::ADC_A_D;
    instrTable[0x8B] = &CPU::ADC_A_E;
    instrTable[0x8C] = &CPU::ADC_A_H;
    instrTable[0x8D] = &CPU::ADC_A_L;
    instrTable[0x8E] = &CPU::ADC_A_mHL;
    instrTable[0x8F] = &CPU::ADC_A_A;

    instrTable[0x98] = &CPU::SBC_A_B;
    instrTable[0x99] = &CPU::SBC_A_C;
    instrTable[0x9A] = &CPU::SBC_A_D;
    instrTable[0x9B] = &CPU::SBC_A_E;
    instrTable[0x9C] = &CPU::SBC_A_H;
    instrTable[0x9D] = &CPU::SBC_A_L;
    instrTable[0x9E] = &CPU::SBC_A_mHL;
    instrTable[0x9F] = &CPU::SBC_A_A;

    instrTable[0xA8] = &CPU::XOR_B;
    instrTable[0xA9] = &CPU::XOR_C;
    instrTable[0xAA] = &CPU::XOR_D;
    instrTable[0xAB] = &CPU::XOR_E;
    instrTable[0xAC] = &CPU::XOR_H;
    instrTable[0xAD] = &CPU::XOR_L;
    instrTable[0xAE] = &CPU::XOR_mHL;
    instrTable[0xAF] = &CPU::XOR_A;

    instrTable[0xB8] = &CPU::CP_B;
    instrTable[0xB9] = &CPU::CP_C;
    instrTable[0xBA] = &CPU::CP_D;
    instrTable[0xBB] = &CPU::CP_E;
    instrTable[0xBC] = &CPU::CP_H;
    instrTable[0xBD] = &CPU::CP_L;
    instrTable[0xBE] = &CPU::CP_mHL;
    instrTable[0xBF] = &CPU::CP_A;

    instrTable[0xC0] = &CPU::RET_NZ;
    instrTable[0xD0] = &CPU::RET_NC;
    instrTable[0xE0] = &CPU::LD_a8_A;
    instrTable[0xF0] = &CPU::LD_A_a8;

    instrTable[0xC1] = &CPU::POP_BC;
    instrTable[0xD1] = &CPU::POP_DE;
    instrTable[0xE1] = &CPU::POP_HL;
    instrTable[0xF1] = &CPU::POP_AF;

    instrTable[0xC2] = &CPU::JP_NZ_a16;
    instrTable[0xD2] = &CPU::JP_NC_a16;
    instrTable[0xE2] = &CPU::LD_mC_A;
    instrTable[0xF2] = &CPU::LD_A_mC;

    instrTable[0xC3] = &CPU::JP_a16;
    instrTable[0xF3] = &CPU::DI;

    instrTable[0xC4] = &CPU::CALL_NZ_a16;
    instrTable[0xD4] = &CPU::CALL_NC_a16;

    instrTable[0xC5] = &CPU::PUSH_BC;
    instrTable[0xD5] = &CPU::PUSH_DE;
    instrTable[0xE5] = &CPU::PUSH_HL;
    instrTable[0xF5] = &CPU::PUSH_AF;

    instrTable[0xC6] = &CPU::ADD_A_d8;
    instrTable[0xD6] = &CPU::SUB_d8;
    instrTable[0xE6] = &CPU::AND_d8;
    instrTable[0xF6] = &CPU::OR_d8;

    instrTable[0xC7] = &CPU::RST_0;
    instrTable[0xD7] = &CPU::RST_2;
    instrTable[0xE7] = &CPU::RST_4;
    instrTable[0xF7] = &CPU::RST_6;

    instrTable[0xC8] = &CPU::RET_Z;
    instrTable[0xD8] = &CPU::RET_C;
    instrTable[0xE8] = &CPU::ADD_SP_s8;
    instrTable[0xF8] = &CPU::LD_HL_SP_s8;

    instrTable[0xC9] = &CPU::RET;
    instrTable[0xD9] = &CPU::RETI;
    instrTable[0xE9] = &CPU::JP_HL;
    instrTable[0xF9] = &CPU::LD_SP_HL;

    instrTable[0xCA] = &CPU::JP_Z_a16;
    instrTable[0xDA] = &CPU::JP_C_a16;
    instrTable[0xEA] = &CPU::LD_a16_A;
    instrTable[0xFA] = &CPU::LD_A_ma16;

    instrTable[0xFB] = &CPU::EI;

    instrTable[0xCC] = &CPU::CALL_Z_a16;
    instrTable[0xDC] = &CPU::CALL_C_a16;

    instrTable[0xCD] = &CPU::CALL_a16;

    instrTable[0xCE] = &CPU::ADC_A_d8;
    instrTable[0xDE] = &CPU::SBC_A_d8;
    instrTable[0xEE] = &CPU::XOR_d8;
    instrTable[0xFE] = &CPU::CP_d8;

    instrTable[0xCF] = &CPU::RST_1;
    instrTable[0xDF] = &CPU::RST_3;
    instrTable[0xEF] = &CPU::RST_5;
    instrTable[0xFF] = &CPU::RST_7;
}

void CPU::initCbInstrTable(void) {
    cbInstrTable[0x00] = &CPU::RLC_B;
    cbInstrTable[0x01] = &CPU::RLC_C;
    cbInstrTable[0x02] = &CPU::RLC_D;
    cbInstrTable[0x03] = &CPU::RLC_E;
    cbInstrTable[0x04] = &CPU::RLC_H;
    cbInstrTable[0x05] = &CPU::RLC_L;
    cbInstrTable[0x06] = &CPU::RLC_mHL;
    cbInstrTable[0x07] = &CPU::RLC_A;

    cbInstrTable[0x08] = &CPU::RRC_B;
    cbInstrTable[0x09] = &CPU::RRC_C;
    cbInstrTable[0x0A] = &CPU::RRC_D;
    cbInstrTable[0x0B] = &CPU::RRC_E;
    cbInstrTable[0x0C] = &CPU::RRC_H;
    cbInstrTable[0x0D] = &CPU::RRC_L;
    cbInstrTable[0x0E] = &CPU::RRC_mHL;
    cbInstrTable[0x0F] = &CPU::RRC_A;

    cbInstrTable[0x10] = &CPU::RL_B;
    cbInstrTable[0x11] = &CPU::RL_C;
    cbInstrTable[0x12] = &CPU::RL_D;
    cbInstrTable[0x13] = &CPU::RL_E;
    cbInstrTable[0x14] = &CPU::RL_H;
    cbInstrTable[0x15] = &CPU::RL_L;
    cbInstrTable[0x16] = &CPU::RL_mHL;
    cbInstrTable[0x17] = &CPU::RL_A;

    cbInstrTable[0x18] = &CPU::RR_B;
    cbInstrTable[0x19] = &CPU::RR_C;
    cbInstrTable[0x1A] = &CPU::RR_D;
    cbInstrTable[0x1B] = &CPU::RR_E;
    cbInstrTable[0x1C] = &CPU::RR_H;
    cbInstrTable[0x1D] = &CPU::RR_L;
    cbInstrTable[0x1E] = &CPU::RR_mHL;
    cbInstrTable[0x1F] = &CPU::RR_A;

    cbInstrTable[0x20] = &CPU::SLA_B;
    cbInstrTable[0x21] = &CPU::SLA_C;
    cbInstrTable[0x22] = &CPU::SLA_D;
    cbInstrTable[0x23] = &CPU::SLA_E;
    cbInstrTable[0x24] = &CPU::SLA_H;
    cbInstrTable[0x25] = &CPU::SLA_L;
    cbInstrTable[0x26] = &CPU::SLA_mHL;
    cbInstrTable[0x27] = &CPU::SLA_A;

    cbInstrTable[0x28] = &CPU::SRA_B;
    cbInstrTable[0x29] = &CPU::SRA_C;
    cbInstrTable[0x2A] = &CPU::SRA_D;
    cbInstrTable[0x2B] = &CPU::SRA_E;
    cbInstrTable[0x2C] = &CPU::SRA_H;
    cbInstrTable[0x2D] = &CPU::SRA_L;
    cbInstrTable[0x2E] = &CPU::SRA_mHL;
    cbInstrTable[0x2F] = &CPU::SRA_A;

    cbInstrTable[0x30] = &CPU::SWAP_B;
    cbInstrTable[0x31] = &CPU::SWAP_C;
    cbInstrTable[0x32] = &CPU::SWAP_D;
    cbInstrTable[0x33] = &CPU::SWAP_E;
    cbInstrTable[0x34] = &CPU::SWAP_H;
    cbInstrTable[0x35] = &CPU::SWAP_L;
    cbInstrTable[0x36] = &CPU::SWAP_mHL;
    cbInstrTable[0x37] = &CPU::SWAP_A;

    cbInstrTable[0x38] = &CPU::SRL_B;
    cbInstrTable[0x39] = &CPU::SRL_C;
    cbInstrTable[0x3A] = &CPU::SRL_D;
    cbInstrTable[0x3B] = &CPU::SRL_E;
    cbInstrTable[0x3C] = &CPU::SRL_H;
    cbInstrTable[0x3D] = &CPU::SRL_L;
    cbInstrTable[0x3E] = &CPU::SRL_mHL;
    cbInstrTable[0x3F] = &CPU::SRL_A;

    cbInstrTable[0x40] = &CPU::BIT_0_B;
    cbInstrTable[0x41] = &CPU::BIT_0_C;
    cbInstrTable[0x42] = &CPU::BIT_0_D;
    cbInstrTable[0x43] = &CPU::BIT_0_E;
    cbInstrTable[0x44] = &CPU::BIT_0_H;
    cbInstrTable[0x45] = &CPU::BIT_0_L;
    cbInstrTable[0x46] = &CPU::BIT_0_mHL;
    cbInstrTable[0x47] = &CPU::BIT_0_A;

    cbInstrTable[0x48] = &CPU::BIT_1_B;
    cbInstrTable[0x49] = &CPU::BIT_1_C;
    cbInstrTable[0x4A] = &CPU::BIT_1_D;
    cbInstrTable[0x4B] = &CPU::BIT_1_E;
    cbInstrTable[0x4C] = &CPU::BIT_1_H;
    cbInstrTable[0x4D] = &CPU::BIT_1_L;
    cbInstrTable[0x4E] = &CPU::BIT_1_mHL;
    cbInstrTable[0x4F] = &CPU::BIT_1_A;

    cbInstrTable[0x50] = &CPU::BIT_2_B;
    cbInstrTable[0x51] = &CPU::BIT_2_C;
    cbInstrTable[0x52] = &CPU::BIT_2_D;
    cbInstrTable[0x53] = &CPU::BIT_2_E;
    cbInstrTable[0x54] = &CPU::BIT_2_H;
    cbInstrTable[0x55] = &CPU::BIT_2_L;
    cbInstrTable[0x56] = &CPU::BIT_2_mHL;
    cbInstrTable[0x57] = &CPU::BIT_2_A;

    cbInstrTable[0x58] = &CPU::BIT_3_B;
    cbInstrTable[0x59] = &CPU::BIT_3_C;
    cbInstrTable[0x5A] = &CPU::BIT_3_D;
    cbInstrTable[0x5B] = &CPU::BIT_3_E;
    cbInstrTable[0x5C] = &CPU::BIT_3_H;
    cbInstrTable[0x5D] = &CPU::BIT_3_L;
    cbInstrTable[0x5E] = &CPU::BIT_3_mHL;
    cbInstrTable[0x5F] = &CPU::BIT_3_A;

    cbInstrTable[0x60] = &CPU::BIT_4_B;
    cbInstrTable[0x61] = &CPU::BIT_4_C;
    cbInstrTable[0x62] = &CPU::BIT_4_D;
    cbInstrTable[0x63] = &CPU::BIT_4_E;
    cbInstrTable[0x64] = &CPU::BIT_4_H;
    cbInstrTable[0x65] = &CPU::BIT_4_L;
    cbInstrTable[0x66] = &CPU::BIT_4_mHL;
    cbInstrTable[0x67] = &CPU::BIT_4_A;

    cbInstrTable[0x68] = &CPU::BIT_5_B;
    cbInstrTable[0x69] = &CPU::BIT_5_C;
    cbInstrTable[0x6A] = &CPU::BIT_5_D;
    cbInstrTable[0x6B] = &CPU::BIT_5_E;
    cbInstrTable[0x6C] = &CPU::BIT_5_H;
    cbInstrTable[0x6D] = &CPU::BIT_5_L;
    cbInstrTable[0x6E] = &CPU::BIT_5_mHL;
    cbInstrTable[0x6F] = &CPU::BIT_5_A;

    cbInstrTable[0x70] = &CPU::BIT_6_B;
    cbInstrTable[0x71] = &CPU::BIT_6_C;
    cbInstrTable[0x72] = &CPU::BIT_6_D;
    cbInstrTable[0x73] = &CPU::BIT_6_E;
    cbInstrTable[0x74] = &CPU::BIT_6_H;
    cbInstrTable[0x75] = &CPU::BIT_6_L;
    cbInstrTable[0x76] = &CPU::BIT_6_mHL;
    cbInstrTable[0x77] = &CPU::BIT_6_A;

    cbInstrTable[0x78] = &CPU::BIT_7_B;
    cbInstrTable[0x79] = &CPU::BIT_7_C;
    cbInstrTable[0x7A] = &CPU::BIT_7_D;
    cbInstrTable[0x7B] = &CPU::BIT_7_E;
    cbInstrTable[0x7C] = &CPU::BIT_7_H;
    cbInstrTable[0x7D] = &CPU::BIT_7_L;
    cbInstrTable[0x7E] = &CPU::BIT_7_mHL;
    cbInstrTable[0x7F] = &CPU::BIT_7_A;

    cbInstrTable[0x80] = &CPU::RES_0_B;
    cbInstrTable[0x81] = &CPU::RES_0_C;
    cbInstrTable[0x82] = &CPU::RES_0_D;
    cbInstrTable[0x83] = &CPU::RES_0_E;
    cbInstrTable[0x84] = &CPU::RES_0_H;
    cbInstrTable[0x85] = &CPU::RES_0_L;
    cbInstrTable[0x86] = &CPU::RES_0_mHL;
    cbInstrTable[0x87] = &CPU::RES_0_A;

    cbInstrTable[0x88] = &CPU::RES_1_B;
    cbInstrTable[0x89] = &CPU::RES_1_C;
    cbInstrTable[0x8A] = &CPU::RES_1_D;
    cbInstrTable[0x8B] = &CPU::RES_1_E;
    cbInstrTable[0x8C] = &CPU::RES_1_H;
    cbInstrTable[0x8D] = &CPU::RES_1_L;
    cbInstrTable[0x8E] = &CPU::RES_1_mHL;
    cbInstrTable[0x8F] = &CPU::RES_1_A;

    cbInstrTable[0x90] = &CPU::RES_2_B;
    cbInstrTable[0x91] = &CPU::RES_2_C;
    cbInstrTable[0x92] = &CPU::RES_2_D;
    cbInstrTable[0x93] = &CPU::RES_2_E;
    cbInstrTable[0x94] = &CPU::RES_2_H;
    cbInstrTable[0x95] = &CPU::RES_2_L;
    cbInstrTable[0x96] = &CPU::RES_2_mHL;
    cbInstrTable[0x97] = &CPU::RES_2_A;

    cbInstrTable[0x98] = &CPU::RES_3_B;
    cbInstrTable[0x99] = &CPU::RES_3_C;
    cbInstrTable[0x9A] = &CPU::RES_3_D;
    cbInstrTable[0x9B] = &CPU::RES_3_E;
    cbInstrTable[0x9C] = &CPU::RES_3_H;
    cbInstrTable[0x9D] = &CPU::RES_3_L;
    cbInstrTable[0x9E] = &CPU::RES_3_mHL;
    cbInstrTable[0x9F] = &CPU::RES_3_A;

    cbInstrTable[0xA0] = &CPU::RES_4_B;
    cbInstrTable[0xA1] = &CPU::RES_4_C;
    cbInstrTable[0xA2] = &CPU::RES_4_D;
    cbInstrTable[0xA3] = &CPU::RES_4_E;
    cbInstrTable[0xA4] = &CPU::RES_4_H;
    cbInstrTable[0xA5] = &CPU::RES_4_L;
    cbInstrTable[0xA6] = &CPU::RES_4_mHL;
    cbInstrTable[0xA7] = &CPU::RES_4_A;

    cbInstrTable[0xA8] = &CPU::RES_5_B;
    cbInstrTable[0xA9] = &CPU::RES_5_C;
    cbInstrTable[0xAA] = &CPU::RES_5_D;
    cbInstrTable[0xAB] = &CPU::RES_5_E;
    cbInstrTable[0xAC] = &CPU::RES_5_H;
    cbInstrTable[0xAD] = &CPU::RES_5_L;
    cbInstrTable[0xAE] = &CPU::RES_5_mHL;
    cbInstrTable[0xAF] = &CPU::RES_5_A;

    cbInstrTable[0xB0] = &CPU::RES_6_B;
    cbInstrTable[0xB1] = &CPU::RES_6_C;
    cbInstrTable[0xB2] = &CPU::RES_6_D;
    cbInstrTable[0xB3] = &CPU::RES_6_E;
    cbInstrTable[0xB4] = &CPU::RES_6_H;
    cbInstrTable[0xB5] = &CPU::RES_6_L;
    cbInstrTable[0xB6] = &CPU::RES_6_mHL;
    cbInstrTable[0xB7] = &CPU::RES_6_A;

    cbInstrTable[0xB8] = &CPU::RES_7_B;
    cbInstrTable[0xB9] = &CPU::RES_7_C;
    cbInstrTable[0xBA] = &CPU::RES_7_D;
    cbInstrTable[0xBB] = &CPU::RES_7_E;
    cbInstrTable[0xBC] = &CPU::RES_7_H;
    cbInstrTable[0xBD] = &CPU::RES_7_L;
    cbInstrTable[0xBE] = &CPU::RES_7_mHL;
    cbInstrTable[0xBF] = &CPU::RES_7_A;

    cbInstrTable[0xC0] = &CPU::SET_0_B;
    cbInstrTable[0xC1] = &CPU::SET_0_C;
    cbInstrTable[0xC2] = &CPU::SET_0_D;
    cbInstrTable[0xC3] = &CPU::SET_0_E;
    cbInstrTable[0xC4] = &CPU::SET_0_H;
    cbInstrTable[0xC5] = &CPU::SET_0_L;
    cbInstrTable[0xC6] = &CPU::SET_0_mHL;
    cbInstrTable[0xC7] = &CPU::SET_0_A;

    cbInstrTable[0xC8] = &CPU::SET_1_B;
    cbInstrTable[0xC9] = &CPU::SET_1_C;
    cbInstrTable[0xCA] = &CPU::SET_1_D;
    cbInstrTable[0xCB] = &CPU::SET_1_E;
    cbInstrTable[0xCC] = &CPU::SET_1_H;
    cbInstrTable[0xCD] = &CPU::SET_1_L;
    cbInstrTable[0xCE] = &CPU::SET_1_mHL;
    cbInstrTable[0xCF] = &CPU::SET_1_A;

    cbInstrTable[0xD0] = &CPU::SET_2_B;
    cbInstrTable[0xD1] = &CPU::SET_2_C;
    cbInstrTable[0xD2] = &CPU::SET_2_D;
    cbInstrTable[0xD3] = &CPU::SET_2_E;
    cbInstrTable[0xD4] = &CPU::SET_2_H;
    cbInstrTable[0xD5] = &CPU::SET_2_L;
    cbInstrTable[0xD6] = &CPU::SET_2_mHL;
    cbInstrTable[0xD7] = &CPU::SET_2_A;

    cbInstrTable[0xD8] = &CPU::SET_3_B;
    cbInstrTable[0xD9] = &CPU::SET_3_C;
    cbInstrTable[0xDA] = &CPU::SET_3_D;
    cbInstrTable[0xDB] = &CPU::SET_3_E;
    cbInstrTable[0xDC] = &CPU::SET_3_H;
    cbInstrTable[0xDD] = &CPU::SET_3_L;
    cbInstrTable[0xDE] = &CPU::SET_3_mHL;
    cbInstrTable[0xDF] = &CPU::SET_3_A;

    cbInstrTable[0xE0] = &CPU::SET_4_B;
    cbInstrTable[0xE1] = &CPU::SET_4_C;
    cbInstrTable[0xE2] = &CPU::SET_4_D;
    cbInstrTable[0xE3] = &CPU::SET_4_E;
    cbInstrTable[0xE4] = &CPU::SET_4_H;
    cbInstrTable[0xE5] = &CPU::SET_4_L;
    cbInstrTable[0xE6] = &CPU::SET_4_mHL;
    cbInstrTable[0xE7] = &CPU::SET_4_A;

    cbInstrTable[0xE8] = &CPU::SET_5_B;
    cbInstrTable[0xE9] = &CPU::SET_5_C;
    cbInstrTable[0xEA] = &CPU::SET_5_D;
    cbInstrTable[0xEB] = &CPU::SET_5_E;
    cbInstrTable[0xEC] = &CPU::SET_5_H;
    cbInstrTable[0xED] = &CPU::SET_5_L;
    cbInstrTable[0xEE] = &CPU::SET_5_mHL;
    cbInstrTable[0xEF] = &CPU::SET_5_A;

    cbInstrTable[0xF0] = &CPU::SET_6_B;
    cbInstrTable[0xF1] = &CPU::SET_6_C;
    cbInstrTable[0xF2] = &CPU::SET_6_D;
    cbInstrTable[0xF3] = &CPU::SET_6_E;
    cbInstrTable[0xF4] = &CPU::SET_6_H;
    cbInstrTable[0xF5] = &CPU::SET_6_L;
    cbInstrTable[0xF6] = &CPU::SET_6_mHL;
    cbInstrTable[0xF7] = &CPU::SET_6_A;

    cbInstrTable[0xF8] = &CPU::SET_7_B;
    cbInstrTable[0xF9] = &CPU::SET_7_C;
    cbInstrTable[0xFA] = &CPU::SET_7_D;
    cbInstrTable[0xFB] = &CPU::SET_7_E;
    cbInstrTable[0xFC] = &CPU::SET_7_H;
    cbInstrTable[0xFD] = &CPU::SET_7_L;
    cbInstrTable[0xFE] = &CPU::SET_7_mHL;
    cbInstrTable[0xFF] = &CPU::SET_7_A;
}

void CPU::setFlag(FlagMask flag, bool val) {
    if (val) AF.lo |= 1 << flag;
    else AF.lo &= ~(1 << flag);
}

bool CPU::getFlag(FlagMask flag) {
    return (AF.lo >> flag) & 1;
}

uint8_t CPU::fetch8(void) {
    uint8_t ret = console.mmu.read8(PC);
    PC += !haltBug;
    haltBug = false;

    return ret;
}

uint16_t CPU::fetch16(void) {
    uint16_t ret = console.mmu.read16(PC);
    PC += 2;

    return ret;
}

bool CPU::checkInterrupts(void) {
    halted = false;
    stopped = false;
    if (!ime) return false;

    uint8_t pending = intFlag & ie;

    if (!pending) return false;

    uint8_t vectors[5] = { 0x40, 0x48, 0x50, 0x58, 0x60 }; // VBLANK, STAT, timer, serial and joypad
    for (int i = 0; i < 5; i++) {
        if ((pending >> i) & 1) {
            ime = false;

            intFlag = intFlag & ~(1 << i);
            
            // service interrupt
            push(PC);
            PC = vectors[i];

            return true; // interrupt serviced
        }
    }

    return false;
}

uint8_t CPU::execute(uint8_t opcode) {
    if (opcode == 0xCB) {
        // handle CB prefix
        opcode = fetch8();

        OpcodeImpl impl = cbInstrTable[opcode];
        if (impl) {
            uint8_t cycles = (this->*impl)() * 4;

            return cycles;
        } else {
            // invalid opcode
            printf("error - invalid opcode $%02X\n", opcode);
            PC--; // loop (also makes a cool sound!!)

            return 4;
        }
    }

    OpcodeImpl impl = instrTable[opcode];

    if (impl) {
        uint8_t cycles = (this->*impl)() * 4;
        return cycles;
    } else {
        // invalid opcode
        printf("error - invalid opcode $%02X\n", opcode);
        PC--; // loop (also makes a cool sound!!)
        return 4;
    }
}

char* CPU::getDebug(void) {
    static char output[256];

    uint8_t pcMem[4];
    for (int i = 0; i < 4; i++) {
        pcMem[i] = console.mmu.read8(PC + i);
    }

    sprintf(output,
            "A: %02X F: %02X B: %02X C: %02X D: %02X E: %02X H: %02X L: %02X SP: %04X PC: %04X (%02X %02X %02X %02X)",
            AF.hi, AF.lo, BC.hi, BC.lo, DE.hi, DE.lo, HL.hi, HL.lo, SP, PC, pcMem[0], pcMem[1], pcMem[2], pcMem[3]);
    
    return output;
}

void CPU::yieldCycles(size_t cycles) {
    forceCycles = cycles;
}

size_t CPU::tick(void) {
    if (forceCycles) {
        size_t c = forceCycles;
        forceCycles = 0;
        return c;
    }

    if (eiPending) {
        ime = true;
        eiPending = false;
    }

    bool interruptServiced = checkInterrupts();

    if (stopped || halted) {
        return 4;
    }

    if (interruptServiced) return 20;

    uint8_t opcode = fetch8();
    size_t cycles = execute(opcode);

    return cycles;
}

} // namespace GB2040::Core