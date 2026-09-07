#pragma once

#include <cstdint>
#include <cstddef>

namespace GB2040::Core
{

class Console; // forward declaration
class MMU;
class PPU;
class CPU; // forward declaration for OpcodeImpl

enum FlagMask : uint8_t {
    CF = 4,
    HF = 5,
    NF = 6,
    ZF = 7
};

typedef uint8_t (CPU::*OpcodeImpl)();

struct RegisterPair {
    uint8_t hi;
    uint8_t lo;

    uint16_t get() {
        return (hi << 8) | lo;
    }

    void set(uint16_t val) {
        lo = val & 0xFF;
        hi = val >> 8;
    }
};

class CPU {
public:
    CPU(Console& console);
    void init(MMU& mmu, PPU& ppu);
    size_t tick(void);
    char* getDebug(void);
    void yieldCycles(size_t);

    bool ime = false;
    uint8_t ie = 0;
    uint8_t intFlag = 0;
    bool stopped = false;

private:
    Console& console;
    MMU* mmu = nullptr;
    PPU* ppu = nullptr;

    bool halted = false;
    bool haltBug = false;

    bool eiPending = false; // interrupts only enable *after 1 instruction*

    size_t forceCycles = 0;

    uint8_t execute(uint8_t);

    // stack helpers
    void push(uint16_t);
    uint16_t pop(void);

    uint8_t fetch8(void);
    uint16_t fetch16(void);

    bool checkInterrupts(void);

    inline void setFlag(FlagMask flag, bool val) {
        if (val) AF.lo |= 1 << flag;
        else AF.lo &= ~(1 << flag);
    }

    inline bool getFlag(FlagMask flag) {
        return (AF.lo >> flag) & 1;
    }

    void initInstrTable(void);
    void initCbInstrTable(void);

    // opcode helpers
    uint8_t LD_r16_d16(RegisterPair&);
    uint8_t LD_mr16_A(RegisterPair&, int8_t = 0);
    uint8_t INC_r16(RegisterPair&);
    uint8_t DEC_r16(RegisterPair&);
    uint8_t INC_r8(uint8_t&);
    uint8_t DEC_r8(uint8_t&);
    uint8_t LD_r8_d8(uint8_t&);
    uint8_t ADD_HL_r16(RegisterPair&);
    uint8_t LD_A_mr16(RegisterPair&, int8_t = 0);
    uint8_t JR_F_s8(bool);

    uint8_t LD_r8_r8(uint8_t&, uint8_t&);
    uint8_t LD_r8_mHL(uint8_t&);
    uint8_t LD_mHL_r8(uint8_t&);

    uint8_t ADD_A_r8(uint8_t&);
    uint8_t SUB_r8(uint8_t&);
    uint8_t AND_r8(uint8_t&);
    uint8_t OR_r8(uint8_t&);
    uint8_t ADC_A_r8(uint8_t&);
    uint8_t SBC_A_r8(uint8_t&);
    uint8_t XOR_r8(uint8_t&);
    uint8_t CP_r8(uint8_t&);

    uint8_t RET_F(bool);

    uint8_t PUSH_r16(RegisterPair&);
    uint8_t POP_r16(RegisterPair&);

    uint8_t JP_F_a16(bool);
    uint8_t CALL_F_a16(bool);

    uint8_t RST_n(uint8_t);

    // opcodes
    uint8_t NOP(void);
    uint8_t STOP(void);
    uint8_t JR_NZ_s8(void);
    uint8_t JR_NC_s8(void);

    uint8_t LD_BC_d16(void);
    uint8_t LD_DE_d16(void);
    uint8_t LD_HL_d16(void);
    uint8_t LD_SP_d16(void);

    uint8_t LD_mBC_A(void);
    uint8_t LD_mDE_A(void);
    uint8_t LD_mHLi_A(void);
    uint8_t LD_mHLd_A(void);

    uint8_t INC_BC(void);
    uint8_t INC_DE(void);
    uint8_t INC_HL(void);
    uint8_t INC_SP(void);

