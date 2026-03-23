# DEFCON SSTV Badge

![Badge Status](https://img.shields.io/badge/status-development-yellow)
![License](https://img.shields.io/badge/license-MIT-green)
![Hardware](https://img.shields.io/badge/hardware-RP2350B-blue)
![RF](https://img.shields.io/badge/radio-SA818-red)

An open-source SSTV (Slow Scan Television) badge designed for DEFCON and the broader badgelife community. This badge combines amateur radio, digital imaging, and social interaction into a wearable electronic art piece shaped like a Simpsons-style retro TV with rabbit ear antennas.

## Project Overview

The DEFCON SSTV Badge enables users to:
- **Capture photos** with an integrated 2MP OV2640 camera
- **Encode images** into SSTV format with callsign overlay
- **Transmit images** via VHF/UHF radio using the SA818 transceiver
- **Receive SSTV signals** from other badges and stations
- **Store and browse** images on microSD card
- **Interact socially** through visual radio communication

Perfect for ham radio operators, makers, and anyone interested in digital modes and RF experimentation.

## Features

### Hardware Capabilities
- **RP2350B Dual-Core MCU** - QFN-80, 48 GPIOs, dedicated cores for UI and signal processing
- **SA818 VHF/UHF Transceiver** - 1W output on swappable carrier board (see below)
- **OV2640 2MP Camera** - Parallel DVP interface with 15-20 FPS live viewfinder via PIO
- **2.4" ILI9341 TFT Display** - 320x240 landscape, SPI interface with built-in SD card slot
- **PCM5102A DAC + RP2350 ADC** - I2S DAC for TX audio, internal ADC for RX audio
- **MicroSD Storage** - Via display module's built-in SD card slot (shared SPI bus)
- **2000mAh LiPo Battery** - 8-10 hours operation with USB-C charging
- **2x SAO Connectors** - Standard Shitty Add-On expansion ports
- **26x WS2812B LEDs** - Animated blinky chain across rabbit ears and display border

### Radio Features
- **Dual-Band Support** - VHF (134-174 MHz) and UHF (400-480 MHz) via carrier board swap
- **Multiple SSTV Modes** - Martin, Scottie, Robot modes with auto-detection
- **Automatic Operation** - No manual PTT required
- **Airplane Mode** - Physical slide switch to disable RF
- **SMA Antenna Port** - On carrier board, right-angle connector with antenna pointing up

### User Interface
- **5-Way D-Pad** - Navigation, menu selection, and photo capture (center button doubles as shutter)
- **Airplane Mode Switch** - Slide switch for RF enable/disable
- **PWR LED** - Traditional RGB LED for battery and system status
- **Blinky LEDs** - 26x WS2812B chain with animated TX/RX/idle patterns
- **Customizable Settings** - Callsign, frequency, SSTV mode, and more

## SA818 Carrier Board System

The SA818 transceiver has 2mm pitch castellated pads that are incompatible with user-swappable sockets. Instead of mounting the SA818 directly on the main badge, the design uses a full-width carrier board system.

### How It Works

The carrier board is a separate PCB (full badge width, ~120mm) that overlays the top portion of the main badge. The SA818 module, SMA connector, and antenna are ALL on the carrier board -- the main badge has zero RF traces.

**Mounting:** 4-point connection via 2x 1x8 signal headers (bottom edge) + 2x 1x3 mechanical headers (top corners, grounded for RF shielding).

**Band detection:** Each carrier has an ID resistor (0 ohm for VHF, DNP for UHF) read by the RP2350B on startup. The firmware automatically detects which carrier is installed and configures frequency limits accordingly. No DIP switches or manual configuration needed.

### Carrier Variants

- **SA818-V Carrier** - VHF (134-174 MHz) with matched VHF stubby antenna
- **SA818-U Carrier** - UHF (400-480 MHz) with matched UHF stubby antenna

### Swapping Carriers

1. Power down the badge
2. Unplug the current carrier board from the headers
3. Plug in the replacement carrier (VHF or UHF)
4. Power on -- firmware auto-detects the band and configures itself

Each carrier includes a band-matched stubby antenna, so users do not need to source separate antennas.

### Design Benefits

- Main badge PCB has no RF routing, simplifying layout and potentially enabling 2-layer PCB
- Carrier board has proper 50-ohm microstrip trace from SA818 to SMA
- Ground stitching around RF trace on carrier for shielding
- SA818 is factory-assembled onto carrier (2mm pitch not user-solderable)

## Power System

The SA818 draws up to 1.7A during TX, which would overwhelm a standard 3.3V regulator. The power system separates the radio from the digital rail:

```
LiPo (3.0-4.2V) --+---> TPS63001 (buck-boost) ---> 3.3V Rail (RP2350B, display, camera, etc.)
                   |
                   +---> TPS22919 (load switch) ---> SA818 (3.5-5.5V direct from LiPo)
```

- **TPS63001**: Buck-boost regulator, 3.3V output, handles full LiPo voltage range (3.0-4.2V)
- **TPS22919**: Load switch controls SA818 power (enables airplane mode and sleep)
- **MCP73871**: Battery charging controller with USB-C input
- **LC709203F**: Battery fuel gauge (I2C)

## Amateur Radio License Verification System

To comply with FCC Part 97 regulations, the badge includes a software-based license verification system.

### Operating Modes

- **Receive-Only Mode**: Default for unlicensed users. Can receive and decode SSTV images but cannot transmit.
- **Licensed Mode**: Full TX/RX for verified amateur radio operators.

### Verification Process

During initial setup, users enter their amateur radio callsign. The badge generates a unique unlock code from the callsign and the device's hardware serial number using SHA-256 hashing. This ensures each unlock code is specific to one badge-callsign combination, preventing casual code sharing between devices.

Licensed operators get full transmit capability with automatic callsign overlay on transmitted images (FCC requirement). Unlicensed users can still receive and view SSTV images -- all reception features work normally.

This system encourages proper amateur radio operation. It is not intended as absolute copy protection.

## Project Status

**Current Phase: PCB Layout**

| Milestone | Status |
|-----------|--------|
| System architecture and block diagram | Complete |
| Schematic design (all sheets) | Complete, ERC clean |
| KiCad schematic entry | Complete (all sheets) |
| SA818 carrier board PCBs (VHF + UHF) | Complete |
| Bill of materials and sourcing | Complete |
| Main badge PCB layout | In progress |
| Firmware development | Not started |
| Prototype assembly and testing | Pending PCB fabrication |

## Repository Structure

```
hrv-sstv-badge/
├── CLAUDE.md                       # AI context file (project specs)
├── CMakeLists.txt                  # Build system
├── CONTRIBUTING.md                 # Contribution guidelines
├── LICENSE                         # MIT license
├── README.md                       # This file
├── docs/                           # Documentation
│   ├── block-diagram.png           # System overview diagram
│   ├── electrical-design.md        # Circuit schematics and component specs
│   ├── firmware-architecture.md    # Detailed software design
│   ├── bom.md                      # Bill of materials with costs
│   ├── bom-raw.md                  # Raw BOM data
│   ├── project-status.md           # Design status tracker
│   └── engineers-notebook/         # Design decisions and rationale
├── firmware/                       # RP2350B embedded firmware (not yet started)
├── hardware/                       # KiCad PCB design files
│   ├── main_badge/                 # Main badge KiCad project
│   ├── sa818-v-carrier/            # VHF carrier board KiCad project
│   ├── sa818-u-carrier/            # UHF carrier board KiCad project
│   ├── footprints.pretty/          # Custom KiCad footprints
│   ├── symbols/                    # Custom KiCad symbols
│   ├── pcb-design.md               # PCB layout guidelines
│   └── README.md                   # Hardware design notes
├── mechanical/                     # Badge accessories and mounting
└── src/                            # Source scaffolding
```

## Technical Specifications

| Parameter | Specification |
|-----------|---------------|
| **MCU** | Raspberry Pi RP2350B (QFN-80), Dual ARM Cortex-M33 |
| **Radio** | SA818 VHF/UHF on carrier board, 1W output |
| **Frequency Range** | VHF: 134-174 MHz, UHF: 400-480 MHz |
| **Camera** | OV2640, 2MP, parallel DVP (8-bit), QVGA for SSTV |
| **Display** | 2.4" ILI9341 TFT LCD, 320x240 landscape, SPI |
| **Audio TX** | PCM5102A DAC via I2S |
| **Audio RX** | RP2350B internal ADC with anti-alias filter |
| **Storage** | MicroSD via display module (shared SPI bus) |
| **Battery** | 2000mAh LiPo, USB-C charging (MCP73871) |
| **Regulator** | TPS63001 buck-boost, 3.3V output |
| **Operating Time** | 8-10 hours typical use |
| **LEDs** | 26x WS2812B (10x 5050 on ears, 16x 2020 on border) + RGB PWR LED |
| **Expansion** | 2x SAO (Shitty Add-On) connectors |
| **Dimensions** | ~120mm x 120mm body, ~190mm total height with ears and feet |
| **Form Factor** | Retro TV shape with rabbit ear antennas, wearable badge (no enclosure) |

## Software Architecture

The firmware uses a bare-metal dual-core design on the RP2350B for deterministic SSTV timing:

- **Core 0**: User interface, file system, system management, display updates
- **Core 1**: SSTV encoding/decoding, audio DSP, camera control, radio management

PIO state machines handle hardware-timed signals (camera capture, WS2812B data, audio clocking). DMA transfers move data without CPU intervention. No RTOS -- bare metal for microsecond-precision SSTV timing.

### Development Environment
- **Language**: C11 (ISO/IEC 9899:2011)
- **Compiler**: GCC ARM Embedded (arm-none-eabi-gcc)
- **Build System**: CMake with Raspberry Pi Pico SDK
- **Debugger**: OpenOCD + GDB via SWD (Tag-Connect)

## Assembly Model

This badge uses a production kit approach:

- **Factory-assembled**: All SMD components on main badge, SA818 onto carrier boards
- **User-assembled at conference**: Through-hole parts (switches, connectors, pin headers)
- **User-configured**: Plug in VHF or UHF carrier board, insert battery, insert SD card

## Getting Started

### Prerequisites
- **Amateur Radio License** - Required for transmit operation (receive works without)
- **Basic Soldering Skills** - Through-hole soldering for final assembly

### Firmware Installation
1. Download the latest firmware release
2. Connect via USB-C while holding BOOTSEL
3. Copy the UF2 file to the mounted drive
4. Configure callsign, frequency, and SSTV mode

### Build from Source
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Flash via UF2 bootloader
cp defcon_sstv_badge.uf2 /Volumes/RPI-RP2/

# Or flash via SWD
openocd -f interface/cmsis-dap.cfg -f target/rp2350.cfg
```

## Badgelife Integration

- **SAO Compatibility** - 2x standard Shitty Add-On connectors (I2C + 2 GPIOs each)
- **Badge-to-Badge Communication** - SSTV image exchange over the air
- **Retro TV Aesthetic** - PCB shaped like a classic TV with rabbit ear antennas
- **Animated LEDs** - 26 WS2812B LEDs with TX/RX/idle animations across ears and display border
- **Open Source** - Hardware and software fully open for hacking and modification

## Legal and Compliance

### Amateur Radio Requirements
- Valid amateur radio license required for transmit operation
- Operate only within licensed frequency allocations
- Follow local RF exposure and power limitations
- Respect band plans and operating procedures

### Regulatory
- FCC Part 97 (United States)
- IC RSS (Canada)
- CE marking (Europe)
- Consult local regulations for other countries

### Safety
- RF radiation exposure -- use appropriate antenna and power levels
- LiPo battery safety -- follow proper charging and storage procedures
- ESD precautions during assembly

## Contributing

Contributions from the badgelife and amateur radio communities are welcome.

### How to Contribute
- **Report Issues** - Found a bug? Open a GitHub issue
- **Submit Pull Requests** - Code improvements, documentation fixes
- **Share Ideas** - Feature requests and enhancement suggestions
- **Test Hardware** - Help validate designs and assembly procedures

### Development Areas
- **Hardware**: PCB layout, carrier board design
- **Firmware**: SSTV algorithms, UI, power management
- **Documentation**: Assembly guides, user manuals, tutorials

## License

- **Hardware**: [CERN-OHL-S v2](https://cern-ohl.web.cern.ch/)
- **Software**: [MIT License](LICENSE)
- **Documentation**: [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/)

## Support and Community

- **Documentation** - See the `docs/` directory
- **Issues** - Open a GitHub issue for bugs or questions
- **Discussions** - Use GitHub Discussions for general topics
- **DEFCON** - Hardware Hacking Village and Ham Radio Village

---

**Made for the badgelife and amateur radio communities. See you at DEFCON!**

**73s!**
