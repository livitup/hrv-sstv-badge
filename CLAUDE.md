# Claude Context: DEFCON SSTV Badge

## Project Overview
This is an open-source SSTV (Slow Scan Television) badge for DEFCON and the badgelife community. The badge enables users to capture photos, encode them as SSTV signals, and transmit/receive via amateur radio.

## Hardware Architecture
- **MCU**: Raspberry Pi RP2350B (QFN-80, 48 GPIOs) — dual ARM Cortex-M33 cores
- **Radio**: SA818 VHF/UHF transceiver module (1W output) on swappable carrier board
- **Camera**: OV2640 2MP module, parallel DVP interface (8-bit data + control signals)
- **Display**: 2.4" ILI9341 TFT LCD (320×240 landscape, SPI) with built-in SD slot
- **Storage**: MicroSD card via display module's SD slot (shared SPI bus)
- **Power**: 2000mAh LiPo battery with USB-C charging
- **Audio**: PCM5102A DAC (TX) + RP2350 ADC (RX) for SSTV signal processing
- **Expansion**: 2x SAO (Shitty Add-On) connectors

## Power System Architecture

**Key Design Decision:** The SA818 runs directly from the LiPo battery (via load switch), NOT through the 3.3V regulator. This separates the 1.7A TX current spikes from the main digital rail.

```
LiPo (3.0-4.2V) ──┬──► TPS63001 (buck-boost) ──► 3.3V Rail (RP2350, display, etc.)
                  │
                  └──► TPS22919 (load switch) ──► SA818 (3.5-5.5V direct)
```

**Components:**
- **TPS63001**: Buck-boost regulator, 3.3V output, handles full LiPo voltage range
- **TPS22919**: Load switch for SA818 power control (airplane mode, sleep)
- **MCP73871**: Battery charging controller
- **LC709203**: Battery fuel gauge
- **USB-C**: Through-hole connector with ESD protection (USBLC6-2 for data, PESD5V0 for VBUS)

See `docs/electrical-design.md` for complete power system documentation.

## SA818 Carrier Board System

**Problem:** SA818 has 2mm pitch castellated pads, incompatible with user-swappable sockets.

**Solution:** Full-width carrier board that overlays the top of the main badge:
- Carrier is full badge width (~120mm), mounts in FRONT of main badge
- Top edges flush — carrier overlays top portion of main badge
- SA818, SMA connector, and antenna ALL on carrier — no RF on main badge
- 4-point mounting: 2x 1x8 signal headers (bottom) + 2x 1x3 mechanical headers (top corners)
- Each carrier includes a matched stubby antenna (VHF or UHF)
- User swaps entire carrier assembly to change bands

**Carrier variants:**
- SA818-V carrier (VHF, 134-174 MHz) with VHF stubby antenna
- SA818-U carrier (UHF, 400-480 MHz) with UHF stubby antenna

**Key benefit:** Main badge has ZERO RF traces, simplifying design and potentially enabling 2-layer PCB.

See `docs/electrical-design.md` for complete carrier board design.

## Audio Interface

**Design choice:** PCM5102A DAC for TX, RP2350 internal ADC for RX.

- **TX path**: RP2350 → I2S → PCM5102A → attenuator/filter → SA818 MIC
- **RX path**: SA818 SPK → DC block → anti-alias filter → bias network → RP2350 ADC
- **Cost**: ~$2.05 total (vs ~$3.50 for WM8960 codec)
- **Quality**: Excellent TX, good RX (adequate for indoor LVCC use)

The TX/RX filter components (resistors, caps) are 0603 and user-solderable. The PCM5102A (TSSOP-20) requires hot air or fab assembly.

See `docs/electrical-design.md` for complete audio circuit design and component values.

## Display Interface

**Module:** 2.4" ILI9341-based TFT LCD breakout with built-in SD card slot.

- **Resolution**: 320×240 pixels, landscape orientation
- **Interface**: SPI (up to 62.5 MHz)
- **Backlight**: PWM dimming via GPIO + 2N7002 MOSFET
- **SD Card**: Built-in slot, shared SPI bus with display (separate CS pins)
- **Touch**: None (D-pad navigation)