    uint8_t INC_B(void);
    uint8_t INC_D(void);
    uint8_t INC_H(void);
    uint8_t INC_mHL(void);

    uint8_t DEC_B(void);
    uint8_t DEC_D(void);
    uint8_t DEC_H(void);
    uint8_t DEC_mHL(void);

    uint8_t LD_B_d8(void);
    uint8_t LD_D_d8(void);
    uint8_t LD_H_d8(void);
    uint8_t LD_mHL_d8(void);

    uint8_t RLCA(void);
    uint8_t RLA(void);
    uint8_t DAA(void);
    uint8_t SCF(void);

    uint8_t LD_a16_SP(void);
    uint8_t JR_s8(void);
    uint8_t JR_Z_s8(void);
    uint8_t JR_C_s8(void);

    uint8_t ADD_HL_BC(void);
    uint8_t ADD_HL_DE(void);
    uint8_t ADD_HL_HL(void);
    uint8_t ADD_HL_SP(void);

    uint8_t LD_A_mBC(void);
    uint8_t LD_A_mDE(void);
    uint8_t LD_A_mHLi(void);
    uint8_t LD_A_mHLd(void);

    uint8_t DEC_BC(void);
    uint8_t DEC_DE(void);
    uint8_t DEC_HL(void);
    uint8_t DEC_SP(void);

    uint8_t INC_C(void);
    uint8_t INC_E(void);
    uint8_t INC_L(void);
    uint8_t INC_A(void);

    uint8_t DEC_C(void);
    uint8_t DEC_E(void);
    uint8_t DEC_L(void);
    uint8_t DEC_A(void);

    uint8_t LD_C_d8(void);
    uint8_t LD_E_d8(void);
    uint8_t LD_L_d8(void);
    uint8_t LD_A_d8(void);

    uint8_t RRCA(void);
    uint8_t RRA(void);
    uint8_t CPL(void);
    uint8_t CCF(void);

    uint8_t LD_B_B(void);
    uint8_t LD_D_B(void);
    uint8_t LD_H_B(void);
    uint8_t LD_mHL_B(void);

    uint8_t LD_B_C(void);
    uint8_t LD_D_C(void);
    uint8_t LD_H_C(void);
    uint8_t LD_mHL_C(void);
    
    uint8_t LD_B_D(void);
    uint8_t LD_D_D(void);
    uint8_t LD_H_D(void);
    uint8_t LD_mHL_D(void);

    uint8_t LD_B_E(void);
    uint8_t LD_D_E(void);
    uint8_t LD_H_E(void);
    uint8_t LD_mHL_E(void);

    uint8_t LD_B_H(void);
    uint8_t LD_D_H(void);
    uint8_t LD_H_H(void);
    uint8_t LD_mHL_H(void);

    uint8_t LD_B_L(void);
    uint8_t LD_D_L(void);
    uint8_t LD_H_L(void);
    uint8_t LD_mHL_L(void);

    uint8_t LD_B_mHL(void);
    uint8_t LD_D_mHL(void);
    uint8_t LD_H_mHL(void);
    uint8_t HALT(void);

    uint8_t LD_B_A(void);
    uint8_t LD_D_A(void);
    uint8_t LD_H_A(void);
    uint8_t LD_mHL_A(void);

    uint8_t LD_C_B(void);
    uint8_t LD_E_B(void);
    uint8_t LD_L_B(void);
    uint8_t LD_A_B(void);

    uint8_t LD_C_C(void);
    uint8_t LD_E_C(void);
    uint8_t LD_L_C(void);
    uint8_t LD_A_C(void);

    uint8_t LD_C_D(void);
    uint8_t LD_E_D(void);
    uint8_t LD_L_D(void);
    uint8_t LD_A_D(void);

    uint8_t LD_C_E(void);
    uint8_t LD_E_E(void);
    uint8_t LD_L_E(void);
    uint8_t LD_A_E(void);

    uint8_t LD_C_H(void);
    uint8_t LD_E_H(void);
    uint8_t LD_L_H(void);
    uint8_t LD_A_H(void);

    uint8_t LD_C_L(void);
    uint8_t LD_E_L(void);
    uint8_t LD_L_L(void);
    uint8_t LD_A_L(void);

