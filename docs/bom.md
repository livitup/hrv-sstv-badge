# DEFCON SSTV Badge - Complete Bill of Materials

## Main ICs and Modules

| Ref | Qty | Description | Part Number | Manufacturer | Supplier | Est. Price |
|-----|-----|-------------|-------------|--------------|----------|------------|
| U4 | 1 | Microcontroller, ARM Cortex-M33, RP2350 | RP2350 | Raspberry Pi | DigiKey | $1.50 |
| U8 | 1 | RF Transceiver Module (socketed) | SA818-V or SA818-U | NiceRF | AliExpress | $12.00 |
| U7 | 1 | Camera Module, 2MP JPEG | OV2640 | OmniVision | Amazon | $8.00 |
| U6 | 1 | TFT LCD Display, 2.4", 240x320 | ST7789 Controller | Generic | Amazon | $12.00 |
| U5 | 1 | SPI Flash Memory, 16MB | W25Q128JVSIQ | Winbond | DigiKey | $1.25 |

## Power Management

| Ref | Qty | Description | Part Number | Manufacturer | Supplier | Est. Price |
|-----|-----|-------------|-------------|--------------|----------|------------|
| U1 | 1 | Li-Po Battery Charger IC | MCP73871-2CCI/ML | Microchip | DigiKey | $2.50 |
| U2 | 1 | Battery Fuel Gauge IC | LC709203FXE-01TBG | ON Semi | DigiKey | $2.25 |
| U3 | 1 | 3.3V LDO Regulator, 1A | AMS1117-3.3 | AMS | LCSC | $0.25 |
| BT1 | 1 | Li-Po Battery, 2000mAh | 503759 | Generic | Amazon | $8.00 |

## Audio Processing

| Ref | Qty | Description | Part Number | Manufacturer | Supplier | Est. Price |
|-----|-----|-------------|-------------|--------------|----------|------------|
| U9 | 1 | Audio Codec, Stereo, Low Power | WM8960CLGEFL/RV | Cirrus Logic | DigiKey | $4.50 |
| SP1 | 1 | Speaker, 8Ω, 0.5W (optional) | CMS-040815-67SPK | CUI Devices | DigiKey | $1.50 |

## Connectors and Mechanical

| Ref | Qty | Description | Part Number | Manufacturer | Supplier | Est. Price |
|-----|-----|-------------|-------------|--------------|----------|------------|
| J1 | 1 | USB-C Receptacle, 16-pin | TYPE-C-31-M-12 | HRO Electronics | LCSC | $0.85 |
| J2 | 1 | MicroSD Card Socket, Push-Push | DM3AT-SF-PEJM5 | Hirose | DigiKey | $1.75 |
| J3 | 1 | SMA Connector, Female, PCB Mount | 142-0701-801 | Johnson | DigiKey | $3.25 |
| J4,J5 | 2 | SAO Connector, 2x3 Pin Header | PEC03SAAN | Sullins | DigiKey | $0.35 |

## User Interface Components

| Ref | Qty | Description | Part Number | Manufacturer | Supplier | Est. Price |
|-----|-----|-------------|-------------|--------------|----------|------------|
| SW3 | 1 | Tactile Switch, 12mm, Photo Button | B3F-4055 | Omron | DigiKey | $1.25 |
| SW4-SW8 | 5 | Tactile Switch, 6x6mm, D-pad | B3F-4050 | Omron | DigiKey | $0.45 |
| SW9 | 1 | Slide Switch, SPDT, Airplane Mode | SS12SDP2 | NKK Switches | DigiKey | $2.15 |
| SW2 | 1 | Slide Switch, SPDT, Audio Mute | SS12SDP2 | NKK Switches | DigiKey | $2.15 |
| SW1 | 1 | DIP Switch, 2-position | 219-2LPST | CTS | DigiKey | $1.25 |

## LEDs and Indicators

| Ref | Qty | Description | Part Number | Manufacturer | Supplier | Est. Price |
|-----|-----|-------------|-------------|--------------|----------|------------|
| LED1 | 1 | LED, Orange, Charge Status | LTST-C170KFKT | Lite-On | DigiKey | $0.15 |
| LED2 | 1 | LED, Green, Charge Complete | LTST-C170GKT | Lite-On | DigiKey | $0.15 |
| LED3 | 1 | LED, Green, Power | LTST-C170GKT | Lite-On | DigiKey | $0.15 |
| LED4 | 1 | LED, Red, TX Status | LTST-C170CKT | Lite-On | DigiKey | $0.15 |
| LED5 | 1 | LED, Blue, RX Status | LTST-C170TBKT | Lite-On | DigiKey | $0.15 |
| LED6 | 1 | LED, Yellow, SD Card | LTST-C170YKT | Lite-On | DigiKey | $0.15 |

## Resistors (0805 Package)

| Ref | Qty | Value | Description | Supplier | Est. Price |
|-----|-----|-------|-------------|----------|------------|
| R1-R20 | 20 | Various | 1% Metal Film, 0805 | LCSC | $0.02 each |

