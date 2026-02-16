# DEFCON SSTV Badge — Bill of Materials

**Version:** 3.0
**Last Updated:** 2026-02
**Source:** KiCad schematic export, verified against `docs/electrical-design.md`

This is the consolidated BOM for the DEFCON SSTV badge. Components are organized by subsystem and assembly method.

---

## Quick Summary

| Category | Est. Cost | Notes |
|----------|-----------|-------|
| Power System | ~$4.00 | TPS63001 buck-boost, TPS22919 load switch |
| USB-C Interface | ~$1.20 | Connector + ESD protection |
| Battery System | ~$11.50 | MCP73871, LC709203F, 2000mAh LiPo |
| Audio | ~$2.10 | PCM5102A DAC + filter passives |
| Display | ~$10.00 | ILI9341 module + backlight circuit |
| Camera | ~$6.00 | OV2640 DVP module + resistors |
| User Controls | ~$1.50 | Switches, PWR LED, resistors |
| Blinky LEDs | ~$2.60 | 26× WS2812B + bypass caps |
| MCU + Support | ~$3.90 | RP2350B, crystal, flash, regulator inductor, passives |
| Connectors | ~$2.00 | SAO, carrier sockets, SD card, debug |
| **Main Badge Total** | **~$46** | Without carrier |
| SA818 Carrier | ~$17.00 | SA818, SMA, antenna, headers |
| **Complete Kit** | **~$63** | Main + one carrier |

*PCB fabrication adds ~$3-5/board at quantity.*

---

## Assembly Legend

| Code | Meaning |
|------|---------|
| **Fab** | Factory assembled (requires reflow/hot air) |
| **User** | User-solderable at conference (through-hole or large pads) |
| **Module** | Pre-assembled breakout, user installs |

---

## Power System

### Buck-Boost Regulator (TPS63001)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U5 | 1 | TPS63001DRCR | QFN-10 (3×3mm) | Fab | Fixed 3.3V output |
| L1 | 1 | 1µH | 3×3mm | Fab | Coilcraft XAL3030-102 or equiv |
| C_IN, C_OUT1 | 2 | 10µF | 0805 | Fab | X5R/X7R, 10V, input/output |

*Note: PS/SYNC resistor intentionally omitted — forced PWM mode provides cleaner audio with minimal ripple. Power save mode would reduce idle current but risks noise coupling into SSTV audio path.*

**Subtotal:** ~$2.50

### Load Switch (TPS22919)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U6 | 1 | TPS22919DCKT | SOT-23-5 | Fab | SA818 power control |
| C17 | 1 | 100µF | 1206 | Fab | Output bulk capacitor |
| C18 | 1 | 10µF | 0603 | Fab | Output bypass |

**Subtotal:** ~$0.80

---

## USB-C Interface

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| J2 | 1 | USB4105-GF-A | SMT+TH hybrid | Fab | GCT USB-C with D+/D- (USB 2.0) |
| U3 | 1 | USBLC6-2SC6 | SOT-23-6 | Fab | D+/D- ESD protection |
| D_VBUS1 | 1 | TVS (PESD5V0S1BL) | SOD-323 | Fab | VBUS surge protection |
| R_CC1, R_CC2 | 2 | 5.1kΩ | 0402 | Fab | USB sink identification (to GND) |
| C_USB1 | 1 | 10µF | 0603 | Fab | VBUS decoupling |

**Subtotal:** ~$1.20

---

## Battery System

### Charger (MCP73871)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U4 | 1 | MCP73871-2CCI/ML | QFN-20 (4×4mm) | Fab | Li-Po charge controller |
| R3 | 1 | 2kΩ | 0603 | Fab | PROG1: Charge current = 1000/R = 500mA |
| R4 | 1 | 15kΩ | 0603 | Fab | PROG3: Pre-charge/termination = ~67mA |
| C8, C10, C11 | 3 | 10µF | 0805 | Fab | Input/battery/output caps, X5R 10V |

**Subtotal:** ~$3.00

### Fuel Gauge (LC709203F)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U7 | 1 | LC709203FQH-01TWG | WDFN-8 (2×2.5mm) | Fab | Battery SoC monitor, I2C |
| C16 | 1 | 100nF | 0402 | Fab | Decoupling |