**SSTV Compatibility:**
- Robot 36/72 (320×240): Perfect 1:1 fit
- Martin/Scottie (320×256): Crop 16-line gray banner or center with small bars
- PD modes (320×480): Scale to fit

The 320×240 resolution covers the vast majority of real-world SSTV traffic.

See `docs/electrical-design.md` for complete display circuit design.

## Camera Interface

**Module:** OV2640 2MP camera on breakout board with 2.54mm pin header (user-solderable).

- **Interface**: Parallel DVP (8-bit data) + SCCB (I2C) for control
- **Resolution**: QVGA (320×240) for SSTV, higher available if needed
- **Output**: RGB565 for viewfinder, JPEG for storage
- **GPIOs**: 12 dedicated (D0-D7, PCLK, VSYNC, HREF, XCLK) + shared I2C
- **Viewfinder**: 15-20 FPS live preview via PIO capture + DMA

**Why parallel DVP (not SPI)?** Live viewfinder requires fast capture. SPI camera modules are too slow (~1-2 FPS) for real-time preview.

**Why RP2350B?** Camera uses 12 GPIOs, user controls use 10. Total system needs ~47 GPIOs, exceeding RP2350A's 30. RP2350B (QFN-80) provides 48 GPIOs with 1 spare.

See `docs/electrical-design.md` for complete camera circuit design.

## User Controls

**Physical controls for badge interaction:**

| Control | Type | GPIOs |
|---------|------|-------|
| D-pad | 5× tactile switches (up/down/left/right/center) | 5 |
| Photo button | 12mm tactile switch | 1 |
| Airplane mode | Slide switch (GPIO read) | 1 |
| PWR LED | Traditional RGB (battery status) | 3 |
| Blinky chain | 26× WS2812B (ears + border) | 1 |

**Total: 11 GPIOs** for user controls

- All switches: active-low with internal pullups (no external resistors)
- PWR LED: Traditional RGB, direct GPIO drive (visually distinct from blinky)
- Software debounce (no hardware debounce caps)