### Specific Resistor Values:
- **1kΩ (6 qty)**: LED current limiting, pullups
- **2kΩ (1 qty)**: MCP73871 charge current setting
- **4.7kΩ (4 qty)**: I2C pullups
- **5.1kΩ (2 qty)**: USB-C CC resistors
- **10kΩ (7 qty)**: General pullups, button pullups

## Capacitors

| Ref | Qty | Value | Voltage | Package | Description | Supplier | Est. Price |
|-----|-----|-------|---------|---------|-------------|----------|------------|
| C1-C5 | 5 | 10-22µF | 16V | 1206 | Tantalum, Power decoupling | LCSC | $0.15 |
| C6-C25 | 20 | 100nF | 16V | 0805 | Ceramic, IC decoupling | LCSC | $0.03 |
| C26-C27 | 2 | 22pF | 16V | 0805 | Ceramic, Crystal loading | LCSC | $0.03 |
| C28-C30 | 3 | 2.2µF | 16V | 0805 | Ceramic, Audio coupling | LCSC | $0.08 |

## Inductors and Filters

| Ref | Qty | Value | Description | Supplier | Est. Price |
|-----|-----|-------|-------------|----------|------------|
| L1 | 1 | 10µH | Ferrite Bead, Camera supply filter | LCSC | $0.08 |
| L2 | 1 | 600Ω@100MHz | Ferrite Bead, Audio supply filter | LCSC | $0.08 |

## Semiconductors

| Ref | Qty | Description | Part Number | Manufacturer | Supplier | Est. Price |
|-----|-----|-------------|-------------|--------------|----------|------------|
| Q1-Q4 | 4 | MOSFET, N-Channel, SOT-23 | 2N7002K | Various | LCSC | $0.05 |

## Crystal and Timing

| Ref | Qty | Description | Part Number | Manufacturer | Supplier | Est. Price |
|-----|-----|-------------|-------------|--------------|----------|------------|
| Y1 | 1 | Crystal, 12MHz, ±20ppm | ABM8-12.000MHZ-18-D2Y-T | Abracon | DigiKey | $0.45 |

## PCB and Assembly

| Item | Qty | Description | Supplier | Est. Price |
|------|-----|-------------|----------|------------|
| PCB | 5 | 4-layer PCB, 120x80mm, HASL | JLCPCB | $25.00 |
| Assembly | 1 | Pick-and-place service (optional) | JLCPCB | $15.00 |
| Stencil | 1 | Solder paste stencil | JLCPCB | $8.00 |

## Hardware and Accessories

| Item | Qty | Description | Supplier | Est. Price |
|------|-----|-------------|----------|------------|
| Socket | 1 | SA818 Module Socket, 16-pin DIP | DigiKey | $1.25 |
| Standoffs | 4 | PCB Standoffs, M3, 8mm | Amazon | $0.25 |
| Screws | 4 | M3x6mm, Phillips head | Amazon | $0.10 |
| Lanyard | 1 | Badge lanyard with clip | Amazon | $1.50 |

## Cost Summary

| Category | Subtotal |
|----------|----------|
| **Main ICs and Modules** | $34.75 |
| **Power Management** | $13.00 |
| **Audio Processing** | $6.00 |
| **Connectors** | $6.20 |
| **User Interface** | $7.25 |
| **LEDs** | $0.90 |
| **Passives (R,C,L)** | $3.50 |
| **Semiconductors** | $0.65 |
| **Crystal** | $0.45 |
| **PCB and Assembly** | $48.00 |
| **Hardware** | $3.10 |
| **TOTAL per badge** | **≈$124** |

## Volume Pricing Estimates

- **10 units**: ~$95 per badge
- **50 units**: ~$75 per badge  
- **100 units**: ~$65 per badge
- **500 units**: ~$55 per badge

*Prices are estimated and may vary based on supplier, quantity, and market conditions*

## Recommended Suppliers

1. **JLCPCB/LCSC**: PCB fabrication and common components
2. **DigiKey**: Premium components, fast shipping
3. **Mouser**: Alternative for premium components
4. **Amazon**: Mechanical parts, batteries
5. **AliExpress**: SA818 modules, displays (longer lead times)

## Assembly Notes

1. **Surface Mount Components**: Most components are 0805 or larger for easier hand assembly
2. **Critical Components**: SA818 socket allows for easy module swapping
3. **Test Points**: Include test points for major power rails and signals
4. **Programming**: Include Tag-Connect footprint for RP2350 programming
5. **Component Orientation**: Mark pin 1 clearly on all ICs
6. **ESD Protection**: Include ESD protection on exposed connectors

## Firmware Development Requirements

- **RP2350 SDK**: Raspberry Pi Pico SDK
- **SSTV Libraries**: Custom encoding/decoding algorithms
- **Camera Interface**: DVP driver for OV2640
- **Display Driver**: ST7789 SPI display driver
- **Audio Processing**: I2S interface for WM8960
- **SA818 Control**: UART protocol implementation
- **File System**: FatFS for microSD card
- **UI Framework**: Simple menu system with button handling