**Subtotal:** ~$2.50

### Shared Thermistor

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| TH1 | 1 | NTC 10k | 0603 | Fab | B=3380, shared by MCP73871 (THERM) + LC709203F (TSENSE) |

*Place near battery for accurate temperature reading.*

### Battery + Connector

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| BAT1 | 1 | 2000mAh LiPo | Pouch ~50×35×8mm | User | JST-PH connector, 3.7V, with PCM |
| J_BAT | 1 | JST-PH 2-pin | Through-hole | User | Battery connector |

**Subtotal:** ~$6.50

---

## Audio Interface

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U8 | 1 | PCM5102A | TSSOP-20 | Fab | I2S stereo DAC for TX |
| C20 | 1 | 10µF | 0805 | Fab | DVDD decoupling |
| C19 | 1 | 100nF | 0603 | Fab | AVDD decoupling |
| C21, C22 | 2 | 2.2µF | 0603 | Fab | Charge pump caps (CAPP, CAPM) |
| R5 | 1 | 47kΩ | 0603 | User | TX attenuator (high) |
| R6 | 1 | 470Ω | 0603 | User | TX attenuator (low) |
| C24 | 1 | 10nF | 0603 | User | TX low-pass filter |
| C25 | 1 | 100nF | 0603 | User | TX DC blocking |
| C26 | 1 | 1µF | 0603 | User | RX DC blocking |
| R7, R8 | 2 | 4.7kΩ | 0603 | User | RX anti-alias filter |
| C27 | 1 | 4.7nF | 0603 | User | RX anti-alias filter |
| R9 | 1 | 47kΩ | 0603 | User | RX bias to 3.3V |
| R10 | 1 | 47kΩ | 0603 | User | RX bias to GND |

**Subtotal:** ~$2.10

---

## Display Interface

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| J3 | 1 | ILI9341 2.4" module | Breakout | Module | 320×240 SPI display |
| Q1 | 1 | 2N7002 | SOT-23 | Fab | Backlight control MOSFET |
| R11 | 1 | 1kΩ | 0603 | User | Gate pull-down |

**Subtotal:** ~$10.00

---

## Camera Interface

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| J4 | 1 | OV2640 DVP module | Breakout | Module | 2.54mm headers, parallel DVP |
| R12, R13 | 2 | 10kΩ | 0603 | Fab | RESET pullup / PWDN pulldown |

**Subtotal:** ~$6.00

---

## User Controls

### Switches

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| SW1-SW5 | 5 | Tactile 6×6mm | TH | User | D-pad (up/down/left/right/center) |
| SW6 | 1 | Tactile 12mm | TH | User | Photo capture (larger) |
| SW7 | 1 | Slide SPDT | TH | User | Airplane mode toggle |

### PWR Status LED (Traditional RGB)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| D1 | 1 | APFA3010 | PLCC-4 (3.0×1.0mm) | Fab | Kingbright RGB LED, **common-anode** |
| R14 | 1 | 220Ω | 0603 | Fab | Red cathode current limit |
| R15, R16 | 2 | 100Ω | 0603 | Fab | Green/Blue cathode current limit |

**Note:** Common-anode LED — anode connects to +3.3V, cathodes through resistors to GPIOs. Firmware uses inverted logic: GPIO LOW = LED on.

**Subtotal:** ~$1.50

---

## Blinky LED Matrix

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| D2–D27 | 26 | WS2812B | 5050 | Fab | Rabbit ears (10) + display border (16) |
| C28–C53 | 26 | 100nF | 0402 | Fab | Bypass caps (one per LED) |

**Subtotal:** ~$2.60
*(26× WS2812B @ ~$0.08 = $2.08, 26× caps @ ~$0.02 = $0.52)*

---