**Blinky LED Matrix (26 WS2812B):**
- PCB shaped like Simpsons-style retro TV with rabbit ear antennas
- Single GPIO drives entire WS2812B chain via PIO
- **Rabbit Ears (#0-9):** 5 LEDs per ear
  - TX: Amber chase UP → tip flashes light blue → WHITE lightning discharge ⚡
  - RX: WHITE materializes at tips → light blue → green chase DOWN 📡
  - Idle: Tips breathe soft blue (~4sec cycle, very subtle) — "listening to the ether"
- **Display Border (#10-25):** 16 LEDs framing display
  - Idle: Rainbow chase flowing around frame 🌈
  - TX: "Capacitor Discharge" — border charges up, ears ZAP, everything flashes WHITE 💥
  - RX: "Filling Tank" — each ear pulse adds a row, border fills up over multiple pulses 📥
- Power budget: ~70mA idle, ~110mA active TX/RX

See `docs/electrical-design.md` for complete user controls circuit design.

## RF Design Notes

- **All RF is on the carrier board** — main badge has no RF routing
- Carrier has 50Ω microstrip trace from SA818 to right-angle SMA
- SMA at top edge, antenna points UP (slight backward tilt for comfort)
- Ground via stitching around RF trace on carrier
- Main badge only needs female header sockets at top edge

## Software Architecture
**Dual-Core Design:**
- **Core 0**: User interface, file system, system management, display updates
- **Core 1**: SSTV encoding/decoding, audio DSP, camera control, radio management

## Development Environment
- **Language**: C11 (ISO/IEC 9899:2011)
- **Compiler**: GCC ARM Embedded (arm-none-eabi-gcc)
- **IDE**: CLion on macOS
- **Build System**: CMake with Raspberry Pi Pico SDK
- **Target**: ARM Cortex-M33 (ARMv8-M architecture)
- **Debugger**: OpenOCD + GDB via SWD

## Critical Real-Time Constraints
**SSTV Timing Requirements (Critical):**
- Line sync pulses: 5ms ±0.1ms precision required
- Pixel timing: ~0.46ms per pixel (Martin M1 mode)
- Color component timing: 146.43ms total line time ±0.1%
- VIS code timing: 300ms sequence with precise bit timing
- Audio sample rates: 8-48kHz with minimal jitter

**RP2350 Real-Time Strategy:**
- **Core 0**: User interface, file system, system management (non-critical timing)
- **Core 1**: Dedicated SSTV processing, audio DSP, timing-critical operations
- **PIO State Machines**: Hardware-generated precise timing signals
- **DMA Controllers**: Audio codec data transfer without CPU intervention
- **Hardware Timers**: Microsecond-level timing accuracy
- **Interrupt Priorities**: SSTV processing gets highest priority levels
- **No Linux/RTOS**: Bare metal operation for deterministic timing

## Amateur Radio License Verification System
**Software-Based License Control:**
- Default mode: Receive-only for unlicensed users
- Licensed mode: Full TX/RX for verified amateur operators
- Callsign + device serial number generate unique unlock codes
- SHA-256 hash verification prevents casual circumvention
- Clear visual indicators of current operating mode
- Automatic callsign overlay on transmitted images (FCC requirement)

## Project Structure
```
defcon-sstv-badge/
├── CLAUDE.md                     # This file - project context for Claude
├── docs/                         # Complete documentation
│   ├── firmware-architecture.md  # Detailed software design
│   ├── schematic.md              # Hardware connections
│   ├── bom.md                    # Bill of materials
│   ├── assembly-notes.md         # Build instructions
│   └── power-and-rf-design.md    # Power system and RF design notes
├── firmware/                     # RP2350 embedded C code
│   ├── src/                      # Main source code
│   │   ├── main.c                # Entry point and initialization
│   │   ├── core0_main.c          # UI and system management
│   │   ├── core1_main.c          # Signal processing
│   │   ├── hardware/             # HAL and device drivers
│   │   ├── sstv/                 # SSTV encoding/decoding
│   │   └── ui/                   # User interface system
│   ├── lib/                      # External libraries
│   └── examples/                 # Test programs
├── hardware/                     # KiCAD PCB design files
│   ├── main-badge/               # Main badge PCB (planned)
│   └── sa818-carrier/            # SA818 carrier board (planned)
└── mechanical/                   # Badge accessories and mounting
```

## Current Development Status
- **Hardware design**: Schematic concept complete, power system redesigned
- **Documentation**: Comprehensive technical docs, power/RF design documented
- **Project structure**: Professional repository setup
- **KiCad entry**: Not started - need to implement redesigned power system
- **Firmware development**: Not started
- **PCB layout**: Planned after schematic entry
- **Prototype testing**: Following PCB fabrication

## Key Design Documents
- `docs/schematic.md` - Original hardware connections (needs update for power system)
- `docs/electrical-design.md` - **NEW** - Power system redesign, SA818 carrier, RF routing
- `docs/bom.md` - Bill of materials (needs update for new power components)
- `docs/firmware-architecture.md` - Complete software design
- `docs/assembly-notes.md` - Build instructions

## Hardware Changes from Original Design

| Subsystem | Original | Revised |
|-----------|----------|---------|
| 3.3V regulator | AMS1117-3.3 (LDO) | TPS63001 (buck-boost) |
| SA818 power | Through 3.3V rail | Direct from LiPo via TPS22919 |
| SA818 mounting | "DIP socket" | Full-width carrier board, front-mounted overlay |
| Band selection | DIP switch | Auto-detect via carrier ID pin |
| RF routing | On main badge (not specified) | ALL RF on carrier — main badge has none |
| SMA connector | On main badge | On carrier, right-angle, antenna points up |
| Antenna | User-supplied | Included with each carrier (band-matched) |
| Audio codec | WM8960 (stereo, $3.50) | PCM5102A DAC + RP2350 ADC ($2.05) |
| Display | 2.4" ST7789 (separate SD socket) | 2.4" ILI9341 with built-in SD slot |
| SD card | Separate socket | Via display module (shared SPI) |
| Camera | OV2640 (interface unspecified) | OV2640 parallel DVP (8-bit + control) |
| MCU variant | RP2350 (unspecified) | RP2350B (QFN-80, 48 GPIOs) |

## Key Hardware Interfaces
```c
// GPIO assignments (RP2350B) - 48/48 allocated
// I2C shared bus
#define I2C_SDA          0    // Camera, fuel gauge, SAO×2
#define I2C_SCL          1

// SPI display + SD
#define SPI_SCK          2
#define SPI_MOSI         3
#define SPI_MISO         4
#define DISP_CS          5
#define DISP_DC          6
#define DISP_RST         7

// Camera DVP (GPIO8-19)
#define CAM_D0           8    // D0-D7 consecutive for PIO
#define CAM_PCLK         16
#define CAM_VSYNC        17
#define CAM_HREF         18
#define CAM_XCLK         19   // PWM ~20MHz

// SAO connectors
#define SAO1_GPIO1       20
#define SAO1_GPIO2       21
#define SAO2_GPIO1       27
#define SAO2_GPIO2       28

// Display misc
#define DISP_BL          22   // PWM backlight
#define SD_CS            23

// Audio (I2S for TX)
#define I2S_BCK          24
#define I2S_LRCK         25
#define I2S_DIN          29

// User controls (active-low switches)
#define DPAD_UP          37
#define DPAD_DOWN        38
#define DPAD_LEFT        39
#define DPAD_RIGHT       26   // Moved from GPIO40 (GPIO40 needed for ADC)
#define DPAD_CENTER      41
#define PHOTO            42
#define AIRPLANE         43

// SA818 (GPIO30-36)
#define SA818_TX         30
#define SA818_RX         31
#define SA818_PTT        32
#define SA818_PD         33   // Also TPS22919 enable
#define SA818_HL         34
#define SA818_SQ         35
#define SA818_ID         36

// ADC input (RP2350B: ADC channels are GPIO40-47, NOT GPIO26-29 like RP2040)
#define RX_AUDIO         40   // ADC0 - RX audio from SA818

// LEDs (GPIO44-47)
#define PWR_R            44
#define PWR_G            45
#define PWR_B            46
#define LED_DATA         47   // WS2812B chain (26 LEDs)
```

See `docs/electrical-design.md` for complete GPIO assignment details.

## Coding Standards
- **Style**: 4-space indentation, snake_case naming
- **Functions**: Doxygen-style documentation
- **Error handling**: Always check return values
- **Memory**: Avoid dynamic allocation, use static buffers
- **Hardware**: Volatile for registers, proper initialization
- **Real-time**: Understand timing constraints for SSTV

## Build and Debug Workflow
```bash
# Build process
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Flash firmware
# Method 1: UF2 bootloader (hold BOOTSEL + USB)
cp defcon_sstv_badge.uf2 /Volumes/RPI-RP2/

# Method 2: SWD debugging
openocd -f interface/cmsis-dap.cfg -f target/rp2350.cfg
# (then use CLion debugger)
```

## Assembly Model

**Production kit approach:**
- Fab pre-assembles all SMD components on main badge
- Fab assembles SA818 onto carrier boards separately
- User solders through-hole parts at conference (switches, connectors, headers)
- User plugs in carrier module (VHF or UHF)

## Important Notes
- This is a **wearable badge** - no enclosure, PCB is the product
- **Real-time constraints** are critical for SSTV timing accuracy
- **Dual-core architecture** requires careful resource sharing
- **Amateur radio regulations** must be followed for legal operation
- **Community project** - designed for badgelife and ham radio communities
- **Power system was redesigned** - original LDO approach had fatal flaws

---
*Context file version: 2.0*
*Last updated: 2025-02*
