# DEFCON SSTV Badge

![Badge Status](https://img.shields.io/badge/status-development-yellow)
![License](https://img.shields.io/badge/license-MIT-green)
![Hardware](https://img.shields.io/badge/hardware-RP2350-blue)
![RF](https://img.shields.io/badge/radio-SA818-red)

An open-source SSTV (Slow Scan Television) badge designed for DEFCON and the broader badgelife community. This badge combines amateur radio, digital imaging, and social interaction into a wearable electronic art piece.

## 🎯 Project Overview

The DEFCON SSTV Badge enables users to:
- **Capture photos** with an integrated 2MP camera
- **Encode images** into SSTV format with callsign overlay
- **Transmit images** via VHF/UHF radio using the SA818 transceiver
- **Receive SSTV signals** from other badges and stations
- **Store and browse** images on microSD card
- **Interact socially** through visual radio communication

Perfect for ham radio operators, makers, and anyone interested in digital modes and RF experimentation.

## 📸 Features

### 🔧 Hardware Capabilities
- **RP2350 Dual-Core MCU** - Dedicated cores for UI and signal processing
- **SA818 VHF/UHF Transceiver** - 1W output, field-programmable frequency. Installed via 
- **2MP Camera Module** - Automatic image capture and processing
- **2.4" Color Display** - View captured and received SSTV images
- **MicroSD Storage** - Unlimited image archival capacity
- **2000mAh LiPo Battery** - 8-10 hours operation with USB-C charging
- **SAO Connectors** - Expandability for add-on modules

### 📡 Radio Features
- **Dual-Band Support** - VHF (144-148 MHz) and UHF (420-450 MHz) via module swap
- **Multiple SSTV Modes** - Martin, Scottie, Robot modes with auto-detection
- **Automatic Operation** - No manual PTT required
- **Airplane Mode** - Physical switch to disable RF for sensitive areas
- **SMA Antenna Port** - Compatible with standard amateur radio antennas

### 🎮 User Interface
- **Photo Snap Button** - Dedicated camera trigger
- **D-Pad Navigation** - Intuitive menu system and image browsing
- **Status LEDs** - TX/RX activity, battery status, and system indicators
- **Physical Switches** - Band select, airplane mode, audio mute
- **Customizable Settings** - Callsign, frequency, SSTV mode, and more

## 🚨 Amateur Radio License Verification System

To comply with FCC Part 97 regulations, the badge includes a software-based license verification system that restricts transmit functionality to licensed amateur radio operators.

### How It Works

The badge operates in two modes based on license verification status:

- **Receive-Only Mode**: Default mode for unlicensed users - can receive and decode SSTV images but cannot transmit
- **Licensed Mode**: Full transmit and receive functionality for verified amateur radio operators

### License Verification Process

During initial setup, users are prompted to enter their amateur radio callsign. The badge generates a unique unlock code using:

1. The entered callsign
2. The badge's unique device serial number
3. A cryptographic hash function to create a device-specific verification code

This approach ensures that:
- Each unlock code is unique to a specific badge and callsign combination
- Casual sharing of unlock codes between devices is prevented
- Licensed operators can easily enable full functionality
- The system creates a meaningful barrier for unlicensed operation

### User Experience

**For Licensed Operators:**
1. Enter valid amateur radio callsign during first boot
2. Badge displays "Licensed Mode Enabled" with callsign confirmation
3. Full transmit functionality is available
4. Transmitted images automatically include callsign overlay (FCC requirement)

**For Unlicensed Users:**
1. Can skip callsign entry or enter invalid callsign
2. Badge operates in "Receive-Only Mode"
3. All SSTV reception and display features work normally
4. Transmit functions are disabled with clear user feedback

### Visual Indicators

The badge provides clear indication of its current mode:
- Status LED shows license verification state
- Main menu displays current operating mode
- Transmit functions show appropriate enabled/disabled status
- Prominent amateur radio license reminders in documentation and UI

### Important Notes

- This system is designed to encourage proper amateur radio operation, not as absolute copy protection
- Licensed operators must still comply with all applicable amateur radio regulations
- The badge includes educational information about amateur radio licensing
- Users are encouraged to obtain proper licensing to access full functionality

### Technical Implementation

The verification system uses industry-standard cryptographic practices:
- Callsign and device data are hashed using SHA-256
- No plaintext callsigns or sensitive data are stored
- Verification data is stored in encrypted flash memory
- System can be reset for legitimate transfers or updates

## ⚙️ SA818 Module Swappability

The SA818 transceiver is mounted in a standard 16-pin DIP socket, allowing users to easily swap between VHF and UHF frequency bands without soldering or hardware modifications. The two module variants are pin-compatible and functionally identical except for their frequency ranges:

- **SA818-V**: VHF operation (134-174 MHz) covering the 2-meter amateur band
- **SA818-U**: UHF operation (400-480 MHz) covering the 70-centimeter amateur band

### Hardware Implementation

The badge PCB includes a 2-position DIP switch that allows the firmware to detect which module variant is installed. When swapping modules, users simply:

1. Power down the badge completely
2. Carefully remove the installed SA818 module from the DIP socket
3. Set the band select DIP switch to match the new module (VHF/UHF)
4. Insert the replacement module, ensuring proper pin 1 orientation
5. Power on the badge - firmware automatically detects the band and adjusts frequency limits accordingly

### Software Band Detection

The RP2350 firmware reads the band select switch on startup and configures:
- Frequency range validation (prevents out-of-band operation)
- Default frequencies for each band
- Antenna tuning parameters
- Display indicators showing current band

### Antenna Considerations

While the SA818 modules are easily swappable, optimal performance requires band-appropriate antennas. The SMA connector accommodates standard amateur radio antennas:
- **2-meter band**: 1/4 wave whip (~19.5 inches) or rubber duck antenna
- **70-centimeter band**: 1/4 wave whip (~6.5 inches) or dual-band antenna

Users operating on both bands should consider a dual-band antenna or maintain separate antennas for each frequency range to ensure proper SWR and maximum range.

## 🚧 Project Status

**Current Phase: Hardware Design Complete**

- ✅ System architecture and block diagram
- ✅ Complete schematic design
- ✅ Bill of materials and sourcing
- ✅ PCB layout guidelines
- 🔄 KiCAD schematic entry (in progress)
- ⏳ PCB layout and routing (planned)
- ⏳ Firmware development (planned)
- ⏳ Prototype assembly and testing (planned)

### 📋 Development Roadmap

| Phase | Description | Status | Target |
|-------|-------------|---------|--------|
| **Phase 1** | Hardware Design | ✅ Complete | Q4 2024 |
| **Phase 2** | PCB Design & Layout | 🔄 In Progress | Q1 2025 |
| **Phase 3** | Firmware Development | ⏳ Planned | Q2 2025 |
| **Phase 4** | Prototype Testing | ⏳ Planned | Q3 2025 |
| **Phase 5** | Production Ready | ⏳ Planned | DEFCON 33 |

## 📁 Repository Structure

```
defcon-sstv-badge/
├── README.md                     # This file
├── docs/                        # Documentation
│   ├── block-diagram.png        # System overview
│   ├── schematic.md             # Complete circuit description
│   ├── bom.md                   # Bill of materials
│   ├── assembly-notes.md        # Build instructions
│   └── firmware-architecture.md # Software design
├── hardware/                    # Hardware design files
│   ├── kicad/                  # KiCAD project files
│   ├── gerbers/                # Production files
│   └── 3d-models/              # Custom component models
├── firmware/                    # Embedded software
│   ├── src/                    # RP2350 source code
│   ├── lib/                    # SSTV encoding/decoding libraries
│   └── examples/               # Test programs and demos
└── mechanical/                  # Mechanical design
    ├── case/                   # 3D printable case files
    └── assembly/               # Assembly drawings and guides
```

## 🛠️ Getting Started

### Prerequisites
- **Amateur Radio License** - Required for legal operation
- **Basic Electronics Skills** - SMD soldering recommended
- **Development Environment** - For firmware customization (optional)

### Hardware Assembly
1. **Review Documentation** - Read all docs before starting
2. **Source Components** - Order parts from the BOM
3. **PCB Fabrication** - Use provided gerber files
4. **Component Assembly** - Follow assembly notes
5. **Testing & Calibration** - Verify all functions

### Firmware Installation
1. **Download Release** - Get latest firmware from releases page
2. **Flash RP2350** - Using SWD programmer or USB bootloader
3. **Configure Settings** - Set callsign, frequency, etc.
4. **Test Functions** - Verify camera, display, and radio

## 📊 Technical Specifications

| Parameter | Specification |
|-----------|---------------|
| **MCU** | Raspberry Pi RP2350, Dual ARM Cortex-M33 |
| **Radio** | SA818 VHF/UHF, 1W output, ±2.5 kHz deviation |
| **Frequency Range** | VHF: 134-174 MHz, UHF: 400-480 MHz |
| **Camera** | OV2640, 2MP, JPEG output |
| **Display** | 2.4" TFT LCD, 240x320 pixels, 65K colors |
| **Storage** | MicroSD card (user supplied) |
| **Battery** | 2000mAh Li-Po, USB-C charging |
| **Operating Time** | 8-10 hours typical use |
| **Dimensions** | ~120mm x 80mm x 15mm (TV-shaped) |
| **Weight** | ~150g including battery |

## 🎨 Badgelife Integration

This badge is designed with badgelife culture in mind:

- **SAO Compatibility** - Standard Shitty Add-On connectors for expansions
- **Interactive Features** - Badge-to-badge SSTV communication
- **Hacker Friendly** - Open source, hackable, expandable
- **Visual Appeal** - Retro TV aesthetic with animated elements
- **Social Function** - Encourages interaction between badge holders

## 🤝 Contributing

We welcome contributions from the badgelife and amateur radio communities!

### How to Contribute
- **Report Issues** - Found a bug? Let us know!
- **Submit Pull Requests** - Code improvements, documentation fixes
- **Share Ideas** - Feature requests and enhancement suggestions  
- **Test Hardware** - Help validate designs and assembly procedures
- **Write Documentation** - Tutorials, guides, and examples

### Development Areas
- 🔧 **Hardware**: PCB layout optimization, mechanical design
- 💾 **Firmware**: SSTV algorithms, UI improvements, power optimization
- 📝 **Documentation**: Assembly guides, user manuals, tutorials
- 🎨 **Design**: Badge aesthetics, case design, user experience

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

## 📜 License

This project is open source hardware and software:

- **Hardware**: Released under [CERN-OHL-S v2](https://cern-ohl.web.cern.ch/)
- **Software**: Released under [MIT License](LICENSE)
- **Documentation**: Released under [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/)

## ⚖️ Legal and Compliance

### Amateur Radio Requirements
- Valid amateur radio license required for operation
- Operate only within licensed frequency allocations
- Follow local RF exposure and power limitations
- Respect band plans and operating procedures

### Regulatory Compliance
- FCC Part 97 (United States)
- IC RSS (Canada) 
- CE marking (Europe)
- Consult local regulations for other countries

### Safety Warnings
- ⚠️ RF radiation exposure - use appropriate antenna and power levels
- ⚠️ Li-Po battery safety - follow proper charging and storage procedures
- ⚠️ SMD components - use proper ESD precautions during assembly

## 🙋 Support and Community

### Getting Help
- **Documentation** - Check the docs/ directory first
- **Issues** - Open a GitHub issue for bugs or questions
- **Discussions** - Use GitHub Discussions for general topics
- **Discord** - Join the badgelife community servers

### Community Links
- **Badgelife Community** - [badgelife.org](https://badgelife.org)
- **Amateur Radio** - Local amateur radio clubs and repeaters
- **DEFCON** - Hardware Hacking Village and Ham Radio Village

## 🔗 Related Projects

- **SAO Ecosystem** - Compatible with standard Shitty Add-Ons
- **Other SSTV Software** - MMSSTV, QSSTV, and other desktop applications
- **Badge Frameworks** - Badge.team, Adafruit CircuitPython badges
- **Amateur Radio Digital Modes** - APRS, FT8, JS8call

## 📈 Project Stats

- 🌟 **Stars**: Show your support!
- 🔧 **Contributors**: Join our team
- 📝 **Commits**: Track development progress
- 🐛 **Issues**: Help us improve
- 📦 **Releases**: Production versions

---

**Made with ❤️ for the badgelife and amateur radio communities**

*This badge is designed to bring people together through the magic of radio and visual communication. See you at DEFCON!*

---

## 📞 Contact

For questions about this project:

- **GitHub Issues** - Technical questions and bug reports
- **GitHub Discussions** - General discussion and community

**73s!** 📡