## MCU and Support

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U1 | 1 | RP2350B | QFN-80 (10×10mm) | Fab | 48 GPIO variant required |
| L2 | 1 | 3.3µH | 3×3mm | Fab | Internal regulator inductor (VREG_LX to DVDD) |
| Y1 | 1 | 12MHz | 3215 | Fab | ±20ppm, CL=10pF (required for USB) |
| C_Y1, C_Y2 | 2 | 15pF | 0402 | Fab | Crystal load caps |
| U2 | 1 | W25Q128JVS | SOIC-8 | Fab | 16MB QSPI flash |
| C9 | 1 | 100nF | 0402 | Fab | Flash decoupling |
| C3-C7 | 5 | 100nF | 0402 | Fab | MCU VDD decoupling |
| C2 | 1 | 10µF | 0603 | Fab | MCU bulk cap |
| C1 | 1 | 1µF | 0402 | Fab | DVDD (core voltage) decoupling |
| SW_BOOT1 | 1 | Tactile 3×4mm | SMD | Fab | BOOTSEL for UF2 bootloader |
| R1, R2 | 2 | 4.7kΩ | 0603 | Fab | I2C SDA/SCL pullups (to +3V3) |
| R17 | 1 | 10kΩ | 0603 | Fab | RUN pin pullup |

**Subtotal:** ~$3.90

*Notes:*
- *Crystal is required — RP2350 internal oscillator is too inaccurate for USB (needs ±0.25%, ROSC is ±2-5%).*
- *L2 is required for the RP2350's internal switching regulator (converts 3.3V → 1.1V core voltage).*
- *I2C pullups MUST connect to +3V3 (IOVDD level), NOT to DVDD (1.1V core).*

---

## Connectors (Main Board)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| J1 | 1 | JST-PH 2-pin | Through-hole | User | Battery connector |
| J5 | 1 | 2×3 header | 2.54mm | User | SAO connector 1 |
| J6 | 1 | 2×3 header | 2.54mm | User | SAO connector 2 |
| J7, J8 | 2 | 1×8 socket | 2.54mm | User | Carrier signal headers |
| J9 | 1 | TC2030-CTX-NL | Pads only | Fab | Tag-Connect SWD (no component, excluded from BOM) |
| J10 | 1 | Micro SD Card | SMD | Fab | Standalone SD card socket |
| J11, J12 | 2 | 1×3 socket | 2.54mm | User | Carrier mechanical (marked N/C) |

**Subtotal:** ~$2.50

*Note: Debug interface is Tag-Connect pogo pads only — no installed header. Developer needs TC2030-CTX-NL cable (~$40 one-time) for programming/debug.*

---

## SA818 Carrier Board

*Assembled separately — one per frequency band*

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U_RF | 1 | SA818-V or SA818-U | Module | Fab | VHF or UHF transceiver |
| J_SMA | 1 | SMA right-angle | PCB mount | Fab | Female, 50Ω |
| C_RF1 | 1 | 100nF | 0402 | Fab | Decoupling |
| C_RF2 | 1 | 10µF | 0603 | Fab | Bulk capacitor |
| R_ID | 1 | 0Ω | 0402 | Fab | Band select (GND=VHF, VCC=UHF) |
| J1 | 1 | 1×8 male header | 2.54mm | User | Signal header (left) |
| J2 | 1 | 1×8 male header | 2.54mm | User | Signal header (right) |
| J3 | 1 | 1×3 male header | 2.54mm | User | Mechanical (left) |
| J4 | 1 | 1×3 male header | 2.54mm | User | Mechanical (right) |
| ANT | 1 | Stubby antenna | SMA male | Included | Band-matched |

**Subtotal (per carrier):** ~$17.00
*(SA818 ~$10, SMA ~$2, antenna ~$4, PCB+parts ~$1)*

---

## Passive Component Totals

### Resistors

| Value | Qty | Package | Use |
|-------|-----|---------|-----|
| 100Ω | 2 | 0603 | PWR LED G/B (R15, R16) |
| 220Ω | 1 | 0603 | PWR LED R (R14) |
| 470Ω | 1 | 0603 | TX attenuator (R6) |
| 1kΩ | 1 | 0603 | Backlight gate (R11) |
| 2kΩ | 1 | 0603 | Charge current PROG1 (R3) |
| 4.7kΩ | 4 | 0603 | RX filter (R7, R8), I2C pullups (R1, R2) |
| 5.1kΩ | 2 | 0402 | USB CC (R_CC1, R_CC2) |
| 10kΩ | 3 | 0603 | Camera RST/PWDN (R12, R13), RUN pullup (R17) |
| 15kΩ | 1 | 0603 | Pre-charge/termination PROG3 (R4) |
| 47kΩ | 3 | 0603 | TX attenuator (R5), RX bias (R9, R10) |