    uint8_t LD_C_mHL(void);
    uint8_t LD_E_mHL(void);
    uint8_t LD_L_mHL(void);
    uint8_t LD_A_mHL(void);

    uint8_t LD_C_A(void);
    uint8_t LD_E_A(void);
    uint8_t LD_L_A(void);
    uint8_t LD_A_A(void);

    uint8_t ADD_A_B(void);
    uint8_t ADD_A_C(void);
    uint8_t ADD_A_D(void);
    uint8_t ADD_A_E(void);
    uint8_t ADD_A_H(void);
    uint8_t ADD_A_L(void);
    uint8_t ADD_A_mHL(void);
    uint8_t ADD_A_A(void);

    uint8_t SUB_B(void);
    uint8_t SUB_C(void);
    uint8_t SUB_D(void);
    uint8_t SUB_E(void);
    uint8_t SUB_H(void);
    uint8_t SUB_L(void);
    uint8_t SUB_mHL(void);
    uint8_t SUB_A(void);

    uint8_t AND_B(void);
    uint8_t AND_C(void);
    uint8_t AND_D(void);
    uint8_t AND_E(void);
    uint8_t AND_H(void);
    uint8_t AND_L(void);
    uint8_t AND_mHL(void);
    uint8_t AND_A(void);

    uint8_t OR_B(void);
    uint8_t OR_C(void);
    uint8_t OR_D(void);
    uint8_t OR_E(void);
    uint8_t OR_H(void);
    uint8_t OR_L(void);
    uint8_t OR_mHL(void);
    uint8_t OR_A(void);

    uint8_t ADC_A_B(void);
    uint8_t ADC_A_C(void);
    uint8_t ADC_A_D(void);
    uint8_t ADC_A_E(void);
    uint8_t ADC_A_H(void);
    uint8_t ADC_A_L(void);
    uint8_t ADC_A_mHL(void);
    uint8_t ADC_A_A(void);

    uint8_t SBC_A_B(void);
    uint8_t SBC_A_C(void);
    uint8_t SBC_A_D(void);
    uint8_t SBC_A_E(void);
    uint8_t SBC_A_H(void);
    uint8_t SBC_A_L(void);
    uint8_t SBC_A_mHL(void);
    uint8_t SBC_A_A(void);

    uint8_t XOR_B(void);
    uint8_t XOR_C(void);
    uint8_t XOR_D(void);
    uint8_t XOR_E(void);
    uint8_t XOR_H(void);
    uint8_t XOR_L(void);
    uint8_t XOR_mHL(void);
    uint8_t XOR_A(void);

    uint8_t CP_B(void);
    uint8_t CP_C(void);
    uint8_t CP_D(void);
    uint8_t CP_E(void);
    uint8_t CP_H(void);
    uint8_t CP_L(void);
    uint8_t CP_mHL(void);
    uint8_t CP_A(void);

    uint8_t RET_NZ(void);
    uint8_t RET_NC(void);
    uint8_t LD_a8_A(void);
    uint8_t LD_A_a8(void);

    uint8_t POP_BC(void);
    uint8_t POP_DE(void);
    uint8_t POP_HL(void);
    uint8_t POP_AF(void);

    uint8_t JP_NZ_a16(void);
    uint8_t JP_NC_a16(void);
    uint8_t LD_mC_A(void);
    uint8_t LD_A_mC(void);

    uint8_t JP_a16(void);
    uint8_t DI(void);

    uint8_t CALL_NZ_a16(void);
    uint8_t CALL_NC_a16(void);

    uint8_t PUSH_BC(void);
    uint8_t PUSH_DE(void);
    uint8_t PUSH_HL(void);
    uint8_t PUSH_AF(void);

    uint8_t ADD_A_d8(void);
    uint8_t SUB_d8(void);
    uint8_t AND_d8(void);
    uint8_t OR_d8(void);

    uint8_t RST_0(void);
    uint8_t RST_2(void);
    uint8_t RST_4(void);
    uint8_t RST_6(void);

