#include "core/cpu.h"
#include "core/console.h"

#include <cstdint>

namespace GB2040::Core
{

uint8_t CPU::INC_r16(RegisterPair& reg) {
    reg.set(reg.get() + 1);

    return 2;
}

uint8_t CPU::INC_BC(void) { return INC_r16(BC); }
uint8_t CPU::INC_DE(void) { return INC_r16(DE); }
uint8_t CPU::INC_HL(void) { return INC_r16(HL); }
uint8_t CPU::INC_SP(void) {
    SP++;

    return 2;
}

uint8_t CPU::DEC_r16(RegisterPair& reg) {
    reg.set(reg.get() - 1);

    return 2;
}

uint8_t CPU::DEC_BC(void) { return DEC_r16(BC); }
uint8_t CPU::DEC_DE(void) { return DEC_r16(DE); }
uint8_t CPU::DEC_HL(void) { return DEC_r16(HL); }
uint8_t CPU::DEC_SP(void) {
    SP--;

    return 2;
}

uint8_t CPU::INC_r8(uint8_t& reg) {
    setFlag(HF, (reg & 0x0F) + 1 > 0x0F); // check if nibble will overflow

    reg++;

    setFlag(ZF, reg == 0);
    setFlag(NF, false);
    // leave CF

    return 1;
}

uint8_t CPU::INC_B(void) { return INC_r8(BC.hi); }
uint8_t CPU::INC_D(void) { return INC_r8(DE.hi); }
uint8_t CPU::INC_H(void) { return INC_r8(HL.hi); }
uint8_t CPU::INC_mHL(void) {
    uint8_t val = console.mmu.read8(HL.get());
    INC_r8(val);

    console.mmu.write8(HL.get(), val);

    return 3;
}

uint8_t CPU::DEC_r8(uint8_t& reg) {
    setFlag(HF, (reg & 0x0F) == 0x00); // check if underflow

    reg--;

    setFlag(ZF, reg == 0);
    setFlag(NF, true);
    // leave CF

    return 1;
}

uint8_t CPU::DEC_B(void) { return DEC_r8(BC.hi); }
uint8_t CPU::DEC_D(void) { return DEC_r8(DE.hi); }
uint8_t CPU::DEC_H(void) { return DEC_r8(HL.hi); }
uint8_t CPU::DEC_mHL(void) {
    uint8_t val = console.mmu.read8(HL.get());
    DEC_r8(val);

    console.mmu.write8(HL.get(), val);

    return 3;
}

uint8_t CPU::ADD_HL_r16(RegisterPair& reg) { // store in reg1
    bool hCarry = (HL.get() & 0x0FFF) + (reg.get() & 0x0FFF) > 0x0FFF;
    bool carry = ((uint32_t)HL.get() + (uint32_t)reg.get()) > 0xFFFF;
    uint16_t val = HL.get() + reg.get();
    HL.set(val);

    setFlag(NF, false);
    setFlag(HF, hCarry);
    setFlag(CF, carry);

    return 2;
}

uint8_t CPU::ADD_HL_BC(void) { return ADD_HL_r16(BC); }
uint8_t CPU::ADD_HL_DE(void) { return ADD_HL_r16(DE); }
uint8_t CPU::ADD_HL_HL(void) { return ADD_HL_r16(HL); }
uint8_t CPU::ADD_HL_SP(void) {
    bool hCarry = (HL.get() & 0x0FFF) + (SP & 0x0FFF) > 0x0FFF;
    bool carry = ((uint32_t)HL.get() + (uint32_t)SP) > 0xFFFF;
    uint16_t val = HL.get() + SP;
    HL.set(val);

    setFlag(NF, false);
    setFlag(HF, hCarry);
    setFlag(CF, carry);

    return 2;
}

uint8_t CPU::INC_C(void) { return INC_r8(BC.lo); }
uint8_t CPU::INC_E(void) { return INC_r8(DE.lo); }
uint8_t CPU::INC_L(void) { return INC_r8(HL.lo); }
uint8_t CPU::INC_A(void) { return INC_r8(AF.hi); }

uint8_t CPU::DEC_C(void) { return DEC_r8(BC.lo); }
uint8_t CPU::DEC_E(void) { return DEC_r8(DE.lo); }
uint8_t CPU::DEC_L(void) { return DEC_r8(HL.lo); }
uint8_t CPU::DEC_A(void) { return DEC_r8(AF.hi); }

uint8_t CPU::ADD_A_r8(uint8_t& reg) {
    setFlag(HF, ((AF.hi & 0x0F) + (reg & 0x0F)) > 0x0F);
    setFlag(CF, (uint16_t)AF.hi + (uint16_t)reg > 0xFF);
    uint8_t val = AF.hi + reg;
    AF.hi = val;

    setFlag(ZF, val == 0);
    setFlag(NF, false);

    return 1;
}

uint8_t CPU::ADD_A_B(void) { return ADD_A_r8(BC.hi); }
uint8_t CPU::ADD_A_C(void) { return ADD_A_r8(BC.lo); }
uint8_t CPU::ADD_A_D(void) { return ADD_A_r8(DE.hi); }
uint8_t CPU::ADD_A_E(void) { return ADD_A_r8(DE.lo); }
uint8_t CPU::ADD_A_H(void) { return ADD_A_r8(HL.hi); }
uint8_t CPU::ADD_A_L(void) { return ADD_A_r8(HL.lo); }
uint8_t CPU::ADD_A_mHL(void) {
    uint8_t val = console.mmu.read8(HL.get());
    ADD_A_r8(val);

    return 2; 
}

uint8_t CPU::ADD_A_A(void) { return ADD_A_r8(AF.hi); }

uint8_t CPU::SUB_r8(uint8_t& reg) {
    setFlag(HF, (AF.hi & 0x0F) < (reg & 0x0F));
    setFlag(CF, AF.hi < reg);

    uint8_t val = AF.hi - reg;
    AF.hi = val;

    
    setFlag(ZF, val == 0);
    setFlag(NF, true);

    return 1;
}

uint8_t CPU::SUB_B(void) { return SUB_r8(BC.hi); }
uint8_t CPU::SUB_C(void) { return SUB_r8(BC.lo); }
uint8_t CPU::SUB_D(void) { return SUB_r8(DE.hi); }
uint8_t CPU::SUB_E(void) { return SUB_r8(DE.lo); }
uint8_t CPU::SUB_H(void) { return SUB_r8(HL.hi); }
uint8_t CPU::SUB_L(void) { return SUB_r8(HL.lo); }
uint8_t CPU::SUB_mHL(void) {
    uint8_t val = console.mmu.read8(HL.get());
    SUB_r8(val);
    return 2;
}
uint8_t CPU::SUB_A(void) { return SUB_r8(AF.hi); }

uint8_t CPU::ADC_A_r8(uint8_t& reg) {
    bool carry = getFlag(CF);
    setFlag(HF, (AF.hi & 0x0F) + (reg & 0x0F) + carry > 0x0F);
    setFlag(CF, (uint16_t)AF.hi + (uint16_t)reg + carry > 0xFF);

    uint8_t val = AF.hi + reg + carry;
    AF.hi = val;

    setFlag(ZF, val == 0);
    setFlag(NF, false);

    return 1;
}

uint8_t CPU::ADC_A_B(void) { return ADC_A_r8(BC.hi); }
uint8_t CPU::ADC_A_C(void) { return ADC_A_r8(BC.lo); }
uint8_t CPU::ADC_A_D(void) { return ADC_A_r8(DE.hi); }
uint8_t CPU::ADC_A_E(void) { return ADC_A_r8(DE.lo); }
uint8_t CPU::ADC_A_H(void) { return ADC_A_r8(HL.hi); }
uint8_t CPU::ADC_A_L(void) { return ADC_A_r8(HL.lo); }
uint8_t CPU::ADC_A_mHL(void) {
    uint8_t val = console.mmu.read8(HL.get());
    ADC_A_r8(val);
    return 2;
}
uint8_t CPU::ADC_A_A(void) { return ADC_A_r8(AF.hi); }

uint8_t CPU::SBC_A_r8(uint8_t& reg) {
    bool carry = getFlag(CF);
    setFlag(HF, (AF.hi & 0x0F) < ((reg & 0x0F) + carry));
    setFlag(CF, AF.hi < (uint16_t)reg + carry);

    uint8_t val = AF.hi - reg - carry;
    AF.hi = val;
    
    setFlag(ZF, val == 0);
    setFlag(NF, true);

    return 1;
}

uint8_t CPU::SBC_A_B(void) { return SBC_A_r8(BC.hi); }
uint8_t CPU::SBC_A_C(void) { return SBC_A_r8(BC.lo); }
uint8_t CPU::SBC_A_D(void) { return SBC_A_r8(DE.hi); }
uint8_t CPU::SBC_A_E(void) { return SBC_A_r8(DE.lo); }
uint8_t CPU::SBC_A_H(void) { return SBC_A_r8(HL.hi); }
uint8_t CPU::SBC_A_L(void) { return SBC_A_r8(HL.lo); }
uint8_t CPU::SBC_A_mHL(void) {
    uint8_t val = console.mmu.read8(HL.get());
    SBC_A_r8(val);
    return 2;
}
uint8_t CPU::SBC_A_A(void) { return SBC_A_r8(AF.hi); }

uint8_t CPU::CP_r8(uint8_t& reg) {
    uint8_t a = AF.hi;
    setFlag(HF, (a & 0x0F) < (reg & 0x0F));
    setFlag(CF, a < reg);
    uint8_t val = a - reg;
    setFlag(ZF, val == 0);
    setFlag(NF, true);

    return 1;
}

uint8_t CPU::CP_B(void) { return CP_r8(BC.hi); }
uint8_t CPU::CP_C(void) { return CP_r8(BC.lo); }
uint8_t CPU::CP_D(void) { return CP_r8(DE.hi); }
uint8_t CPU::CP_E(void) { return CP_r8(DE.lo); }
uint8_t CPU::CP_H(void) { return CP_r8(HL.hi); }
uint8_t CPU::CP_L(void) { return CP_r8(HL.lo); }
uint8_t CPU::CP_mHL(void) {
    uint8_t val = console.mmu.read8(HL.get());
    CP_r8(val);
    return 2;
}
uint8_t CPU::CP_A(void) { return CP_r8(AF.hi); }

uint8_t CPU::ADD_A_d8(void) {
    uint8_t imm = fetch8();
    ADD_A_r8(imm);

    return 2;
}

uint8_t CPU::SUB_d8(void) {
    uint8_t imm = fetch8();
    SUB_r8(imm);

    return 2;
}

uint8_t CPU::ADD_SP_s8(void) {
    int8_t s8 = static_cast<int8_t>(fetch8());

    bool hCarry = ((SP & 0x0F) + (s8 & 0x0F)) > 0xF;
    bool carry = ((SP & 0xFF) + (s8 & 0xFF)) > 0xFF;
    uint16_t val = SP + s8;
    SP = val;

    setFlag(ZF, false);
    setFlag(NF, false);
    setFlag(HF, hCarry);
    setFlag(CF, carry);

    return 4;
}

uint8_t CPU::ADC_A_d8(void) {
    uint8_t imm = fetch8();
    ADC_A_r8(imm);

    return 2;
}

uint8_t CPU::SBC_A_d8(void) {
    uint8_t imm = fetch8();
    SBC_A_r8(imm);

    return 2;
}

} // namespace GB2040::Core