### Thermistor

| Value | Qty | Package | Use |
|-------|-----|---------|-----|
| NTC 10k | 1 | 0603 | Temperature sensing, B=3380 (TH1) |

### Capacitors

| Value | Qty | Package | Use |
|-------|-----|---------|-----|
| 15pF | 2 | 0402 | Crystal load (C_Y1, C_Y2) |
| 4.7nF | 1 | 0603 | RX anti-alias (C27) |
| 10nF | 1 | 0603 | TX filter (C24) |
| 100nF | 36 | 0402/0603 | Decoupling: MCU (C3-C7, C9), LEDs (C28-C53), ICs |
| 1µF | 2 | 0402/0603 | DVDD (C1), RX DC block (C26) |
| 2.2µF | 2 | 0603 | PCM5102A charge pump (C21, C22) |
| 10µF | 12 | 0603/0805 | Bulk caps throughout |
| 100µF | 1 | 1206 | Load switch bulk (C17) |

### Inductors

| Value | Qty | Package | Use |
|-------|-----|---------|-----|
| 1µH | 1 | 3×3mm | TPS63001 buck-boost (L1) |
| 3.3µH | 1 | 3×3mm | RP2350 internal regulator (L2) |

---

## Sourcing Notes

### Recommended Suppliers

| Supplier | Best For | Lead Time |
|----------|----------|-----------|
| **LCSC** | Passives, WS2812B, common ICs | 1-2 weeks |
| **Digi-Key** | TI/Microchip ICs, precision parts | 2-3 days |
| **Mouser** | Alternative to Digi-Key | 2-3 days |
| **AliExpress** | Display, camera, SA818 modules | 2-4 weeks |
| **Amazon** | LiPo battery, lanyard, misc | 1-2 days |
| **JLCPCB** | PCB fab, SMT assembly | 1-2 weeks |

### Long Lead-Time Items (Order Early!)

- **SA818 modules** — 2-4 weeks from China
- **Custom PCBs** — 1-2 weeks from fab
- **OV2640 camera modules** — 1-3 weeks
- **ILI9341 display modules** — 1-3 weeks

### Kit Packaging Suggestions

- Group user-solderable parts in labeled bags by subsystem
- Include 2-3 spare 0603 resistors/caps (easy to lose)
- Print assembly instructions with component placement diagram
- Include test procedure checklist

---

## Cost Breakdown by Assembly Type

| Assembly | Component Cost | Notes |
|----------|---------------|-------|
| **Fab (SMT)** | ~$25 | ICs, small passives, LEDs |
| **User (TH)** | ~$12 | Switches, modules, headers |
| **Modules** | ~$16 | Display, camera |
| **Battery** | ~$6 | LiPo cell |
| **Carrier** | ~$17 | Separate board |
| **PCB** | ~$5 | Main + carrier at qty |
| **Total** | **~$82** | Complete kit with carrier |

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2024-XX | Initial BOM (outdated design) |
| 2.0 | 2025-02 | Complete rewrite: TPS63001, PCM5102A, ILI9341, WS2812B blinky, USB-C with ESD |
| 2.1 | 2025-02 | Debug interface changed to Tag-Connect TC2030-CTX-NL (pads only) |
| 2.2 | 2025-02 | Battery system detailed: MCP73871 charger, LC709203F fuel gauge, shared NTC |
| 2.3 | 2025-02 | MCU support: 12MHz crystal (required for USB), W25Q128 16MB flash |
| 2.4 | 2025-02 | Added BOOTSEL button for UF2 bootloader |
| 2.5 | 2025-02 | Added L2 (3.3µH) and C_DVDD (1µF) for RP2350 internal regulator |
| 3.0 | 2026-02 | **Synchronized with KiCad schematic**: Updated all ref designators to match KiCad, added R4 (15k PROG3), added C21/C22 (2.2µF charge pump), added J10 (SD card), fixed TH1 as NTC, removed PS/SYNC resistor (intentional), added I2C pullup warning |

---

*This BOM is generated from KiCad schematic and verified against `docs/electrical-design.md`.*