    uint8_t RET_Z(void);
    uint8_t RET_C(void);
    uint8_t ADD_SP_s8(void);
    uint8_t LD_HL_SP_s8(void);

    uint8_t RET(void);
    uint8_t RETI(void);
    uint8_t JP_HL(void);
    uint8_t LD_SP_HL(void);

    uint8_t JP_Z_a16(void);
    uint8_t JP_C_a16(void);
    uint8_t LD_a16_A(void);
    uint8_t LD_A_ma16(void);

    uint8_t EI(void);

    uint8_t CALL_Z_a16(void);
    uint8_t CALL_C_a16(void);

    uint8_t CALL_a16(void);

    uint8_t ADC_A_d8(void);
    uint8_t SBC_A_d8(void);
    uint8_t XOR_d8(void);
    uint8_t CP_d8(void);

    uint8_t RST_1(void);
    uint8_t RST_3(void);
    uint8_t RST_5(void);
    uint8_t RST_7(void);

    // cb opcodes
    uint8_t RLC(uint8_t&);
    uint8_t RLC_B(void);
    uint8_t RLC_C(void);
    uint8_t RLC_D(void);
    uint8_t RLC_E(void);
    uint8_t RLC_H(void);
    uint8_t RLC_L(void);
    uint8_t RLC_mHL(void);
    uint8_t RLC_A(void);

    uint8_t RRC(uint8_t&);
    uint8_t RRC_B(void);
    uint8_t RRC_C(void);
    uint8_t RRC_D(void);
    uint8_t RRC_E(void);
    uint8_t RRC_H(void);
    uint8_t RRC_L(void);
    uint8_t RRC_mHL(void);
    uint8_t RRC_A(void);

    uint8_t RL(uint8_t&);
    uint8_t RL_B(void);
    uint8_t RL_C(void);
    uint8_t RL_D(void);
    uint8_t RL_E(void);
    uint8_t RL_H(void);
    uint8_t RL_L(void);
    uint8_t RL_mHL(void);
    uint8_t RL_A(void);

    uint8_t RR(uint8_t&);
    uint8_t RR_B(void);
    uint8_t RR_C(void);
    uint8_t RR_D(void);
    uint8_t RR_E(void);
    uint8_t RR_H(void);
    uint8_t RR_L(void);
    uint8_t RR_mHL(void);
    uint8_t RR_A(void);

    uint8_t SLA(uint8_t&);
    uint8_t SLA_B(void);
    uint8_t SLA_C(void);
    uint8_t SLA_D(void);
    uint8_t SLA_E(void);
    uint8_t SLA_H(void);
    uint8_t SLA_L(void);
    uint8_t SLA_mHL(void);
    uint8_t SLA_A(void);

    uint8_t SRA(uint8_t&);
    uint8_t SRA_B(void);
    uint8_t SRA_C(void);
    uint8_t SRA_D(void);
    uint8_t SRA_E(void);
    uint8_t SRA_H(void);
    uint8_t SRA_L(void);
    uint8_t SRA_mHL(void);
    uint8_t SRA_A(void);

    uint8_t SWAP(uint8_t&);
    uint8_t SWAP_B(void);
    uint8_t SWAP_C(void);
    uint8_t SWAP_D(void);
    uint8_t SWAP_E(void);
    uint8_t SWAP_H(void);
    uint8_t SWAP_L(void);
    uint8_t SWAP_mHL(void);
    uint8_t SWAP_A(void);

    uint8_t SRL(uint8_t&);
    uint8_t SRL_B(void);
    uint8_t SRL_C(void);
    uint8_t SRL_D(void);
    uint8_t SRL_E(void);
    uint8_t SRL_H(void);
    uint8_t SRL_L(void);
    uint8_t SRL_mHL(void);
    uint8_t SRL_A(void);

    uint8_t BIT(uint8_t, uint8_t&);

    uint8_t BIT_0_B(void);
    uint8_t BIT_0_C(void);
    uint8_t BIT_0_D(void);
    uint8_t BIT_0_E(void);
    uint8_t BIT_0_H(void);
    uint8_t BIT_0_L(void);
    uint8_t BIT_0_mHL(void);
    uint8_t BIT_0_A(void);

