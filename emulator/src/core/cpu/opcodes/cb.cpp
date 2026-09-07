#include "core/cpu.h"
#include "core/console.h"

#include <cstdint>

namespace GB2040::Core
{

uint8_t CPU::RLC(uint8_t& reg) {
    bool bit7 = (reg >> 7) & 1;

    reg = (reg << 1) | bit7;

    setFlag(ZF, reg == 0);
    setFlag(HF, false);
    setFlag(NF, false);
    setFlag(CF, bit7);

    return 2;
}

uint8_t CPU::RLC_B(void) { return RLC(BC.hi); }
uint8_t CPU::RLC_C(void) { return RLC(BC.lo); }
uint8_t CPU::RLC_D(void) { return RLC(DE.hi); }
uint8_t CPU::RLC_E(void) { return RLC(DE.lo); }
uint8_t CPU::RLC_H(void) { return RLC(HL.hi); }
uint8_t CPU::RLC_L(void) { return RLC(HL.lo); }

uint8_t CPU::RLC_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    RLC(value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::RLC_A(void) { return RLC(AF.hi); }

uint8_t CPU::RRC(uint8_t& reg) {
    bool bit0 = reg & 1;

    reg = (reg >> 1) | (bit0 << 7);

    setFlag(ZF, reg == 0);
    setFlag(HF, false);
    setFlag(NF, false);
    setFlag(CF, bit0);

    return 2;
}

uint8_t CPU::RRC_B(void) { return RRC(BC.hi); }
uint8_t CPU::RRC_C(void) { return RRC(BC.lo); }
uint8_t CPU::RRC_D(void) { return RRC(DE.hi); }
uint8_t CPU::RRC_E(void) { return RRC(DE.lo); }
uint8_t CPU::RRC_H(void) { return RRC(HL.hi); }
uint8_t CPU::RRC_L(void) { return RRC(HL.lo); }

uint8_t CPU::RRC_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    RRC(value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::RRC_A(void) { return RRC(AF.hi); }

uint8_t CPU::RL(uint8_t& reg) {
    bool bit7 = (reg >> 7) & 1;

    reg = (reg << 1) | getFlag(CF);

    setFlag(ZF, reg == 0);
    setFlag(HF, false);
    setFlag(NF, false);
    setFlag(CF, bit7);

    return 2;
}

uint8_t CPU::RL_B(void) { return RL(BC.hi); }
uint8_t CPU::RL_C(void) { return RL(BC.lo); }
uint8_t CPU::RL_D(void) { return RL(DE.hi); }
uint8_t CPU::RL_E(void) { return RL(DE.lo); }
uint8_t CPU::RL_H(void) { return RL(HL.hi); }
uint8_t CPU::RL_L(void) { return RL(HL.lo); }

uint8_t CPU::RL_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    RL(value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::RL_A(void) { return RL(AF.hi); }

uint8_t CPU::RR(uint8_t& reg) {
    bool bit0 = reg & 1;

    reg = (reg >> 1) | (getFlag(CF) << 7);

    setFlag(ZF, reg == 0);
    setFlag(HF, false);
    setFlag(NF, false);
    setFlag(CF, bit0);

    return 2;
}

uint8_t CPU::RR_B(void) { return RR(BC.hi); }
uint8_t CPU::RR_C(void) { return RR(BC.lo); }
uint8_t CPU::RR_D(void) { return RR(DE.hi); }
uint8_t CPU::RR_E(void) { return RR(DE.lo); }
uint8_t CPU::RR_H(void) { return RR(HL.hi); }
uint8_t CPU::RR_L(void) { return RR(HL.lo); }

uint8_t CPU::RR_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    RR(value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::RR_A(void) { return RR(AF.hi); }

uint8_t CPU::SLA(uint8_t& reg) {
    bool bit7 = (reg >> 7) & 1;

    reg = reg << 1;

    setFlag(ZF, reg == 0);
    setFlag(HF, false);
    setFlag(NF, false);
    setFlag(CF, bit7);

    return 2;
}

uint8_t CPU::SLA_B(void) { return SLA(BC.hi); }
uint8_t CPU::SLA_C(void) { return SLA(BC.lo); }
uint8_t CPU::SLA_D(void) { return SLA(DE.hi); }
uint8_t CPU::SLA_E(void) { return SLA(DE.lo); }
uint8_t CPU::SLA_H(void) { return SLA(HL.hi); }
uint8_t CPU::SLA_L(void) { return SLA(HL.lo); }

uint8_t CPU::SLA_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    SLA(value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::SLA_A(void) { return SLA(AF.hi); }

uint8_t CPU::SRA(uint8_t& reg) {
    bool bit7 = reg >> 7;
    bool bit0 = reg & 1;

    reg = (reg >> 1) | (bit7 << 7);

    setFlag(ZF, reg == 0);
    setFlag(HF, false);
    setFlag(NF, false);
    setFlag(CF, bit0);

    return 2;
}

uint8_t CPU::SRA_B(void) { return SRA(BC.hi); }
uint8_t CPU::SRA_C(void) { return SRA(BC.lo); }
uint8_t CPU::SRA_D(void) { return SRA(DE.hi); }
uint8_t CPU::SRA_E(void) { return SRA(DE.lo); }
uint8_t CPU::SRA_H(void) { return SRA(HL.hi); }
uint8_t CPU::SRA_L(void) { return SRA(HL.lo); }

uint8_t CPU::SRA_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    SRA(value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::SRA_A(void) { return SRA(AF.hi); }

uint8_t CPU::SWAP(uint8_t& reg) {
    uint8_t lower = reg & 0xF;
    reg = (reg >> 4) | (lower << 4);

    setFlag(ZF, reg == 0);
    setFlag(HF, false);
    setFlag(NF, false);
    setFlag(CF, false);

    return 2;
}

uint8_t CPU::SWAP_B(void) { return SWAP(BC.hi); }
uint8_t CPU::SWAP_C(void) { return SWAP(BC.lo); }
uint8_t CPU::SWAP_D(void) { return SWAP(DE.hi); }
uint8_t CPU::SWAP_E(void) { return SWAP(DE.lo); }
uint8_t CPU::SWAP_H(void) { return SWAP(HL.hi); }
uint8_t CPU::SWAP_L(void) { return SWAP(HL.lo); }

uint8_t CPU::SWAP_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    SWAP(value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::SWAP_A(void) { return SWAP(AF.hi); }

uint8_t CPU::SRL(uint8_t& reg) {
    bool bit0 = reg & 1;

    reg = (reg >> 1) & 0x7F; // bit 7 zeroed

    setFlag(ZF, reg == 0);
    setFlag(HF, false);
    setFlag(NF, false);
    setFlag(CF, bit0);

    return 2;
}

uint8_t CPU::SRL_B(void) { return SRL(BC.hi); }
uint8_t CPU::SRL_C(void) { return SRL(BC.lo); }
uint8_t CPU::SRL_D(void) { return SRL(DE.hi); }
uint8_t CPU::SRL_E(void) { return SRL(DE.lo); }
uint8_t CPU::SRL_H(void) { return SRL(HL.hi); }
uint8_t CPU::SRL_L(void) { return SRL(HL.lo); }

uint8_t CPU::SRL_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    SRL(value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::SRL_A(void) { return SRL(AF.hi); }

uint8_t CPU::BIT(uint8_t index, uint8_t& reg) {
    bool val = (reg >> index) & 1;

    setFlag(ZF, !val);
    setFlag(NF, false);
    setFlag(HF, true);
    // keep CF same

    return 2;
}

uint8_t CPU::BIT_0_B(void) { return BIT(0, BC.hi); }
uint8_t CPU::BIT_0_C(void) { return BIT(0, BC.lo); }
uint8_t CPU::BIT_0_D(void) { return BIT(0, DE.hi); }
uint8_t CPU::BIT_0_E(void) { return BIT(0, DE.lo); }
uint8_t CPU::BIT_0_H(void) { return BIT(0, HL.hi); }
uint8_t CPU::BIT_0_L(void) { return BIT(0, HL.lo); }

uint8_t CPU::BIT_0_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    BIT(0, value);

    return 3;
}

uint8_t CPU::BIT_0_A(void) { return BIT(0, AF.hi); }

uint8_t CPU::BIT_1_B(void) { return BIT(1, BC.hi); }
uint8_t CPU::BIT_1_C(void) { return BIT(1, BC.lo); }
uint8_t CPU::BIT_1_D(void) { return BIT(1, DE.hi); }
uint8_t CPU::BIT_1_E(void) { return BIT(1, DE.lo); }
uint8_t CPU::BIT_1_H(void) { return BIT(1, HL.hi); }
uint8_t CPU::BIT_1_L(void) { return BIT(1, HL.lo); }

uint8_t CPU::BIT_1_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    BIT(1, value);

    return 3;
}

uint8_t CPU::BIT_1_A(void) { return BIT(1, AF.hi); }

uint8_t CPU::BIT_2_B(void) { return BIT(2, BC.hi); }
uint8_t CPU::BIT_2_C(void) { return BIT(2, BC.lo); }
uint8_t CPU::BIT_2_D(void) { return BIT(2, DE.hi); }
uint8_t CPU::BIT_2_E(void) { return BIT(2, DE.lo); }
uint8_t CPU::BIT_2_H(void) { return BIT(2, HL.hi); }
uint8_t CPU::BIT_2_L(void) { return BIT(2, HL.lo); }

uint8_t CPU::BIT_2_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    BIT(2, value);

    return 3;
}

uint8_t CPU::BIT_2_A(void) { return BIT(2, AF.hi); }

uint8_t CPU::BIT_3_B(void) { return BIT(3, BC.hi); }
uint8_t CPU::BIT_3_C(void) { return BIT(3, BC.lo); }
uint8_t CPU::BIT_3_D(void) { return BIT(3, DE.hi); }
uint8_t CPU::BIT_3_E(void) { return BIT(3, DE.lo); }
uint8_t CPU::BIT_3_H(void) { return BIT(3, HL.hi); }
uint8_t CPU::BIT_3_L(void) { return BIT(3, HL.lo); }

uint8_t CPU::BIT_3_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    BIT(3, value);

    return 3;
}

uint8_t CPU::BIT_3_A(void) { return BIT(3, AF.hi); }

uint8_t CPU::BIT_4_B(void) { return BIT(4, BC.hi); }
uint8_t CPU::BIT_4_C(void) { return BIT(4, BC.lo); }
uint8_t CPU::BIT_4_D(void) { return BIT(4, DE.hi); }
uint8_t CPU::BIT_4_E(void) { return BIT(4, DE.lo); }
uint8_t CPU::BIT_4_H(void) { return BIT(4, HL.hi); }
uint8_t CPU::BIT_4_L(void) { return BIT(4, HL.lo); }

uint8_t CPU::BIT_4_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    BIT(4, value);

    return 3;
}

uint8_t CPU::BIT_4_A(void) { return BIT(4, AF.hi); }

uint8_t CPU::BIT_5_B(void) { return BIT(5, BC.hi); }
uint8_t CPU::BIT_5_C(void) { return BIT(5, BC.lo); }
uint8_t CPU::BIT_5_D(void) { return BIT(5, DE.hi); }
uint8_t CPU::BIT_5_E(void) { return BIT(5, DE.lo); }
uint8_t CPU::BIT_5_H(void) { return BIT(5, HL.hi); }
uint8_t CPU::BIT_5_L(void) { return BIT(5, HL.lo); }

uint8_t CPU::BIT_5_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    BIT(5, value);

    return 3;
}

uint8_t CPU::BIT_5_A(void) { return BIT(5, AF.hi); }

uint8_t CPU::BIT_6_B(void) { return BIT(6, BC.hi); }
uint8_t CPU::BIT_6_C(void) { return BIT(6, BC.lo); }
uint8_t CPU::BIT_6_D(void) { return BIT(6, DE.hi); }
uint8_t CPU::BIT_6_E(void) { return BIT(6, DE.lo); }
uint8_t CPU::BIT_6_H(void) { return BIT(6, HL.hi); }
uint8_t CPU::BIT_6_L(void) { return BIT(6, HL.lo); }

uint8_t CPU::BIT_6_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    BIT(6, value);

    return 3;
}

uint8_t CPU::BIT_6_A(void) { return BIT(6, AF.hi); }

uint8_t CPU::BIT_7_B(void) { return BIT(7, BC.hi); }
uint8_t CPU::BIT_7_C(void) { return BIT(7, BC.lo); }
uint8_t CPU::BIT_7_D(void) { return BIT(7, DE.hi); }
uint8_t CPU::BIT_7_E(void) { return BIT(7, DE.lo); }
uint8_t CPU::BIT_7_H(void) { return BIT(7, HL.hi); }
uint8_t CPU::BIT_7_L(void) { return BIT(7, HL.lo); }

uint8_t CPU::BIT_7_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    BIT(7, value);

    return 3;
}

uint8_t CPU::BIT_7_A(void) { return BIT(7, AF.hi); }

uint8_t CPU::RES(uint8_t index, uint8_t& reg) {
    reg &= ~(1 << index);

    return 2;
}

uint8_t CPU::RES_0_B(void) { return RES(0, BC.hi); }
uint8_t CPU::RES_0_C(void) { return RES(0, BC.lo); }
uint8_t CPU::RES_0_D(void) { return RES(0, DE.hi); }
uint8_t CPU::RES_0_E(void) { return RES(0, DE.lo); }
uint8_t CPU::RES_0_H(void) { return RES(0, HL.hi); }
uint8_t CPU::RES_0_L(void) { return RES(0, HL.lo); }

uint8_t CPU::RES_0_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    RES(0, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::RES_0_A(void) { return RES(0, AF.hi); }

uint8_t CPU::RES_1_B(void) { return RES(1, BC.hi); }
uint8_t CPU::RES_1_C(void) { return RES(1, BC.lo); }
uint8_t CPU::RES_1_D(void) { return RES(1, DE.hi); }
uint8_t CPU::RES_1_E(void) { return RES(1, DE.lo); }
uint8_t CPU::RES_1_H(void) { return RES(1, HL.hi); }
uint8_t CPU::RES_1_L(void) { return RES(1, HL.lo); }

uint8_t CPU::RES_1_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    RES(1, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::RES_1_A(void) { return RES(1, AF.hi); }

uint8_t CPU::RES_2_B(void) { return RES(2, BC.hi); }
uint8_t CPU::RES_2_C(void) { return RES(2, BC.lo); }
uint8_t CPU::RES_2_D(void) { return RES(2, DE.hi); }
uint8_t CPU::RES_2_E(void) { return RES(2, DE.lo); }
uint8_t CPU::RES_2_H(void) { return RES(2, HL.hi); }
uint8_t CPU::RES_2_L(void) { return RES(2, HL.lo); }

uint8_t CPU::RES_2_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    RES(2, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::RES_2_A(void) { return RES(2, AF.hi); }

uint8_t CPU::RES_3_B(void) { return RES(3, BC.hi); }
uint8_t CPU::RES_3_C(void) { return RES(3, BC.lo); }
uint8_t CPU::RES_3_D(void) { return RES(3, DE.hi); }
uint8_t CPU::RES_3_E(void) { return RES(3, DE.lo); }
uint8_t CPU::RES_3_H(void) { return RES(3, HL.hi); }
uint8_t CPU::RES_3_L(void) { return RES(3, HL.lo); }

uint8_t CPU::RES_3_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    RES(3, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::RES_3_A(void) { return RES(3, AF.hi); }

uint8_t CPU::RES_4_B(void) { return RES(4, BC.hi); }
uint8_t CPU::RES_4_C(void) { return RES(4, BC.lo); }
uint8_t CPU::RES_4_D(void) { return RES(4, DE.hi); }
uint8_t CPU::RES_4_E(void) { return RES(4, DE.lo); }
uint8_t CPU::RES_4_H(void) { return RES(4, HL.hi); }
uint8_t CPU::RES_4_L(void) { return RES(4, HL.lo); }

uint8_t CPU::RES_4_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    RES(4, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::RES_4_A(void) { return RES(4, AF.hi); }

uint8_t CPU::RES_5_B(void) { return RES(5, BC.hi); }
uint8_t CPU::RES_5_C(void) { return RES(5, BC.lo); }
uint8_t CPU::RES_5_D(void) { return RES(5, DE.hi); }
uint8_t CPU::RES_5_E(void) { return RES(5, DE.lo); }
uint8_t CPU::RES_5_H(void) { return RES(5, HL.hi); }
uint8_t CPU::RES_5_L(void) { return RES(5, HL.lo); }

uint8_t CPU::RES_5_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    RES(5, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::RES_5_A(void) { return RES(5, AF.hi); }

uint8_t CPU::RES_6_B(void) { return RES(6, BC.hi); }
uint8_t CPU::RES_6_C(void) { return RES(6, BC.lo); }
uint8_t CPU::RES_6_D(void) { return RES(6, DE.hi); }
uint8_t CPU::RES_6_E(void) { return RES(6, DE.lo); }
uint8_t CPU::RES_6_H(void) { return RES(6, HL.hi); }
uint8_t CPU::RES_6_L(void) { return RES(6, HL.lo); }

uint8_t CPU::RES_6_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    RES(6, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::RES_6_A(void) { return RES(6, AF.hi); }

uint8_t CPU::RES_7_B(void) { return RES(7, BC.hi); }
uint8_t CPU::RES_7_C(void) { return RES(7, BC.lo); }
uint8_t CPU::RES_7_D(void) { return RES(7, DE.hi); }
uint8_t CPU::RES_7_E(void) { return RES(7, DE.lo); }
uint8_t CPU::RES_7_H(void) { return RES(7, HL.hi); }
uint8_t CPU::RES_7_L(void) { return RES(7, HL.lo); }

uint8_t CPU::RES_7_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    RES(7, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::RES_7_A(void) { return RES(7, AF.hi); }

uint8_t CPU::SET(uint8_t index, uint8_t& reg) {
    reg |= 1 << index;

    return 2;
}

uint8_t CPU::SET_0_B(void) { return SET(0, BC.hi); }
uint8_t CPU::SET_0_C(void) { return SET(0, BC.lo); }
uint8_t CPU::SET_0_D(void) { return SET(0, DE.hi); }
uint8_t CPU::SET_0_E(void) { return SET(0, DE.lo); }
uint8_t CPU::SET_0_H(void) { return SET(0, HL.hi); }
uint8_t CPU::SET_0_L(void) { return SET(0, HL.lo); }

uint8_t CPU::SET_0_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    SET(0, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::SET_0_A(void) { return SET(0, AF.hi); }

uint8_t CPU::SET_1_B(void) { return SET(1, BC.hi); }
uint8_t CPU::SET_1_C(void) { return SET(1, BC.lo); }
uint8_t CPU::SET_1_D(void) { return SET(1, DE.hi); }
uint8_t CPU::SET_1_E(void) { return SET(1, DE.lo); }
uint8_t CPU::SET_1_H(void) { return SET(1, HL.hi); }
uint8_t CPU::SET_1_L(void) { return SET(1, HL.lo); }

uint8_t CPU::SET_1_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    SET(1, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::SET_1_A(void) { return SET(1, AF.hi); }

uint8_t CPU::SET_2_B(void) { return SET(2, BC.hi); }
uint8_t CPU::SET_2_C(void) { return SET(2, BC.lo); }
uint8_t CPU::SET_2_D(void) { return SET(2, DE.hi); }
uint8_t CPU::SET_2_E(void) { return SET(2, DE.lo); }
uint8_t CPU::SET_2_H(void) { return SET(2, HL.hi); }
uint8_t CPU::SET_2_L(void) { return SET(2, HL.lo); }

uint8_t CPU::SET_2_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    SET(2, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::SET_2_A(void) { return SET(2, AF.hi); }

uint8_t CPU::SET_3_B(void) { return SET(3, BC.hi); }
uint8_t CPU::SET_3_C(void) { return SET(3, BC.lo); }
uint8_t CPU::SET_3_D(void) { return SET(3, DE.hi); }
uint8_t CPU::SET_3_E(void) { return SET(3, DE.lo); }
uint8_t CPU::SET_3_H(void) { return SET(3, HL.hi); }
uint8_t CPU::SET_3_L(void) { return SET(3, HL.lo); }

uint8_t CPU::SET_3_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    SET(3, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::SET_3_A(void) { return SET(3, AF.hi); }

uint8_t CPU::SET_4_B(void) { return SET(4, BC.hi); }
uint8_t CPU::SET_4_C(void) { return SET(4, BC.lo); }
uint8_t CPU::SET_4_D(void) { return SET(4, DE.hi); }
uint8_t CPU::SET_4_E(void) { return SET(4, DE.lo); }
uint8_t CPU::SET_4_H(void) { return SET(4, HL.hi); }
uint8_t CPU::SET_4_L(void) { return SET(4, HL.lo); }

uint8_t CPU::SET_4_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    SET(4, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::SET_4_A(void) { return SET(4, AF.hi); }

uint8_t CPU::SET_5_B(void) { return SET(5, BC.hi); }
uint8_t CPU::SET_5_C(void) { return SET(5, BC.lo); }
uint8_t CPU::SET_5_D(void) { return SET(5, DE.hi); }
uint8_t CPU::SET_5_E(void) { return SET(5, DE.lo); }
uint8_t CPU::SET_5_H(void) { return SET(5, HL.hi); }
uint8_t CPU::SET_5_L(void) { return SET(5, HL.lo); }

uint8_t CPU::SET_5_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    SET(5, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::SET_5_A(void) { return SET(5, AF.hi); }

uint8_t CPU::SET_6_B(void) { return SET(6, BC.hi); }
uint8_t CPU::SET_6_C(void) { return SET(6, BC.lo); }
uint8_t CPU::SET_6_D(void) { return SET(6, DE.hi); }
uint8_t CPU::SET_6_E(void) { return SET(6, DE.lo); }
uint8_t CPU::SET_6_H(void) { return SET(6, HL.hi); }
uint8_t CPU::SET_6_L(void) { return SET(6, HL.lo); }

uint8_t CPU::SET_6_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    SET(6, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::SET_6_A(void) { return SET(6, AF.hi); }

uint8_t CPU::SET_7_B(void) { return SET(7, BC.hi); }
uint8_t CPU::SET_7_C(void) { return SET(7, BC.lo); }
uint8_t CPU::SET_7_D(void) { return SET(7, DE.hi); }
uint8_t CPU::SET_7_E(void) { return SET(7, DE.lo); }
uint8_t CPU::SET_7_H(void) { return SET(7, HL.hi); }
uint8_t CPU::SET_7_L(void) { return SET(7, HL.lo); }

uint8_t CPU::SET_7_mHL(void) { 
    uint8_t value = mmu->read8(HL.get());
    SET(7, value);
    mmu->write8(HL.get(), value);

    return 4;
}

uint8_t CPU::SET_7_A(void) { return SET(7, AF.hi); }

} // namespace GB2040::Core
