# Documentation

This directory contains comprehensive technical documentation for the DEFCON SSTV Badge project.

## System Overview

[![System Block Diagram](block-diagram.png)](block-diagram.png)

The DEFCON SSTV Badge combines amateur radio, digital imaging, and embedded systems into a wearable electronic badge for the badgelife community. The system uses a dual-core RP2350 microcontroller with SA818 radio transceiver to capture, encode, transmit, and receive SSTV (Slow Scan Television) images.

## Documentation Files

### Hardware Design

**[schematic.md](schematic.md)**  
Complete circuit schematic with detailed component connections, pin assignments, and electrical specifications. Includes power management, MCU configuration, peripheral interfaces, and PCB layout considerations.

**[bom.md](bom.md)**  
Comprehensive bill of materials with part numbers, suppliers, quantities, and cost estimates. Organized by functional sections with volume pricing information and sourcing recommendations.

### Assembly and Manufacturing

**[assembly-notes.md](assembly-notes.md)**  
Step-by-step assembly guide covering safety requirements, tool lists, component placement, soldering techniques, and testing procedures. Includes troubleshooting guides and quality control checklists.

### Software Architecture

**[firmware-architecture.md](firmware-architecture.md)**  
Detailed firmware design document describing the dual-core software architecture, real-time constraints, SSTV signal processing algorithms, and embedded C programming patterns used throughout the project.

## Technical Specifications

| Parameter | Specification |
|-----------|---------------|
| **Microcontroller** | Raspberry Pi RP2350 (Dual ARM Cortex-M33) |
| **Radio** | SA818 VHF/UHF transceiver, 1W output |
| **Camera** | OV2640 2MP with JPEG encoding |
| **Display** | 2.4" TFT LCD, 240x320 pixels |
| **Storage** | MicroSD card (user supplied) |
| **Power** | 2000mAh LiPo, USB-C charging |
| **Battery Life** | 8-10 hours typical use |
| **Dimensions** | ~120mm x 80mm TV-shaped PCB |
| **Interfaces** | SMA antenna, SAO expansion ports |

## Signal Flow

### Transmit Mode
Photo capture → Image processing → Callsign overlay → SSTV encoding → Audio codec → SA818 radio → Antenna transmission

### Receive Mode  
Antenna reception → SA818 radio → Audio codec → SSTV decoding → Image reconstruction → Display → SD card storage

### User Interface
D-pad controls → RP2350 Core 0 → Menu navigation → Display updates → Status feedback

---

*This documentation is continuously updated as the project evolves. Check the git history for the latest changes and improvements.*

**73s!** 📡