import sys
import os

def main():
    input_rom = sys.argv[1]
    output_h = sys.argv[2]

    with open(input_rom, "rb") as f:
        data = f.read()

    rom_size = len(data)

    lines = []
    lines.append("#pragma once")
    lines.append("")
    lines.append("#include <cstdint>")
    lines.append("")
    lines.append(f"static constexpr uint32_t embedded_rom_size = {rom_size};")
    lines.append("static constexpr uint8_t embedded_rom[] = {")

    for i in range(0, rom_size, 12):
        chunk = data[i:i+12]
        hex_vals = ", ".join(f"0x{b:02x}" for b in chunk)
        lines.append(f"    {hex_vals},")

    lines.append("};")
    lines.append("")

    with open(output_h, "w") as f:
        f.write("\n".join(lines))

if __name__ == "__main__":
    main()