    uint8_t BIT_1_B(void);
    uint8_t BIT_1_C(void);
    uint8_t BIT_1_D(void);
    uint8_t BIT_1_E(void);
    uint8_t BIT_1_H(void);
    uint8_t BIT_1_L(void);
    uint8_t BIT_1_mHL(void);
    uint8_t BIT_1_A(void);

    uint8_t BIT_2_B(void);
    uint8_t BIT_2_C(void);
    uint8_t BIT_2_D(void);
    uint8_t BIT_2_E(void);
    uint8_t BIT_2_H(void);
    uint8_t BIT_2_L(void);
    uint8_t BIT_2_mHL(void);
    uint8_t BIT_2_A(void);

    uint8_t BIT_3_B(void);
    uint8_t BIT_3_C(void);
    uint8_t BIT_3_D(void);
    uint8_t BIT_3_E(void);
    uint8_t BIT_3_H(void);
    uint8_t BIT_3_L(void);
    uint8_t BIT_3_mHL(void);
    uint8_t BIT_3_A(void);

    uint8_t BIT_4_B(void);
    uint8_t BIT_4_C(void);
    uint8_t BIT_4_D(void);
    uint8_t BIT_4_E(void);
    uint8_t BIT_4_H(void);
    uint8_t BIT_4_L(void);
    uint8_t BIT_4_mHL(void);
    uint8_t BIT_4_A(void);

    uint8_t BIT_5_B(void);
    uint8_t BIT_5_C(void);
    uint8_t BIT_5_D(void);
    uint8_t BIT_5_E(void);
    uint8_t BIT_5_H(void);
    uint8_t BIT_5_L(void);
    uint8_t BIT_5_mHL(void);
    uint8_t BIT_5_A(void);

    uint8_t BIT_6_B(void);
    uint8_t BIT_6_C(void);
    uint8_t BIT_6_D(void);
    uint8_t BIT_6_E(void);
    uint8_t BIT_6_H(void);
    uint8_t BIT_6_L(void);
    uint8_t BIT_6_mHL(void);
    uint8_t BIT_6_A(void);

    uint8_t BIT_7_B(void);
    uint8_t BIT_7_C(void);
    uint8_t BIT_7_D(void);
    uint8_t BIT_7_E(void);
    uint8_t BIT_7_H(void);
    uint8_t BIT_7_L(void);
    uint8_t BIT_7_mHL(void);
    uint8_t BIT_7_A(void);

    uint8_t RES(uint8_t, uint8_t&);

    uint8_t RES_0_B(void);
    uint8_t RES_0_C(void);
    uint8_t RES_0_D(void);
    uint8_t RES_0_E(void);
    uint8_t RES_0_H(void);
    uint8_t RES_0_L(void);
    uint8_t RES_0_mHL(void);
    uint8_t RES_0_A(void);

    uint8_t RES_1_B(void);
    uint8_t RES_1_C(void);
    uint8_t RES_1_D(void);
    uint8_t RES_1_E(void);
    uint8_t RES_1_H(void);
    uint8_t RES_1_L(void);
    uint8_t RES_1_mHL(void);
    uint8_t RES_1_A(void);

    uint8_t RES_2_B(void);
    uint8_t RES_2_C(void);
    uint8_t RES_2_D(void);
    uint8_t RES_2_E(void);
    uint8_t RES_2_H(void);
    uint8_t RES_2_L(void);
    uint8_t RES_2_mHL(void);
    uint8_t RES_2_A(void);

    uint8_t RES_3_B(void);
    uint8_t RES_3_C(void);
    uint8_t RES_3_D(void);
    uint8_t RES_3_E(void);
    uint8_t RES_3_H(void);
    uint8_t RES_3_L(void);
    uint8_t RES_3_mHL(void);
    uint8_t RES_3_A(void);

    uint8_t RES_4_B(void);
    uint8_t RES_4_C(void);
    uint8_t RES_4_D(void);
    uint8_t RES_4_E(void);
    uint8_t RES_4_H(void);
    uint8_t RES_4_L(void);
    uint8_t RES_4_mHL(void);
    uint8_t RES_4_A(void);

