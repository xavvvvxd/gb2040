#include "core/cpu.h"
#include "core/console.h"

#include <cstdint>

namespace GB2040::Core
{

uint8_t CPU::RLCA(void) {
    RLC(AF.hi); // use helper from CB opcodes

    setFlag(ZF, false);
    setFlag(NF, false);
    setFlag(HF, false);

    return 1;
}

uint8_t CPU::RLA(void) {
    RL(AF.hi);

    setFlag(ZF, false);
    setFlag(NF, false);
    setFlag(HF, false);

    return 1;
}

uint8_t CPU::RRCA(void) {
    RRC(AF.hi); // use helper from CB opcodes

    setFlag(ZF, false);
    setFlag(NF, false);
    setFlag(HF, false);

    return 1;
}

uint8_t CPU::RRA(void) {
    RR(AF.hi);

    setFlag(ZF, false);
    setFlag(NF, false);
    setFlag(HF, false);

    return 1;
}

uint8_t CPU::AND_r8(uint8_t& reg) {
    uint8_t val = AF.hi & reg;
    AF.hi = val;

    setFlag(ZF, val == 0);
    setFlag(NF, false);
    setFlag(HF, true);
    setFlag(CF, false);

    return 1;
}

uint8_t CPU::AND_B(void) { return AND_r8(BC.hi); }
uint8_t CPU::AND_C(void) { return AND_r8(BC.lo); }
uint8_t CPU::AND_D(void) { return AND_r8(DE.hi); }
uint8_t CPU::AND_E(void) { return AND_r8(DE.lo); }
uint8_t CPU::AND_H(void) { return AND_r8(HL.hi); }
uint8_t CPU::AND_L(void) { return AND_r8(HL.lo); }
uint8_t CPU::AND_mHL() {
    uint8_t val = mmu->read8(HL.get());
    AND_r8(val);
    return 2;
}
uint8_t CPU::AND_A(void) { return AND_r8(AF.hi); }

uint8_t CPU::OR_r8(uint8_t& reg) {
    uint8_t val = AF.hi | reg;
    AF.hi = val;

    setFlag(ZF, val == 0);
    setFlag(NF, false);
    setFlag(HF, false);
    setFlag(CF, false);

    return 1;
}

uint8_t CPU::OR_B(void) { return OR_r8(BC.hi); }
uint8_t CPU::OR_C(void) { return OR_r8(BC.lo); }
uint8_t CPU::OR_D(void) { return OR_r8(DE.hi); }
uint8_t CPU::OR_E(void) { return OR_r8(DE.lo); }
uint8_t CPU::OR_H(void) { return OR_r8(HL.hi); }
uint8_t CPU::OR_L(void) { return OR_r8(HL.lo); }
uint8_t CPU::OR_mHL(void) {
    uint8_t val = mmu->read8(HL.get());
    OR_r8(val);
    return 2;
}
uint8_t CPU::OR_A(void) { return OR_r8(AF.hi); }

uint8_t CPU::XOR_r8(uint8_t& reg) {
    uint8_t val = AF.hi ^ reg;
    AF.hi = val;

    setFlag(ZF, val == 0);
    setFlag(NF, false);
    setFlag(HF, false);
    setFlag(CF, false);

    return 1;
}

uint8_t CPU::XOR_B(void) { return XOR_r8(BC.hi); }
uint8_t CPU::XOR_C(void) { return XOR_r8(BC.lo); }
uint8_t CPU::XOR_D(void) { return XOR_r8(DE.hi); }
uint8_t CPU::XOR_E(void) { return XOR_r8(DE.lo); }
uint8_t CPU::XOR_H(void) { return XOR_r8(HL.hi); }
uint8_t CPU::XOR_L(void) { return XOR_r8(HL.lo); }
uint8_t CPU::XOR_mHL(void) {
    uint8_t val = mmu->read8(HL.get());
    XOR_r8(val);
    return 2;
}
uint8_t CPU::XOR_A(void) { return XOR_r8(AF.hi); }

uint8_t CPU::AND_d8(void) {
    uint8_t imm = fetch8();
    AND_r8(imm);

    return 2;
}

uint8_t CPU::OR_d8(void) {
    uint8_t imm = fetch8();
    OR_r8(imm);

    return 2;
}

uint8_t CPU::XOR_d8(void) {
    uint8_t imm = fetch8();
    XOR_r8(imm);

    return 2;
}

uint8_t CPU::CP_d8(void) {
    uint8_t imm = fetch8();
    CP_r8(imm);

    return 2;
}

} // namespace GB2040::Core
