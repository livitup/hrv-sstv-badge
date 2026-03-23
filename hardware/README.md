# DEFCON SSTV Badge — Hardware Design

This directory contains all KiCad design files for the DEFCON SSTV Badge.

## Directory Structure

```
hardware/
├── main_badge/                    # Main badge KiCad project
│   ├── defcon-sstv-badge.kicad_pro    # Project file
│   ├── defcon-sstv-badge.kicad_sch    # Top-level schematic
│   ├── defcon-sstv-badge.kicad_pcb    # PCB layout (in progress)
│   ├── power.kicad_sch                # Power: TPS63001, TPS22919, MCP73871, LC709203F
│   ├── audio.kicad_sch                # Audio: PCM5102A DAC, TX/RX filters
│   ├── camera-display.kicad_sch       # Display (ILI9341) + Camera (OV2640)
│   ├── mcu.kicad_sch                  # RP2350B, crystal, flash, USB-C
│   ├── switches.kicad_sch             # D-pad, airplane switch, PWR LED, WS2812B chain
│   └── connectors.kicad_sch           # SAO×2, carrier headers, SD card, Tag-Connect
├── sa818-v-carrier/               # VHF carrier board (134-174 MHz)
│   ├── sa818-v-carrier.kicad_pro
│   ├── sa818-v-carrier.kicad_sch
│   └── sa818-v-carrier.kicad_pcb      # PCB layout complete
├── sa818-u-carrier/               # UHF carrier board (400-480 MHz)
│   ├── sa818-u-carrier.kicad_pro
│   ├── sa818-u-carrier.kicad_sch
│   └── sa818-u-carrier.kicad_pcb      # PCB layout complete
├── symbols/                       # Custom KiCad symbol libraries
├── footprints.pretty/             # Custom KiCad footprint libraries
├── fp-lib-table                   # Footprint library table
├── sym-lib-table                  # Symbol library table
├── pcb-design.md                  # PCB outline, dimensions, and layout zones
└── README.md                      # This file
```

## Design Overview

### Main Badge
- **PCB shape**: Simpsons-style retro TV with rabbit ear antennas and stubby feet
- **Body dimensions**: 120mm × 120mm
- **Total height**: ~190mm (ears + body + feet)
- **Layer count**: Targeting 4-layer (signal/ground/power/signal)
- **Thickness**: 1.6mm standard FR4
- **Soldermask**: Purple with white silkscreen (Simpsons TV aesthetic)

### Carrier Boards
- **Size**: Full badge width (~120mm), overlays top of main badge
- **Purpose**: SA818 module + SMA connector + antenna — all RF on carrier, none on main badge
- **Variants**: VHF (SA818-V) and UHF (SA818-U), identical PCB with different module population
- **PCB layout**: Complete for both variants

### Key Design Decisions
- **No RF on main badge** — all RF routing is on the carrier board
- **Carrier is front-mounted** — overlays the top portion of the main badge, top edges flush
- **4-point mounting** — 2× 1×8 signal headers + 2× 1×3 mechanical headers (GND for RF shielding)
- **Band auto-detect** — carrier ID resistor (0Ω = VHF, DNP = UHF) read by firmware on startup

## KiCad Version

This project uses **KiCad 8**. Custom symbols and footprints are included in the repository.

## Schematic Status

All schematic sheets are complete and ERC clean:
- Power, Audio, Display/Camera, MCU, Switches, Connectors (main badge)
- SA818-V Carrier, SA818-U Carrier

## PCB Layout Status

- **Carrier boards**: Complete (VHF and UHF)
- **Main badge**: Board outline complete (TV shape with ears and feet), component placement and routing in progress

## Related Documentation

- [PCB Design](pcb-design.md) — Board outline, dimensions, component placement zones
- [Electrical Design](../docs/electrical-design.md) — Circuit schematics and component specs
- [BOM](../docs/bom.md) — Bill of materials with costs and sourcing
- [Project Status](../docs/project-status.md) — Design and implementation tracker

---

*73 de HRV*
