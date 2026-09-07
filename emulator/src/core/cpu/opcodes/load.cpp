#include "core/cpu.h"
#include "core/console.h"

#include <cstdint>

namespace GB2040::Core
{

uint8_t CPU::LD_r16_d16(RegisterPair& reg) {
    uint16_t imm = fetch16();
    reg.set(imm);

    return 3;
}

uint8_t CPU::LD_BC_d16(void) { return LD_r16_d16(BC); }
uint8_t CPU::LD_DE_d16(void) { return LD_r16_d16(DE); }
uint8_t CPU::LD_HL_d16(void) { return LD_r16_d16(HL); }
uint8_t CPU::LD_SP_d16(void) {
    uint16_t imm = fetch16();
    SP = imm;

    return 3;
}

uint8_t CPU::LD_mr16_A(RegisterPair& reg, int8_t offset) {
    uint16_t addr = reg.get();

    mmu->write8(addr, AF.hi);
    reg.set(addr + offset);

    return 2;
}

uint8_t CPU::LD_mBC_A(void) { return LD_mr16_A(BC); }
uint8_t CPU::LD_mDE_A(void) { return LD_mr16_A(DE); }
uint8_t CPU::LD_mHLi_A(void) { return LD_mr16_A(HL, 1); }
uint8_t CPU::LD_mHLd_A(void) { return LD_mr16_A(HL, -1); }

uint8_t CPU::LD_r8_d8(uint8_t& reg) {
    uint8_t imm = fetch8();

    reg = imm;
    return 2;
}

uint8_t CPU::LD_B_d8(void) { return LD_r8_d8(BC.hi); }
uint8_t CPU::LD_D_d8(void) { return LD_r8_d8(DE.hi); }
uint8_t CPU::LD_H_d8(void) { return LD_r8_d8(HL.hi); }
uint8_t CPU::LD_mHL_d8(void) { 
    uint8_t val;
    LD_r8_d8(val);
    mmu->write8(HL.get(), val);

    return 3;
}

uint8_t CPU::LD_a16_SP(void) {
    mmu->write16(fetch16(), SP);

    return 5;
}

uint8_t CPU::LD_A_mr16(RegisterPair& reg, int8_t offset) {
    uint16_t addr = reg.get();

    AF.hi = mmu->read8(addr);
    reg.set(addr + offset);

    return 2;
}

uint8_t CPU::LD_A_mBC(void) { return LD_A_mr16(BC); }
uint8_t CPU::LD_A_mDE(void) { return LD_A_mr16(DE); }
uint8_t CPU::LD_A_mHLi(void) { return LD_A_mr16(HL, 1); }
uint8_t CPU::LD_A_mHLd(void) { return LD_A_mr16(HL, -1); }

uint8_t CPU::LD_C_d8(void) { return LD_r8_d8(BC.lo); }
uint8_t CPU::LD_E_d8(void) { return LD_r8_d8(DE.lo); }
uint8_t CPU::LD_L_d8(void) { return LD_r8_d8(HL.lo); }
uint8_t CPU::LD_A_d8(void) { return LD_r8_d8(AF.hi); }

uint8_t CPU::LD_r8_r8(uint8_t& reg1, uint8_t& reg2) {
    reg1 = reg2;

    return 1;
}

uint8_t CPU::LD_r8_mHL(uint8_t& reg) {
    reg = mmu->read8(HL.get());

    return 2;
}

uint8_t CPU::LD_mHL_r8(uint8_t& reg) {
    mmu->write8(HL.get(), reg);

    return 2;
}

uint8_t CPU::LD_B_B(void) { return LD_r8_r8(BC.hi, BC.hi); }
uint8_t CPU::LD_D_B(void) { return LD_r8_r8(DE.hi, BC.hi); }
uint8_t CPU::LD_H_B(void) { return LD_r8_r8(HL.hi, BC.hi); }
uint8_t CPU::LD_mHL_B(void) { return LD_mHL_r8(BC.hi); }

uint8_t CPU::LD_B_C(void) { return LD_r8_r8(BC.hi, BC.lo); }
uint8_t CPU::LD_D_C(void) { return LD_r8_r8(DE.hi, BC.lo); }
uint8_t CPU::LD_H_C(void) { return LD_r8_r8(HL.hi, BC.lo); }
uint8_t CPU::LD_mHL_C(void) { return LD_mHL_r8(BC.lo); }

uint8_t CPU::LD_B_D(void) { return LD_r8_r8(BC.hi, DE.hi); }
uint8_t CPU::LD_D_D(void) { return LD_r8_r8(DE.hi, DE.hi); }
uint8_t CPU::LD_H_D(void) { return LD_r8_r8(HL.hi, DE.hi); }
uint8_t CPU::LD_mHL_D(void) { return LD_mHL_r8(DE.hi); }

uint8_t CPU::LD_B_E(void) { return LD_r8_r8(BC.hi, DE.lo); }
uint8_t CPU::LD_D_E(void) { return LD_r8_r8(DE.hi, DE.lo); }
uint8_t CPU::LD_H_E(void) { return LD_r8_r8(HL.hi, DE.lo); }
uint8_t CPU::LD_mHL_E(void) { return LD_mHL_r8(DE.lo); }

uint8_t CPU::LD_B_H(void) { return LD_r8_r8(BC.hi, HL.hi); }
uint8_t CPU::LD_D_H(void) { return LD_r8_r8(DE.hi, HL.hi); }
uint8_t CPU::LD_H_H(void) { return LD_r8_r8(HL.hi, HL.hi); }
uint8_t CPU::LD_mHL_H(void) { return LD_mHL_r8(HL.hi); }

uint8_t CPU::LD_B_L(void) { return LD_r8_r8(BC.hi, HL.lo); }
uint8_t CPU::LD_D_L(void) { return LD_r8_r8(DE.hi, HL.lo); }
uint8_t CPU::LD_H_L(void) { return LD_r8_r8(HL.hi, HL.lo); }
uint8_t CPU::LD_mHL_L(void) { return LD_mHL_r8(HL.lo); }

uint8_t CPU::LD_B_mHL(void) { return LD_r8_mHL(BC.hi); }
uint8_t CPU::LD_D_mHL(void) { return LD_r8_mHL(DE.hi); }
uint8_t CPU::LD_H_mHL(void) { return LD_r8_mHL(HL.hi); }

uint8_t CPU::LD_B_A(void) { return LD_r8_r8(BC.hi, AF.hi); }
uint8_t CPU::LD_D_A(void) { return LD_r8_r8(DE.hi, AF.hi); }
uint8_t CPU::LD_H_A(void) { return LD_r8_r8(HL.hi, AF.hi); }
uint8_t CPU::LD_mHL_A(void) { return LD_mHL_r8(AF.hi); }

uint8_t CPU::LD_C_B(void) { return LD_r8_r8(BC.lo, BC.hi); }
uint8_t CPU::LD_E_B(void) { return LD_r8_r8(DE.lo, BC.hi); }
uint8_t CPU::LD_L_B(void) { return LD_r8_r8(HL.lo, BC.hi); }
uint8_t CPU::LD_A_B(void) { return LD_r8_r8(AF.hi, BC.hi); }

uint8_t CPU::LD_C_C(void) { return LD_r8_r8(BC.lo, BC.lo); }
uint8_t CPU::LD_E_C(void) { return LD_r8_r8(DE.lo, BC.lo); }
uint8_t CPU::LD_L_C(void) { return LD_r8_r8(HL.lo, BC.lo); }
uint8_t CPU::LD_A_C(void) { return LD_r8_r8(AF.hi, BC.lo); }

uint8_t CPU::LD_C_D(void) { return LD_r8_r8(BC.lo, DE.hi); }
uint8_t CPU::LD_E_D(void) { return LD_r8_r8(DE.lo, DE.hi); }
uint8_t CPU::LD_L_D(void) { return LD_r8_r8(HL.lo, DE.hi); }
uint8_t CPU::LD_A_D(void) { return LD_r8_r8(AF.hi, DE.hi); }

uint8_t CPU::LD_C_E(void) { return LD_r8_r8(BC.lo, DE.lo); }
uint8_t CPU::LD_E_E(void) { return LD_r8_r8(DE.lo, DE.lo); }
uint8_t CPU::LD_L_E(void) { return LD_r8_r8(HL.lo, DE.lo); }
uint8_t CPU::LD_A_E(void) { return LD_r8_r8(AF.hi, DE.lo); }

uint8_t CPU::LD_C_H(void) { return LD_r8_r8(BC.lo, HL.hi); }
uint8_t CPU::LD_E_H(void) { return LD_r8_r8(DE.lo, HL.hi); }
uint8_t CPU::LD_L_H(void) { return LD_r8_r8(HL.lo, HL.hi); }
uint8_t CPU::LD_A_H(void) { return LD_r8_r8(AF.hi, HL.hi); }

uint8_t CPU::LD_C_L(void) { return LD_r8_r8(BC.lo, HL.lo); }
uint8_t CPU::LD_E_L(void) { return LD_r8_r8(DE.lo, HL.lo); }
uint8_t CPU::LD_L_L(void) { return LD_r8_r8(HL.lo, HL.lo); }
uint8_t CPU::LD_A_L(void) { return LD_r8_r8(AF.hi, HL.lo); }

uint8_t CPU::LD_C_mHL(void) { return LD_r8_mHL(BC.lo); }
uint8_t CPU::LD_E_mHL(void) { return LD_r8_mHL(DE.lo); }
uint8_t CPU::LD_L_mHL(void) { return LD_r8_mHL(HL.lo); }
uint8_t CPU::LD_A_mHL(void) { return LD_r8_mHL(AF.hi); }

uint8_t CPU::LD_C_A(void) { return LD_r8_r8(BC.lo, AF.hi); }
uint8_t CPU::LD_E_A(void) { return LD_r8_r8(DE.lo, AF.hi); }
uint8_t CPU::LD_L_A(void) { return LD_r8_r8(HL.lo, AF.hi); }
uint8_t CPU::LD_A_A(void) { return LD_r8_r8(AF.hi, AF.hi); }

uint8_t CPU::LD_a8_A(void) {
    uint8_t addr = fetch8();
    mmu->write8(addr + 0xFF00, AF.hi);

    return 3;
}

uint8_t CPU::LD_A_a8(void) {
    uint8_t addr = fetch8();
    AF.hi = mmu->read8(addr + 0xFF00);

    return 3;
}

uint8_t CPU::LD_mC_A(void) {
    uint8_t addr = BC.lo;
    mmu->write8(addr + 0xFF00, AF.hi);
    
    return 2;
}

uint8_t CPU::LD_A_mC(void) {
    uint8_t addr = BC.lo;
    AF.hi = mmu->read8(addr + 0xFF00);

    return 2;
}

uint8_t CPU::LD_HL_SP_s8(void) {
    int8_t s8 = static_cast<int8_t>(fetch8());

    bool hCarry = ((SP & 0x0F) + (s8 & 0x0F)) > 0xF;
    bool carry = ((SP & 0xFF) + (s8 & 0xFF)) > 0xFF;
    uint16_t val = SP + s8;
    HL.set(val);

    setFlag(ZF, false);
    setFlag(NF, false);
    setFlag(HF, hCarry);
    setFlag(CF, carry);

    return 3;
}

uint8_t CPU::LD_SP_HL(void) {
    SP = HL.get();

    return 2;
}

uint8_t CPU::LD_a16_A(void) {
    uint16_t addr = fetch16();
    mmu->write8(addr, AF.hi);

    return 4;
}

uint8_t CPU::LD_A_ma16(void) {
    uint16_t addr = fetch16();
    AF.hi = mmu->read8(addr);

    return 4;
}

} // namespace GB2040::Core