    uint8_t RES_5_B(void);
    uint8_t RES_5_C(void);
    uint8_t RES_5_D(void);
    uint8_t RES_5_E(void);
    uint8_t RES_5_H(void);
    uint8_t RES_5_L(void);
    uint8_t RES_5_mHL(void);
    uint8_t RES_5_A(void);

    uint8_t RES_6_B(void);
    uint8_t RES_6_C(void);
    uint8_t RES_6_D(void);
    uint8_t RES_6_E(void);
    uint8_t RES_6_H(void);
    uint8_t RES_6_L(void);
    uint8_t RES_6_mHL(void);
    uint8_t RES_6_A(void);

    uint8_t RES_7_B(void);
    uint8_t RES_7_C(void);
    uint8_t RES_7_D(void);
    uint8_t RES_7_E(void);
    uint8_t RES_7_H(void);
    uint8_t RES_7_L(void);
    uint8_t RES_7_mHL(void);
    uint8_t RES_7_A(void);

    uint8_t SET(uint8_t, uint8_t&);

    uint8_t SET_0_B(void);
    uint8_t SET_0_C(void);
    uint8_t SET_0_D(void);
    uint8_t SET_0_E(void);
    uint8_t SET_0_H(void);
    uint8_t SET_0_L(void);
    uint8_t SET_0_mHL(void);
    uint8_t SET_0_A(void);

    uint8_t SET_1_B(void);
    uint8_t SET_1_C(void);
    uint8_t SET_1_D(void);
    uint8_t SET_1_E(void);
    uint8_t SET_1_H(void);
    uint8_t SET_1_L(void);
    uint8_t SET_1_mHL(void);
    uint8_t SET_1_A(void);

    uint8_t SET_2_B(void);
    uint8_t SET_2_C(void);
    uint8_t SET_2_D(void);
    uint8_t SET_2_E(void);
    uint8_t SET_2_H(void);
    uint8_t SET_2_L(void);
    uint8_t SET_2_mHL(void);
    uint8_t SET_2_A(void);

    uint8_t SET_3_B(void);
    uint8_t SET_3_C(void);
    uint8_t SET_3_D(void);
    uint8_t SET_3_E(void);
    uint8_t SET_3_H(void);
    uint8_t SET_3_L(void);
    uint8_t SET_3_mHL(void);
    uint8_t SET_3_A(void);

    uint8_t SET_4_B(void);
    uint8_t SET_4_C(void);
    uint8_t SET_4_D(void);
    uint8_t SET_4_E(void);
    uint8_t SET_4_H(void);
    uint8_t SET_4_L(void);
    uint8_t SET_4_mHL(void);
    uint8_t SET_4_A(void);

    uint8_t SET_5_B(void);
    uint8_t SET_5_C(void);
    uint8_t SET_5_D(void);
    uint8_t SET_5_E(void);
    uint8_t SET_5_H(void);
    uint8_t SET_5_L(void);
    uint8_t SET_5_mHL(void);
    uint8_t SET_5_A(void);

    uint8_t SET_6_B(void);
    uint8_t SET_6_C(void);
    uint8_t SET_6_D(void);
    uint8_t SET_6_E(void);
    uint8_t SET_6_H(void);
    uint8_t SET_6_L(void);
    uint8_t SET_6_mHL(void);
    uint8_t SET_6_A(void);

    uint8_t SET_7_B(void);
    uint8_t SET_7_C(void);
    uint8_t SET_7_D(void);
    uint8_t SET_7_E(void);
    uint8_t SET_7_H(void);
    uint8_t SET_7_L(void);
    uint8_t SET_7_mHL(void);
    uint8_t SET_7_A(void);

    RegisterPair AF, BC, DE, HL; // general use registers
    uint16_t SP = 0xFFFE;
    uint16_t PC = 0x0000; // PSW registers

    // instruction set
    OpcodeImpl instrTable[256];
    OpcodeImpl cbInstrTable[256];
};

} // namespace GB2040::Core

